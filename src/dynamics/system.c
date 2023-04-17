#ifndef SYSTEMSL_H
#define SYSTEMSL_H
        #include "system.h"
#endif

#define INHI2INHI (in2in) 

#define ALPHAexci (6e-3) 
#define ALPHAinhi (720e-3) 
#define ALPHAmini (8e-3) 

#define TAUexci 2.0
#define TAUinhi 4.0
#define TAUmini 2.0

#define STRENGTH (1.0)

#define EXCI_INPUT(t) (ALPHAexci*exp(-(t)/TAUexci)/TAUexci)
#define INHI_INPUT(t) (ALPHAinhi*exp(-(t)/TAUinhi)/TAUinhi)
#define MINI_INPUT(t) (ALPHAmini*exp(-(t)/TAUmini)/TAUmini)

#define EXCI_INIT_INPUT(t) (exp(-(t)/TAUexci))
#define INHI_INIT_INPUT(t) (exp(-(t)/TAUinhi))
#define MINI_INIT_INPUT(t) (exp(-(t)/TAUmini))

#define EE (0)
#define EI (-70.0)

#define DELAY(j,k) (delay[(k)*N+(j)])

double strength;
double *delay;
double in2in, alphaInhi;
int id_delay, max_id_delay, Nex;
double renorm;

int system_initialize(int argc, char **argv){
	if(argc<6){
		fprintf(stderr,"USAGE: ./evolve <network> <outputs> <Iconstant> <Inoise> <overinhi>\n");
		return -1;
	}
	
	t=0;
	//dt=0.02;
	dt=0.005;
	if(initialize_network(argv[1])==-1){
		fprintf(stderr,"Errors in system_stuartlanday.c, program breaking.\n");
		return -1;
	}
	
	alloc_number=0;
	free_number=0;

//	///////////// NOISE //////////////////////
	double diff; //=0.01;
	diff=atof(argv[4]);
	freqnoise=atof(argv[5]);
	lambda=0;
	Inoise=sqrt(6.*dt*2*diff);
//	///////////// GSL RNG ////////////////////
	gsl_rng_env_setup();
	
	T = gsl_rng_default;
	rrr = gsl_rng_alloc(T);
	
	// Different perturbation	
	//srand(time(0)*getpid());
	//gsl_rng_set(rrr,time(0)*getpid());

	srand(123);
	gsl_rng_set(rrr,123);

	// Same perturbation always
//	int flag=atoi((argv[6]));
	//int flag=0; //atoi((argv[6]));
	//srand(flag);
	//gsl_rng_set(rrr,flag);
	////////////////////////////////////////

	fprintf(stderr,"Initializing with N=%d neurons\n",N);

	x=(double *)malloc(sizeof(double)*N);				
	y=(double *)malloc(sizeof(double)*N);				
	x_0=(double *)malloc(sizeof(double)*N);				
	y_0=(double *)malloc(sizeof(double)*N);				
	aux_x=(double *)malloc(sizeof(double)*N);			
	aux_y=(double *)malloc(sizeof(double)*N);			
	IC=(double *)malloc(sizeof(double)*N);				
	spyke_times=(double *)malloc(sizeof(double)*N);			
	coordx=(int *)malloc(sizeof(int)*N);				
	coordy=(int *)malloc(sizeof(int)*N);				
	label=(int *)malloc(sizeof(int)*N);				
	Iexci=(double *)malloc(sizeof(double)*N);		
	Iinhi=(double *)malloc(sizeof(double)*N);	
	Imini=(double *)malloc(sizeof(double)*N);
	outputs=(double *)malloc(sizeof(double)*N);
	external=(bool *)malloc(sizeof(bool)*N);
	//lfp=(bool *)malloc(sizeof(bool)*N);
	

	//alphaInhi=atof(argv[6]);
	in2in=atof(argv[6]);
	//in2in=1; //atof(argv[6]);
	int nn=N/dt, i; 
	max_id_delay=(int) 1./dt;
	id_delay=0;
	fprintf(stderr,"\n\n %d %d %d\n\n",nn, max_id_delay, id_delay);
	delay=(double *)malloc(sizeof(double)*nn);
	for(i=0;i<nn;i++) delay[i]=0;	

	acti=0;

	excitatory_current=EXCI_INIT_INPUT(dt);
	inhibitory_current=INHI_INIT_INPUT(dt);
	synaptic_minicurrent=MINI_INIT_INPUT(dt);
	
	top=ALPHAexci/TAUexci;
	bottom=ALPHAinhi/TAUinhi;

	fprintf(stderr,"Currents:\n\texci=%lf\n\tinhi=%lf\n\tmini=%lf\n",excitatory_current,inhibitory_current,synaptic_minicurrent);
        fprintf(stderr,"Strength:\n\texci=%lf\n\tinhi=%lf\n\tmini=%lf\n",ALPHAexci,ALPHAinhi,ALPHAmini);


	read_coordinates(argv[7]);
	initial_conditions(atof(argv[3]));
	relabel();
	fprintf(stderr,"Initialization done\n");

	return 1;
}

