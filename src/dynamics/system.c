#ifndef SYSTEMSL_H
#define SYSTEMSL_H
        #include "system.h"
#endif

#define INHI2INHI (in2in) 

#define ALPHAexci (6e-3) 
#define ALPHAinhi (720e-3) 
#define ALPHAexte (8e-3) 

#define TAUexci 2.0
#define TAUinhi 4.0
#define TAUexte 2.0

#define STRENGTH (1.0)

#define EXCI_INPUT(t) (ALPHAexci*exp(-(t)/TAUexci)/TAUexci)
#define INHI_INPUT(t) (ALPHAinhi*exp(-(t)/TAUinhi)/TAUinhi)
#define EXTE_INPUT(t) (ALPHAexte*exp(-(t)/TAUexte)/TAUexte)

#define EXCI_INIT_INPUT(t) (exp(-(t)/TAUexci))
#define INHI_INIT_INPUT(t) (exp(-(t)/TAUinhi))
#define EXTE_INIT_INPUT(t) (exp(-(t)/TAUexte))

#define EE (0)
#define EI (-70.0)

#define DELAY(j,k) (delay[(k)*N+(j)])

double strength;
double *delay;
double in2in, alphaInhi;
int id_delay, max_id_delay, Nex;
double renorm;

int system_initialize(int argc, char **argv){
	if(argc!=5){
		fprintf(stderr,"USAGE: ./evolve <network> <outputs> <lambda> <overinhi>\n");
		return -1;
	}
	
	t=0;
	dt=0.005;
	if(initialize_network(argv[1])==-1){
		fprintf(stderr,"Errors in system.c, sorry. See ya!\n");
		return -1;
	}
	
	in2in=atof(argv[4]);
	lambda=atof(argv[3]);
	
	// rng setup
	gsl_rng_env_setup();
	T = gsl_rng_default;
	rrr = gsl_rng_alloc(T);
	srand(time(0)*getpid());
	gsl_rng_set(rrr,time(0)*getpid());


	fprintf(stderr,"Initializing with N=%d neurons\n",N);
	
	// memory allocations
	x=(double *)malloc(sizeof(double)*N);				
	y=(double *)malloc(sizeof(double)*N);				
	x_0=(double *)malloc(sizeof(double)*N);				
	y_0=(double *)malloc(sizeof(double)*N);				
	aux_x=(double *)malloc(sizeof(double)*N);			
	aux_y=(double *)malloc(sizeof(double)*N);			
	spyke_times=(double *)malloc(sizeof(double)*N);			
	label=(int *)malloc(sizeof(int)*N);				
	Iexci=(double *)malloc(sizeof(double)*N);		
	Iinhi=(double *)malloc(sizeof(double)*N);	
	Iexte=(double *)malloc(sizeof(double)*N);
	outputs=(double *)malloc(sizeof(double)*N);
	

	int nn=N/dt, i; 
	max_id_delay=(int) 1./dt;
	id_delay=0;
	fprintf(stderr,"\n\n %d %d %d\n\n",nn, max_id_delay, id_delay);
	delay=(double *)malloc(sizeof(double)*nn);
	for(i=0;i<nn;i++) delay[i]=0;	

	acti=0;

	excitatory_current=EXCI_INIT_INPUT(dt);
	inhibitory_current=INHI_INIT_INPUT(dt);
	synaptic_extecurrent=EXTE_INIT_INPUT(dt);
	
	top=ALPHAexci/TAUexci;
	bottom=ALPHAinhi/TAUinhi;

	fprintf(stderr,"Currents:\n\texci=%lf\n\tinhi=%lf\n\texte=%lf\n",excitatory_current,inhibitory_current,synaptic_extecurrent);
        fprintf(stderr,"Strength:\n\texci=%lf\n\tinhi=%lf\n\texte=%lf\n",ALPHAexci,ALPHAinhi,ALPHAexte);


	initial_conditions();
	relabel();
	fprintf(stderr,"Initialization done\n");

	return 1;
}

int initial_conditions(){
	int i;
	Nex=0;
	for(i=0;i<N;i++){
		inhibitory[i]=( RAND < 0.2 ? -1 : 1);
		Nex+= (inhibitory[i]==1 ? 1 : 0);
		Iexci[i]=Iinhi[i]=Iexte[i]=0;
	}

	for(i=0;i<N;i++){
		if(RAND<0.7) 
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
        degree=(int *) malloc(sizeof(int)*N);				
        cumulative_degree=(int *) malloc(sizeof(int)*(N+1));		
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

        weights=(double *) malloc(sizeof(double)*cumulative);	
        neighbour=(int *) malloc(sizeof(int)*cumulative);	
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

        inhibitory=(int *) malloc(sizeof(int)*N);	
        new_spyke=(int *) malloc(sizeof(int)*N);	

	fprintf(stderr,"Done\n");
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
	//free(lfp);		
	free(label);
	free(Iexci); free(Iinhi); free(Iexte);
	free(outputs);
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
	double I;
	synapses(t);
        for(i=0;i<N;i++){
                a=(inhibitory[i]==1 ? 0.02 : 0.1);
		I = (Iexci[i]+Iexte[i])*(EE-x[i])+Iinhi[i]*(EI-x[i]);
                k=f(x[i],y[i],I);  
                l=g(x[i],y[i],a,b);
		x_0[i]=x[i];
                x[i]=x[i]+dt*k; 
                y[i]=y[i]+dt*l;
                new_spyke[i]=spyke_reseting(i);
        }
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
		double d=(inhi ? 2.0 : 8.0 );		// reset of y depends on neuron type

		ts=t+dt*(30-x_0[i])/(x[i]-x_0[i]); 	// linear interpolation of spike time
    		spyke_times[i]=ts;			
    		DELAY(i,id_delay)=ts;			
		//-------just for visualization ---------------
                if(inhi)
                          current=-INHI_INPUT(t-ts)/inhibitory_current;
                else
                          current=EXCI_INPUT(t-ts)/excitatory_current;
                outputs[i]+=current;
		//----------------------------------------------

    		x[i]=-65;
    		y[i]=y[i]+d; 		
    		r=1;

		acti++;	// firing rate
    	}
	id_delay=(id_delay+1 < max_id_delay ? id_delay+1 : 0);

    	return r;
}


void synapses(double t){
	int i,j,jj;
	bool inhi;
	double w,current,ts;

	for(i=0;i<N;i++){
		inhi=(inhibitory[i]==-1);
		ts=DELAY(i, id_delay);
		DELAY(i, id_delay)=-1;	// empty matrix for nexts steps
		if(ts > 0){
			if(inhi)
				current=INHI_INPUT(t-ts-1)/inhibitory_current;
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
		if(shot_noise()){
			ts=t+RAND*dt;	// random time between t and t+dt
			Iexte[i]+=EXTE_INPUT(t-ts)/synaptic_extecurrent;
		}	
	}

	for(i=0;i<N;i++){
		Iexci[i]*=excitatory_current;
		Iinhi[i]*=inhibitory_current;
		Iexte[i]*=synaptic_extecurrent;
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
	return (RAND< lambda*dt ? 1 : 0);
}
