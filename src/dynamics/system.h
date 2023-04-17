#ifndef STD_H
#define STD_H
        #include <stdio.h>
        #include <stdlib.h>
        #include <math.h>
        #include <time.h>
        #include <unistd.h> 
        #include <sys/types.h>
#endif

#ifndef GSL_H
#define GSL_H
        #include <gsl/gsl_fft_complex.h>
        #include <gsl/gsl_rng.h>
        #include <gsl/gsl_randist.h>
#endif

#define KMAX 100
#define RAND (gsl_rng_uniform(rrr))//((double)rand())/RAND_MAX 
#define RANDI(n) (gsl_rng_uniform_int(rrr,n))
#define RANDIFF (-1+2.*RAND)
#define RANDGAUSS (gsl_ran_gaussian(rrr,1))

#ifndef BOOL_H
#define BOOL_H
        typedef enum { false, true } bool;
#endif

// gsl rng
const gsl_rng_type * T;
gsl_rng * rrr;

int acti;

// network variables
int *degree,*neighbour, *inhibitory, *new_spyke, *coordx,*coordy, *label, *cumulative_degree;
double *weights; 
double *spyke_times;
double *Iexci, *Iinhi,*Iexte, *outputs;
double *Rexci, *Rinhi,*Rexte, *Routputs;
double excitatory_current, inhibitory_current,synaptic_extecurrent;
double excitatory_raisecurrent, inhibitory_raisecurrent,synaptic_raiseextecurrent;

// dynamics variables
double top,bottom;
double *x, *y, *x_0, *y_0, *aux_x, *aux_y, *v; 
double t,dt;
double Inoise;
double *IC;
double freqnoise, activity;
int N;
bool *external,*lfp;


int initialize_network(char *name);
void synapses(double t);
int system_initialize(int argc, char **argv);
int system_finish();
double f(double x, double y, double I);
double g(double x, double y, double a, double b);

int euler();
int initial_conditions();
int spyke_reseting(int i);
int shot_noise();
int relabel();

double lambda;
