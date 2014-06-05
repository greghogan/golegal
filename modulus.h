unsigned char modulusdeltas[]={
  0, // 2^64                        1*64=  64 bits up to 2e19
  3, // 13 3889 364870227143809     2*64= 128 bits up to 3e38
  5, // 11 59 98818999 287630261    3*64= 192 bits up to 6e57
  7, // 3^2 818923289 2502845209    4*64= 256 bits up to 1e77
  9, // 7 9241 464773 613566757     5*64= 320 bits up to 2e96
 11, // 5 2551 1446236305269271     6*64= 384 bits up to 4e115
 15, // 53 348051774975651917       7*64= 448 bits up to 7e134
 17, // 19 67 14490765179661863     8*64= 512 bits up to 1e154
 33, // 827 3894899 5726879071      9*64= 576 bits up to 2e173
 35, // 17 72786899 14907938207    10*64= 640 bits up to 5e192
 39, // 139646831 132095686967     11*64= 704 bits up to 8e211
 45, // 11071 844709 1972539689    12*64= 768 bits up to 2e231
 47, // 31 173 9419 439771 830387  13*64= 832 bits up to 3e250
 53, // 29 74187931 8574098437     14*64= 896 bits up to 5e269
 57, // 41 163 269 8807 1165112831 15*64= 960 bits up to 1e289
 59, // 18446744073709551557       16*64=1024 bits up to 2e308
 63, // 401 119241217 385788209    17*64=1088 bits up to 3e327
 75, // 23 73 7283 1508546551513   18*64=1152 bits up to 6e346
 77, // 61 268909 1124564966411    19*64=1216 bits up to 1e366
 83, // 18446744073709551533       20*64=1280 bits up to 2e385
 87, // 14843 13973891 88936633    21*64=1344 bits up to 4e404
 89, // 17099 1078820052266773     22*64=1408 bits up to 7e423
 95, // 18446744073709551521       23*64=1472 bits up to 1e443
 99, // 397 46465350311610961      24*64=1536 bits up to 2e462
105, // 2617 7048813172987983      25*64=1600 bits up to 4e481
113, // 119026343 154980348121     26*64=1664 bits up to 8e500
117, // 363269 5077984654267       27*64=1728 bits up to 2e520
119, // 47 83 4728721885083197     28*64=1792 bits up to 3e539
125, // 315781601 58416145891      29*64=1856 bits up to 5e558
129, // 127 145249953336295681     30*64=1920 bits up to 9e577
143, // 113 383 521 260191 3144217 31*64=1984 bits up to 2e597
147, // 52561 8438519 41590091     32*64=2048 bits up to 3e616
153, // 14879 1239783861395897     33*64=2112 bits up to 6e635
155, // 76511 241099241595451      34*64=2176 bits up to 1e655
165, // 1291 14288725076459761     35*64=2240 bits up to 2e674
173, // 3183958073 5793651691      36*64=2304 bits up to 4e693
179, // 18446744073709551437       37*64=2368 bits up to 7e712
183, // 42239 7118119 61353713     38*64=2432 bits up to 1e732
189, // 18446744073709551427       39*64=2496 bits up to 2e751
195, // 4513 52363 78060237559     40*64=2560 bits up to 4e770
197, // 37 137 363529 10010559919  41*64=2624 bits up to 8e789
209, // 79 829811 281393099803     42*64=2688 bits up to 1e809
215, // 443 457 5023 18139972237   43*64=2752 bits up to 3e828
243, // 8004911 2304428378243      44*64=2816 bits up to 5e847
249, // 61001 302400683164367      45*64=2880 bits up to 9e462
255  // 97 673 282574471495681     46*64=2944 bits up to 2e462
};

#define NMODULI (int)(sizeof modulusdeltas)
