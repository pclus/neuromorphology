#!/bin/bash

FN=$1
FOUT=$2
awk '{if(A[$2]==""){A[$2]=1}else{A[$2]++; B[$2]=$3}}; END {for(i in A){print i,A[i],B[i]}}' $FN > rates_${FOUT}.dat
awk '{if(A[$2]==""){ }else{print $2,$1-A[$2],$3;}; A[$2]=$1}' $FN > isi_${FOUT}.dat

