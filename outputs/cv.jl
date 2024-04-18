using DelimitedFiles
using Statistics

genotypes=["WT" "TgDyrk1A" "Ts65Dn"];
for g in genotypes
        stats = zeros(100,11,5)
        statsCV = zeros(11,5)
        for (i,sim) in enumerate(0:99)
        	for (jj,j) in enumerate(5:15)
        		filename = "from_cloud/ixi_0.3/isi_net_"*g*string(sim)*"_l"*string(j)*"_ixi.dat";
        
        		isi = readdlm(filename);
        		idE = findall(@. Int(isi[:,3])==1);
        		idI = findall(@. Int(isi[:,3])==-1);
        		isiE = isi[idE,2];
        		isiI = isi[idI,2];
        		me = mean(isiE)
        		se = std(isiE)
        		mi = mean(isiI)
        		si = std(isiI)
        		stats[i,jj,:]=[me, se, mi, si,j];
        	end
        end
        
        cve = stats[:,:,2]./stats[:,:,1]; 
        cvi = stats[:,:,4]./stats[:,:,3]; 

        statsCV[:,1] = 5:15; 
        statsCV[:,2] = mean(cve,dims=1)
        statsCV[:,3] = std(cve,dims=1)
        statsCV[:,4] = mean(cvi,dims=1)
        statsCV[:,5] = std(cvi,dims=1)

        cve = reshape(cve,:,1);
        cvi = reshape(cvi,:,1);
        λ = reshape(stats[:,:,5],:,1);

        writedlm("CVdata__"*g*"_ixi.dat", [λ cve cvi],' ')
        writedlm("CVstat_"*g*"_ixi.dat", statsCV,' ')
        
        
        #plot(stats[:,:,5],stats[:,:,2]./stats[:,:,1],lt=:scatter,legend=false);
        #plot!(stats[:,:,5],stats[:,:,4]./stats[:,:,3],lt=:scatter);
end

# command for gnuplot:
# plot 'CVdata_WT.dat'  w p,'' smoo uniq w p, 'CVstat_WT.dat' u 1:2:3 w errorba lc black
#
#

