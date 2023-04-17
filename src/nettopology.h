#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef GSL_H
#define GSL_H
        #include <gsl/gsl_rng.h>
        #include <gsl/gsl_randist.h>
#endif

#define RAND (gsl_rng_uniform(rrr))             // random double between 0 and 1
#define RANDI(n) gsl_rng_uniform_int(rrr,n);    // random integer between 0 to n-1 inclusive
#define GAUSSIAN(mu,sigma) (mu+gsl_ran_gaussian(rrr, sigma))
#define RAYLEIGH(sigma) gsl_ran_rayleigh(rrr, sigma)

#define LATTICE(x,y) lattice[(x)*ny+y]
#define SQUARE(x) (x-floor(x) > 0.5 ? floor(x)+1 : floor(x))

// ------- GSL RNG --------
const gsl_rng_type * T;
gsl_rng * rrr;


typedef enum { excitatory, inhibitory } neuron_type;
typedef enum { false, true } bool;

// basic struct to build the network
typedef struct neuron{

        // BASIC IDENTIFIERS
        neuron_type type;       // inhibitory or excitatory

        // SPATIAL VARIABLES
        int coord_x;            // center of the soma, coordinate x
        int coord_y;            // center of the soma, coordinate y
        int dendritic_radius;   // denditric tree radius
        int axon_length;

        // NETWORK VARIABLES 
        int k_out;              // number of outputs to other neurons
        int *weights_out;
        int *neighbours_out;    // list of neighbours of the neuron

}Neuron;

typedef struct smallsquare{
	int *list;
	int n;
}Square;


int nx,ny,N;
Neuron *neu; 	//array of neurons forming the graph
Square *lattice;
int max_list, max_radius;
int soma_radius,mean_dr,sigma_dr,sigma_a,sigma_angle;
double scale, spine_x;

int initialize(int argc,char **argv);
void finish();
void create_neurons();
void paint_lattice(int i);
void grow_axons();
bool synapse(double radius, double rr);
double alpha(double r, double rr);
void create_link(int i, int j);
void outputs();
