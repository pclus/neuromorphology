#!/bin/bash

#for i in {WT,Ts65Dn,TgDyrk1A};
#do
#	./netdyn outputs/network${i}.dat sim${i} 9.0 1.0
#done

for i in results/net*dat;
do
	for j in {1..10};
	do
		nn=${i/results\//}
		nn=${nn/.dat/}
		./netdyn ${i} sim${j}_${nn} 9.0 1.0
	done
done

