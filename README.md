# Neuromorphology alterations in Down Syndrome: a computational model of the motor cortex

This repository contains the data and software used in the scientific paper [Modeling the impact of neuromorphological alterations in Down syndrome on fast neural oscillations](https://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1012259).

## Data

Data used in the article can be found in the `data` directory.

The directory contains three files with the Sholl analysis of the 6 reconstructed
neurons for each animal model.
First column of each `sholls_<type>.dat` file is the distance from the
soma center (in micrometers), and the next six columns are the number
of intersections for each neuron, obtained from the Sholl analysis.
Refer to the paper Methods section for more details.

The `spines.dat` file contains 4 columns:
	1. Distance from soma in μm
	2. Spine density per 10μm corresponding to wild type (WT), from [Dierssen et al. (2003)](doi.org/10.1093/cercor/13.7.758)
	3. Spine density per 10μm corresponding to trisomic mice (Ts65Dn) from [Dierssen et al. (2003)](doi.org/10.1093/cercor/13.7.758)
	4. Spine density per 10μm corresponding to transgenic mice (TgDyrk1A) from [Martinez de Lagran et al. (2012)](doi.org/doi:10.1093/cercor/bhr362)

## Topology generation

Network topologies for each phenotype are generated
using the C source code `nettopology.c` and `nettopology.h` provided in `src/topology`.
This software requires the [Gnu Scientific Library (GSL)](https://www.gnu.org/software/gsl/doc/html/index.html).

The software can be compiled from `src` with:

```
gcc nettopology.c -o ../nettopology -O3 -lm -lgsl -lgslcblas
```

The software requires 6 inputs, although only 2 of them are used in the paper:
1. soma radius, fixed to 16μm.
2. average radius of the dendritic tree, which depends on the phenotype.
3. Standard deviation of the dendritic tree radius, which is fixed to 40μm
4. Mean axon length, fixed to 500μm
5. Standard deviation of the angle change for the axon growth, fixed to 6 degrees.
6. Scale of the spine density (parameter α in the paper).

Therefore, calls to produce networks for each phenotype follow:

```
./nettopology 16 156.30 40 500 6 1 	#WT
./nettopology 16 100.67 40 500 6 0.937 	#Ts65Dn
./nettopology 16 93.24 40 500 6 0.826	#TgDyrk1A
```

Upon success different output files will be generated in the `outputs` folder. From these files,
 `network.dat` contains the generated weighted directed network topology in a three column format `i j w_ij`, meaning that postsynaptic neuron `i` receives input from presynaptic neuron `j`
with `w_ij` contacts. First row of the file is the number of neurons.

## Dynamical model

For details on the model and the parameters refer to the article.
The code is in the folder `src/dynamics` and can be compiled with the provided `makefile`
just typing `make` in a terminal within the directory.
This software requires the [Gnu Scientific Library (GSL)](https://www.gnu.org/software/gsl/doc/html/index.html).

The executable `netdyn` requires 4 inputs

1. Network topology where to run the simulation, with the format specified in the previous section.
2. Flag to identify output files
3. External firing rate input to single neurons `lambda` in spikes per milisecond.
4. Ratio of overinhibition, defaults to 1.

Thus, an exemplary call to the software reads:

```
./netdyn outputs/network.dat simulation1 9.0 1.0
```

The software produces two output files, one containing the time series of the firing rate of the network (spikes per time step, `dt=0.005`), and the other the simulated local field potential. 




