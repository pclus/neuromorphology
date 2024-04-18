using DelimitedFiles, Statistics

for g in ["WT" "Ts65Dn" "TgDyrk1A"]
        all = zeros(750,100);
        for i in 0:99
            a= readdlm("isidist"*g*string(i)*".dat");
            all[:,i+1]=a[1:750]
        end
        
        writedlm("isidist"*g*"_I.dat", [mean(all,dims=2) std(all,dims=2)],' ' );
end
