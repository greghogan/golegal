#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "instream.h"
#include "modadd.h"

#define MAXSTREAMS 1024
#ifndef FANIN
#define FANIN 1
#endif

struct goin {
  uint64_t modulus;
  uint64_t totalin;
  statebuf stream[MAXSTREAMS];
  statebuf *heap[MAXSTREAMS];
  int nstreams;
  int ncpus;
  int cpuid;
};

uint64_t nstreams(goin *gin)
{
  return gin->nstreams;
}

uint64_t totalread(goin *gin)
{
  return gin->totalin;
}

uint64_t readdelta(statebuf *sb)
{
  int i=0,c;
  uint64_t delta=0LL;

  do {
    if ((c = fgetc(sb->fp)) == EOF) {
      printf("EOF during readdelta in file %s\n",sb->fname);
      exit(1);
    }
    delta |= (uint64_t)(c & 0x7f) << (7 * i++);
  } while (c & 0x80);
  return delta;
}

void fillbuf(goin *gin, statebuf *sb)
{
  uint64_t delta;
  int notok;

  delta = readdelta(sb);
  sb->state += delta;
  if (!fread(&sb->cnt,sizeof(uint64_t),1,sb->fp)) {
    printf("failed to read state %lo count from file %s\n", sb->state, sb->fname);
    exit(1);
  }
  modadd(gin->modulus, &sb->cumcnt, sb->cnt);
  if (sb->state == FINALSTATE) {
    notok = fclose(sb->fp);
    if (notok) {
      printf("Failed to close %s after reading\n",sb->fname);
      exit(1);
    }
    sb->fp = NULL;
    if (sb->cumcnt) {
      printf("file %s corrupt; cumcnt=%lu\n", sb->fname, sb->cumcnt);
      exit(1);
    }
  } else gin->totalin++;
}

void hpreplace(goin *gin, int i)
{
  statebuf *sb;
  int j;

  for (fillbuf(gin, sb=gin->heap[i]); (j=2*i+1) < gin->nstreams; i=j) {
    if (j+1 < gin->nstreams) {
      if (gin->heap[j+1]->state == gin->heap[j]->state) {
        if (gin->heap[j]->fp != 0) {
          modadd(gin->modulus, &gin->heap[j]->cnt, gin->heap[j+1]->cnt);
          hpreplace(gin, j+1);
        }
      } else if (gin->heap[j+1]->state < gin->heap[j]->state)
        j++;
    }
    if (sb->state > gin->heap[j]->state) {
      gin->heap[i] = gin->heap[j];
      gin->heap[j] = sb;
    } else if (sb->state == gin->heap[j]->state && sb->fp) {
      modadd(gin->modulus, &sb->cnt, gin->heap[j]->cnt);
      fillbuf(gin, sb = gin->heap[j]);
    } else break;
  }
}

void hpinsert(goin *gin, int i, statebuf *sb)
{
  int p;

  for (fillbuf(gin, gin->heap[i]=sb); (p=(i+1)/2-1) >= 0; i=p) {
    if (gin->heap[p]->state > sb->state) {
      gin->heap[i] = gin->heap[p];
      gin->heap[p] = sb;
    } else if (gin->heap[p]->state == sb->state && sb->fp) {
      modadd(gin->modulus, &gin->heap[p]->cnt, sb->cnt);
      hpreplace(gin, i);
      return;
    } else break;
  }
}

statebuf *minstream(goin *gin)
{
  return gin->heap[0];
}

void deletemin(goin *gin)
{
  hpreplace(gin, 0);
}

goin *openstreams(char *inbase, int ncpus, int cpuid, uint64_t modulus) {

  char inname[FILENAMELEN];
  statebuf *sb;
  FILE *fp;
  int from,to,j;
  goin *gin;

  gin = (goin *)calloc(1,sizeof(goin));
  if (gin == NULL) {
    printf ("Failed to allocate goin record of size %d\n", (int)sizeof(goin));
    exit(1);
  }
  gin->totalin = 0LL;
  gin->ncpus = ncpus;
  gin->cpuid = cpuid;
  gin->modulus = modulus;
  sb = &gin->stream[gin->nstreams = 0];
  for (from=0; from<FANIN*ncpus; from++) {
    for (to=cpuid; to<FANIN*ncpus; to+=ncpus) {
      for (j=0; ; j++) {
        sprintf(inname,"%s.%d.%d.%d",inbase,from,to,j); 
        if (!(fp = fopen(inname, "r"))) {
          break;
        }
        if (gin->nstreams == MAXSTREAMS) {
          printf ("#inputfiles exceeds MAXSTREAMS (%d)\n", MAXSTREAMS);
          exit(1);
        }
        // printf("opened %s\n", inname);
        sb->fp = fp;
        strncpy(sb->fname, inname, FILENAMELEN);
        hpinsert(gin, gin->nstreams++, sb++);
      }
    }
  }
  return gin;
}

#ifdef MAININSTREAM
#include "modulus.h"
#include <string.h>

int main(int argc, char *argv[])
{
  int wd,modidx,y,x;
  uint64_t totin,nin;
  uint64_t modulus;
  char inbase[64];
  statebuf *mb;
  goin gin;
  int ncpus, cpuid;

  if (argc!=7) {
    fprintf(stderr, "usage: %s width imod y x ncpus cpuid\n", argv[0]);
    exit(1);
  }
  wd = atoi(argv[1]);
  modidx = atoi(argv[2]);
  if (modidx < 0 || modidx >= NMODULI) {
    fprintf (stderr, "modulo_index %d not in range [0,%d)\n", modidx, NMODULI);
    exit(1);
  }
  modulus = -(uint64_t)modulusdeltas[modidx];
  y = atoi(argv[3]);
  x = atoi(argv[4]);
  ncpus = atoi(argv[5]);
  if (ncpus < 1 || ncpus > MAXCPUS) {
    fprintf (stderr, "#cpus %d not in range [0,%d]\n", ncpus, MAXCPUS);
    exit(1);
  }
  cpuid = atoi(argv[6]);
  if (cpuid < 0 || cpuid >= ncpus) {
    fprintf (stderr, "cpuid %d not in range [0,%d]\n", ncpus, ncpus-1);
    exit(1);
  }

  sprintf(inbase,"state.%d.%d.%d.%d",wd,modidx,y,x);
  gin = openstreams(inbase, ncpus, cpuid, modulus))
  if (!nstreams(gin))
    fprintf (stderr, "wanring: no input files\n");
  for (nin=0LL; (mb = minstream(gin))->state != FINALSTATE; nin++) {
    if (fwrite(&mb->state, sizeof(uint64_t),2,stdout) < 2) {
      printf("failed to write state+count\n");
      exit(1);
    }
    deletemin(gin)
  }
  totin = totalread(gin);
  fprintf(stderr, "(%d,%d) size %llu",y,x,nin);
  fprintf(stderr, " avg %1.3lf\n", totin/(double)nin);
  return 0;
}
#endif