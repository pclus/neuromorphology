using DelimitedFiles

n = length(500:10:4500);
psd_fil = zeros(2048,n);
psd_unfil = zeros(2048,n);
for (i,k) in enumerate(500:10:4500)
	a = readdlm("psd_"*string(k)*".dat");
	psd_fil[:,i] = a[:,3]
	psd_unfil[:,i] = a[:,2]
end
# df= 0.00199951
#
writedlm("heatmap.dat",psd_unfil,' ')
writedlm("heatmap_fil.dat",psd_fil,' ')

