#!/bin/bash

for i in {1..10};
do
	./nettopology 16 156.30 40 500 6 1      #WT
	cp outputs/network.dat networks/net_WT${i}.dat
	./nettopology 16 100.67 40 500 6 0.937  #Ts65Dn
	cp outputs/network.dat networks/net_Ts65Dn${i}.dat
	./nettopology 16 93.24 40 500 6 0.826   #TgDyrk1A
	cp outputs/network.dat networks/net_TgDyrk1A${i}.dat
done

