#!/bin/sh

compile() {
  if [ $# -ne 4 ] ; then 
    echo "bad usage: <nr> <p1> <p2> <p3>"
    exit 1
  fi

  # in ego/playout.cpp use LOCAL_P1, LOCAL_P2, LOCAL_P3
  # for simpler compilation many posibilities.

  g++ \
    -ggdb3 -Wall -Wextra -Wswitch-enum -Wunused -O3 \
    -fomit-frame-pointer -frename-registers -ffast-math \
    -I ego -DLOCAL_P1=$2 -DLOCAL_P2=$3 -DLOCAL_P3=$4 \
    -o locality-v0.1-p$1 example/main.cpp ego/ego.cpp || exit 1
    #-march=native # why compilation crashes with this?
}

#compile 01 0.10  0.10  0.10  &
#compile 02 0.25  0.25  0.25  &
#compile 03 0.33  0.33  0.33  &
#compile 04 0.50  0.50  0.50  &
#wait
#compile 05 0.66  0.66  0.66  &
#compile 06 0.75  0.75  0.75  &
#compile 07 0.90  0.90  0.90  &

#compile 08 0.100 0.055 0.059 &
#wait
#compile 09 0.150 0.094 0.104 &
#compile 10 0.200 0.125 0.143 &
#compile 11 0.250 0.173 0.210 &
#compile 12 0.330 0.254 0.340 &
#wait
#compile 13 0.400 0.333 0.500 &
#compile 14 0.500 0.500 0.100 &

#compile 15 0.100 0.111 0.125 &
#compile 16 0.150 0.176 0.214 &
#wait
#compile 17 0.200 0.250 0.333 &
#compile 18 0.250 0.333 0.500 &
#compile 19 0.330 0.492 0.970 &

#compile 20 0.100 0.078 0.084 &
#wait
#compile 21 0.150 0.118 0.133 &
#compile 22 0.200 0.162 0.194 &
#compile 23 0.250 0.227 0.293 &
#compile 24 0.330 0.328 0.489 &
#wait

compile 25 0.15  0.15  0.15  &
compile 26 0.20  0.20  0.20  &
compile 27 0.27  0.27  0.27  &
compile 28 0.30  0.30  0.30  &
wait

compile 29 0.270 0.185 0.227 &
compile 30 0.300 0.214 0.273 &
compile 31 0.350 0.269 0.368 &
compile 32 0.370 0.294 0.416 &
wait

compile 33 0.166 0.199 0.248 &
compile 34 0.184 0.225 0.291 &
compile 35 0.216 0.275 0.380 &
compile 36 0.234 0.305 0.440 &
wait

compile 37 0.216 0.132 0.152 &
compile 38 0.184 0.149 0.176 &
compile 39 0.216 0.184 0.225 &
compile 40 0.234 0.204 0.256 &
wait

