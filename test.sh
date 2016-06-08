#!/bin/sh
#Assume https_enable==1

rm disabled.cfg enabled.cfg

echo "STAGE 1\n"
./deSTAbilizer  -v -v --cfg https_enable=0  -i SPA232D_1.3.5_cur.cfg -o disabled.cfg

#Enable it back
echo "STAGE 2\n"
./deSTAbilizer  -v -v --cfg https_enable=1 -i disabled.cfg -o enabled.cfg

xxd enabled.cfg > 1.dif
xxd  SPA232D_1.3.5_cur.cfg > 2.dif

diff 1.dif 2.dif