int initial_conditions(double ampl){
	int i;
	Nex=0;
	for(i=0;i<N;i++){
		inhibitory[i]=( RAND < 0.2 ? -1 : 1);
		Nex+= (inhibitory[i]==1 ? 1 : 0);
		Iexci[i]=Iinhi[i]=Imini[i]=0;
		//external[i]=(RAND < 0.2 ? true : false);
		//external[i]=(coordx[i] < 500 && coordy[i]<500 && inhibitory[i]==1 ? true : false);
		//external[i]=(inhibitory[i]==1 ? true : false);
		external[i]=true; //(coordx[i] < 500 && coordy[i]<500 && inhibitory[i]==1 ? true : false);
		IC[i]=ampl;
		//IC[i]=(external[i] ? ampl : 0);
		//if(inhibitory[i]==-1) fprintf(stderr,"Inhi  %d\n",i);
	}

	for(i=0;i<N;i++){
		if(RAND<0.7) // unactive neurons
		//if(coordx[i]<1000 && coordy[i]<1000) // unactive neurons
			x[i]=-65+40*(0.5-RANDIFF);
		else
			x[i]=35;
		y[i]=-8+4*(0.5-RANDIFF);
		new_spyke[i]=spyke_reseting(i);
	}
	return 1;
}

int relabel(){
        int i,j=0;
        for(i=0;i<N;i++){
                if(inhibitory[i]==-1){
                        label[i]=j;
                        j++;
                }
        }
        for(i=0;i<N;i++){
                if(inhibitory[i]==1){
                        label[i]=j;
                        j++;
                }
        }
        return 0;
}


int initialize_network(char *namenet){
        int i,cumulative=0,j=0;
        int a,b, maxdegree=-1;
	double c;


        FILE *fnet=fopen(namenet,"r");

	if(fnet==NULL){
		fprintf(stderr,"ERROR (initialize_network()): File %s not found!\n",namenet);
		return -1;
	}
	fprintf(stderr,"Initializing: proper ordering of %s is not checked authomatically\n",namenet);

        fscanf(fnet,"%d\n",&N);
	fprintf(stderr,"Reading %d nodes of %s\n",N,namenet);
        degree=(int *) malloc(sizeof(int)*N);				alloc_number++;
        cumulative_degree=(int *) malloc(sizeof(int)*(N+1));		alloc_number++;
        for(i=0;i<N;i++){
		degree[i]=cumulative_degree[i]=0;
	}
        while(fscanf(fnet,"%d %d %lf\n",&a,&b,&c)!=EOF){
                if(a<0 || a>N || b<0 || b>N){
                        fprintf(stderr,"ERROR (initialize_network()): Index out of range\n a=%d\nb=%d\n",a,b);
                        return -1;
                }
                degree[a]++;
		if(degree[a]>maxdegree) maxdegree=degree[a];
		cumulative++;
        }
	for(i=1;i<=N;i++) cumulative_degree[i]+=cumulative_degree[i-1]+degree[i-1];
        fclose(fnet);

        weights=(double *) malloc(sizeof(double)*cumulative);	alloc_number++;
        neighbour=(int *) malloc(sizeof(int)*cumulative);	alloc_number++;
        fnet=fopen(namenet,"r");
        fscanf(fnet,"%*d\n");

	strength=STRENGTH; //1./30.0;
	j=0;
	renorm=0;
        while(fscanf(fnet,"%d %d %lf\n",&a,&b,&c)!=EOF){
               	neighbour[j]=b;
		//weights[j]=c;
		//weights[j]=c/(N*1.0);
		weights[j]=c*strength;
                j++;
		renorm+=c*strength;
        }
        fclose(fnet);
        if(j!=cumulative){
                fprintf(stderr,"ERROR (initialize_network()): Missmatch between j=%d and cumulative=%d\n",j,cumulative);
                return -1;
        }

        inhibitory=(int *) malloc(sizeof(int)*N);	alloc_number++;
        new_spyke=(int *) malloc(sizeof(int)*N);	alloc_number++;

//	/***** check ************/
//	int jj,w;
//	FILE *ffnet=fopen("provanet.dat","w");
//	for(i=0;i<N;i++){
//		for(j=cumulative_degree[i];j<cumulative_degree[i+1];j++){
//			jj=neighbour[j];
//			w=weights[j];
//			fprintf(ffnet,"%d %d %d\n",i,jj,w);
//		}
//	}
//	fclose(ffnet);
//	/***********************/

	fprintf(stderr,"Done\n");
        return 1;
}
int read_coordinates(char *name){
	int a,b,i;
	FILE *fcoord=fopen(name,"r");
	if(fcoord==NULL){
		fprintf(stderr,"WARNING: File '%s' not found: using random numbers\n",name);
		for(i=0;i<N;i++){
			coordx[i]=RANDI(1500);
			coordy[i]=RANDI(1500);
		}
	}
	else{
		for(i=0;i<N;i++){
			fscanf(fcoord,"%d %d\n",&a,&b);
			coordx[i]=a;
			coordy[i]=b;
		}
		fclose(fcoord);
	}
	double aa,bb;
	for(i=0;i<N;i++)
	{
		aa=coordx[i]-750;
		bb=coordy[i]-750;
	//	lfp[i]=(sqrt(aa*aa+bb*bb)<300 ? true : false );
	}
	

	return 1;
}


