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
#endif

#ifndef SYSTEMSL_H
#define SYSTEMSL_H
        #include "system.h"
#endif

#ifndef VISUAL_H
#define VISUAL_H
        #include "visualization.h"
#endif

#ifndef BOOL_H
#define BOOL_H
	typedef enum { false, true } bool;
#endif

bool visual;
bool current;

int child_thread(int *pfd);
int parent_thread(int argc, char **argv, int *pdf);
char visualize(char c, int count, int *pfd, FILE *pip, FILE *pip2);
