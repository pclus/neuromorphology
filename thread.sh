#!/bin/bash

#for i in {WT,Ts65Dn,TgDyrk1A};
#do
#	./netdyn outputs/network${i}.dat sim${i} 9.0 1.0
#done
echo "network networks/net_${1}.dat"

for i in networks/net_${1}.dat;
do
	for j in {1..10};
	do
		nn=${i/networks\//}
		nn=${nn/.dat/}
		for k in {5..15};
		do
			./netdyn ${i} sim_${nn}_${j}_lam$k $k 1.0
		done
	done
done