int system_finish(){
	free(x);		
	free(y);		
	free(x_0);		
	free(y_0);		
	free(aux_x);		
	free(aux_y);		
	free(spyke_times);	
	free(new_spyke);	
	free(weights); 		
	free(degree);		
	free(cumulative_degree); 
	free(neighbour);	
	free(inhibitory); 	
	free(IC);		
	//free(lfp);		
	free(coordx); free(coordy);
	free(label);
	free(Iexci); free(Iinhi); free(Imini);
	free(outputs);
	free(external);
	free(delay);
	return 1;
}



// IZHIKEVICH MODEL: a is the recovery speed, b is the sensitivity to membrane potential, d 
double f(double v, double w, double I){
	return 0.04*v*v+5*v+140-w+I;
}
double g(double v, double w, double a, double b){
	return a*(b*v-w);
}



int euler(){
	int i;
	double a,b=0.2,k,l;
	activity=0;
	synapses(t);
        for(i=0;i<N;i++){
                a=(inhibitory[i]==1 ? 0.02 : 0.1);
                k=f(x[i],y[i],(Iexci[i]+Imini[i])*(EE-x[i])-Iinhi[i]*(EI-x[i])+IC[i]);  // using reversal potential
                //k=f(x[i],y[i],Iexci[i]+Iinhi[i]+Imini[i]+IC[i]);
                l=g(x[i],y[i],a,b);
		x_0[i]=x[i];
                x[i]=x[i]+dt*k; //+Inoise*RANDIFF;
                y[i]=y[i]+dt*l;
		//if(inhibitory[i]==1 && lfp[i])
		if(inhibitory[i]==1)
			activity+=fabs((Iexci[i]+Imini[i])*(EE-x[i]))+fabs(Iinhi[i]*(EI-x[i]));
                new_spyke[i]=spyke_reseting(i);
        }
	activity/=(1.*Nex);
	//lambda+=dt*ornstein_uhlenbeck(lambda);
        t+=dt;
        return 0;
}


int spyke_reseting(int i){
	int r=0;
	double ts;
	bool inhi=(inhibitory[i]==-1);
	
    	if(x[i]>30){
		int j,jj; 
		double w, current;
		//double d=8.0; 
		double d=(inhi ? 2.0 : 8.0 );	// reset of y accordingly

		ts=t+dt*(30-x_0[i])/(x[i]-x_0[i]); 	// linear interpolation
    		spyke_times[i]=ts;			// store proper timing
    		DELAY(i,id_delay)=ts;			// store proper timing
		//-------just for visualization ---------------
                if(inhi)
                          current=-INHI_INPUT(t-ts)/inhibitory_current;
                else
                          current=EXCI_INPUT(t-ts)/excitatory_current;
                outputs[i]+=current;
		//----------------------------------------------

    		x[i]=-65;
    		y[i]=y[i]+d; // Reset of recovery variable
    		r=1;

		acti++;
    	}
	id_delay=(id_delay+1 < max_id_delay ? id_delay+1 : 0);

    	return r;
}

double ornstein_uhlenbeck(double x){
	double sigma=0.4; //[spykes/second]
	double tau=16; //[ms]
	return (freqnoise-x+sigma*sqrt(2./tau)*RANDGAUSS);
}

void synapses(double t){
	int i,j,jj;
	bool inhi;
	double w,current,ts;

	for(i=0;i<N;i++){
		inhi=(inhibitory[i]==-1);
		ts=DELAY(i, id_delay);
		DELAY(i, id_delay)=-1;	// empty the matrix for nexts steps
		if(ts > 0){
			if(inhi)
				current=-INHI_INPUT(t-ts-1)/inhibitory_current;
			else
				current=EXCI_INPUT(t-ts-1)/excitatory_current;

			for(j=cumulative_degree[i];j<cumulative_degree[i+1];j++){
                        	jj=neighbour[j];
                        	w=weights[j];
                        	if(inhi)
				{
					if(inhibitory[jj]==-1)
					{
						Iinhi[jj]+=INHI2INHI*w*current;
					}
                        	        else
					{
						Iinhi[jj]+=w*current;
					}
				}	
                        	else
				{
                        	        Iexci[jj]+=w*current;
				}
                	}
		}
		// external noisy input
		//if(external[i])
			if(shot_noise()){
				ts=t+RAND*dt;	// random time between t and t+dt
				Imini[i]+=MINI_INPUT(t-ts)/synaptic_minicurrent;
		}
	}

	for(i=0;i<N;i++){
		Iexci[i]*=excitatory_current;
		Iinhi[i]*=inhibitory_current;
		Imini[i]*=synaptic_minicurrent;
		if(inhibitory[i]==-1)
		{
			outputs[i]*=inhibitory_current;
		}
		else
		{
			outputs[i]*=excitatory_current;
		}
	}
	return ;
}

int shot_noise(){
	return (RAND<(freqnoise+lambda)*dt ? 1 : 0);
}
