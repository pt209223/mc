#!/bin/sh

if [ $# -ne 5 ] ; then
	echo "$0 <first-engine> <second-engine> <file> <games-01> <games-10>"
	exit 1
fi

if [ $4 -gt 0 ] ; then
	gogui-twogtp -black "$1" -white "$2" -auto -size 9 -komi 6.5 -games $4 -sgffile $3-01 
fi

if [ $5 -gt 0 ] ; then
	gogui-twogtp -black "$2" -white "$1" -auto -size 9 -komi 6.5 -games $5 -sgffile $3-10 
fi

exit 0

