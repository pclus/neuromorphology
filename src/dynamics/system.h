#ifndef STD_H
#define STD_H
        #include <stdio.h>
        #include <stdlib.h>
        #include <math.h>
        #include <time.h>
        #include <unistd.h> // needed for pipes
        #include <fcntl.h> // needed only for the nonblocking read
        #include <sys/types.h>
        #include <sys/wait.h>
        #include <signal.h>
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


///////////// GSL RNG ////////////////////
const gsl_rng_type * T;
gsl_rng * rrr;
////////////////////////////////////

/* LEAK CONTROL */
int alloc_number, free_number;
int acti;

/* NETWORK */
int *degree,*neighbour, *inhibitory, *new_spyke, *coordx,*coordy, *label, *cumulative_degree;
double *weights; 
double *spyke_times;
double *Iexci, *Iinhi,*Imini, *outputs;
double *Rexci, *Rinhi,*Rmini, *Routputs;
double excitatory_current, inhibitory_current,synaptic_minicurrent;
double excitatory_raisecurrent, inhibitory_raisecurrent,synaptic_raiseminicurrent;
/*-------*/

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
int initial_conditions(double ampl);
int spyke_reseting(int i);
int read_coordinates(char *name);
int shot_noise();
int relabel();

double ornstein_uhlenbeck(double x);
double lambda;
