./netdyn networks/net_WT1.dat simWT_lam6_ixi 6.0 0.3
./netdyn networks/net_TgDyrk1A1.dat simTgDyrk1A_lam6_ixi 6.0 0.3
./netdyn networks/net_Ts65Dn1.dat simTs65Dn_lam6_ixi 6.0 0.3

./netdyn networks/net_WT1.dat simWT_lam6 6.0 1.0
./netdyn networks/net_TgDyrk1A1.dat simTgDyrk1A_lam6 6.0 1.0
./netdyn networks/net_Ts65Dn1.dat simTs65Dn_lam6 6.0 1.0

# topology:
# alpha 0.1 and 2
# beta 0.5 and 2

./nettopology 16 78.15 40 500 6 0.1 
cp outputs/network.dat networks/net_alpha0.1_beta0.5
./nettopology 16 312.6 40 500 6 0.1
cp outputs/network.dat networks/net_alpha0.1_beta2
./nettopology 16 78.15 40 500 6 2
cp outputs/network.dat networks/net_alpha2_beta0.5
./nettopology 16 312.6 40 500 6 2
cp outputs/network.dat networks/net_alpha2_beta2

for i in networks/net_alpha*;
do
	name="$(basename -- $i)"
	./netdyn $i sim_${name} 9.0 1.0
done

