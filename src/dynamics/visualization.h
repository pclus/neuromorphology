#ifndef STD_H
#define STD_H
        #include <stdio.h>
        #include <stdlib.h>
        #include <math.h>
        #include <time.h>
        #include <unistd.h> // needed for pipes
        #include <fcntl.h> // needed only for the nonblocking read
        #include <sys/types.h>
        #include <signal.h>
#endif

#ifndef SYSTEMSL_H
#define SYSTEMSL_H
        #include "system.h"
#endif


int vflag;
FILE *visual_initialize_network();
int visual_network(FILE *pip);

FILE *visual_initialize();
int visual_finish(FILE *pip);
int visual_plot(FILE *pip);

int visual_ts(FILE *pip);
FILE *visual_initialize_ts();

FILE *visual_initialize_video();
int visual_video(FILE *pip);
