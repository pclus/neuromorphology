#!/bin/bash


for i in {1..10};
do
	./nettopology 16 156.30 40 500 6 1      #WT
	mv outputs/network.dat results/netWT${i}.dat
	./nettopology 16 100.67 40 500 6 0.937  #Ts65Dn
	mv outputs/network.dat results/netTs65Dn${i}.dat
	./nettopology 16 93.24 40 500 6 0.826   #TgDyrk1A
	mv outputs/network.dat results/netTgDyrk1A${i}.dat
done
