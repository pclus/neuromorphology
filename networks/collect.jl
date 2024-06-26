using DelimitedFiles, Statistics

for g in ["WT" "Ts65Dn" "TgDyrk1A"]
        all = zeros(60,10);
        bll = zeros(60,10);
        cll = zeros(60,10);
        for i in 1:10
            a= readdlm("indegree_net_"*g*string(i)*".dat";skipstart=4);
	    idx = Int.(a[:,1]/10).+1
            all[idx,i]=a[:,2]

            b= readdlm("outdegree_net_"*g*string(i)*".dat";skipstart=4);
	    idx = Int.(b[:,1]/10).+1
            bll[idx,i]=b[:,2]

            c= readdlm("strength_net_"*g*string(i)*".dat";skipstart=4);
	    idx = Int.(c[:,1])
            cll[idx,i]=c[:,2]
        end
        
        writedlm("alldist"*g*".dat", [mean(all,dims=2) std(all,dims=2) mean(bll,dims=2) std(bll,dims=2) mean(cll,dims=2) std(cll,dims=2)],' ' );
end



