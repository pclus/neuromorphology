#!/bin/bash

for i in {WT,Ts65Dn,TgDyrk1A};
do
	./netdyn outputs/network${i}.dat sim${i} 9.0 1.0
done
