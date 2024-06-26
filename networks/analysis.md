In order to get the number of synapses, is just the number of entries in each file (the result of `wc -l` minus 1).

In order to get the in-degree for each neuron:
```
plot 'net_WT1.dat' skip 1 u 1:(1) w d smoo freq
```

The out-degree is:

```
plot 'net_WT1.dat' skip 1 u 2:(1) w d smoo freq
```

Degree distribution:
```
do for[g in "WT Ts65Dn TgDyrk1A"]{
do for[k=1:10]{
	@reaxis
	net='net_'.g.k.'.dat'
	deg = 'degree_'.net
	outdeg = 'out'.deg
	indeg = 'in'.deg

	set table 'in.dat'
	plot net skip 1 u 1:(1) w d smoo freq
	unset table 
	set table 'out.dat'
	plot net skip 1 u 2:(1) w d smoo freq
	unset table
	system('paste in.dat out.dat > '.deg)
	
	set table indeg
	plot deg u (bin($2,10)):(1.0) smoo freq w lp # in-degree
	set table outdeg
	plot deg u (bin($5,10)):(1.0) smoo freq w lp # out-degree
	unset table
	
}}

# use julia code 'collect.jl'

plot 'alldistWT.dat' u ($0*10):($1):($2) w errorba t 'indegree', 'alldistWT.dat' u ($0*10):($3):4 w errorba t 'outdegree'

```


synaptic strength distribution:


```
do for[g in "WT Ts65Dn TgDyrk1A"]{
do for[k=1:10]{
        @reaxis
        net='net_'.g.k.'.dat'
        netstre = 'strength_'.net

        set table netstre
	plot net skip 1 u 3:(1.0) smoo freq
}}
unset table

plot 'alldistWT.dat' u ($0+1):5


gnuplot> stats 'alldistWT.dat' u 5 nooutput; print STATS_sum
608777.6
gnuplot> stats 'alldistTs65Dn.dat' u 5 nooutput; print STATS_sum
365337.9
gnuplot> stats 'alldistTgDyrk1A.dat' u 5 nooutput; print STATS_sum
334134.3

```



```
set print "stats_WT.dat"
do for[k=1:10]{
	if(k==1){
		print "# num_of_synapses av_deg std_deg average_strength_of_synapses average_stddev_of_synapses"
	}
	stats 'net_WT'.k.'.dat' skip 1 u 1:3 nooutput;
	a = STATS_mean_y
	b = STATS_stddev_y
	set table $data
	plot 'net_WT'.k.'.dat' skip 1 u 1:(1) w d smoo freq
	unset table
	stats $data nooutput;
	print STATS_sum_y,STATS_mean_y,STATS_stddev_y,a,b
}

set print "stats_Ts65Dn.dat"
do for[k=1:10]{
	if(k==1){
		print "# num_of_synapses av_deg std_deg average_strength_of_synapses average_stddev_of_synapses"
	}
        stats 'net_Ts65Dn'.k.'.dat' skip 1 u 1:3 nooutput;
        a = STATS_mean_y
        b = STATS_stddev_y
        set table $data
        plot 'net_Ts65Dn'.k.'.dat' skip 1 u 1:(1) w d smoo freq
        unset table
        stats $data nooutput;
        print STATS_sum_y,STATS_mean_y,STATS_stddev_y,a,b
}

set print "stats_TgDyrk1a.dat"
do for[k=1:10]{
	if(k==1){
		print "# num_of_synapses av_deg std_deg average_strength_of_synapses average_stddev_of_synapses"
	}
        stats 'net_TgDyrk1A'.k.'.dat' skip 1 u 1:3 nooutput;
        a = STATS_mean_y
        b = STATS_stddev_y
        set table $data
        plot 'net_TgDyrk1A'.k.'.dat' skip 1 u 1:(1) w d smoo freq
        unset table
        stats $data nooutput;
        print STATS_sum_y,STATS_mean_y,STATS_stddev_y,a,b
}
set print
```

