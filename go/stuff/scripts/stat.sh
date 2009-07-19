#!/bin/sh

whenempty=""

if [ $# -eq 0 ] ; then
	whenempty="./"
fi

for prefix in $@ $whenempty ; do
	bw1=`cat $prefix*01.dat 2>/dev/null | grep -c 'B+R'`
	bw2=`cat $prefix*01.dat 2>/dev/null | grep -c 'W+R'`
	wb1=`cat $prefix*10.dat 2>/dev/null | grep -c 'W+R'`
	wb2=`cat $prefix*10.dat 2>/dev/null | grep -c 'B+R'`

	echo "$prefix    -> [   $bw1 : $bw2 (B:W)   ] + [   $wb1 : $wb2 (W:B)   ] = [   $(($bw1+$wb1)) : $(($bw2+$wb2)) (Summary) ]"
done

