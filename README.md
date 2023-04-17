# Neuromorphology alterations in Down Syndrome: a computational model of the motor cortex

## Data

Data used in the article can be found in the `data` directory.

The directory contains three files with the Sholl analysis of the 6 reconstructed
neurons for each phenotypes.
First column of each `sholls_<type>.dat` file is the distance from the
soma center (in micrometers), and the next six columns are the number
of intersections for each neuron, obtained from the Sholl analysis.
Refer to the paper Methods section for more details.

The `spines.dat` file contains 4 columns:
	1. Distance from soma in μm
	2. Spine density per 10μm corresponding to wild type (WT).
	3. Spine density per 10μm corresponding to trisomic mice (Ts65Dn).
	4. Spine density per 10μm corresponding to transgenic mice (TgDyrk1A).

## Topology generation

Network topologies for each phenotype are generated
using the C source code `nettopology.c` and `nettopology.h` provided in `src`.
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

## Dynamical model

