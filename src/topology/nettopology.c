#include "nettopology.h"

int nx,ny,N;
Neuron *neu;	//array of neurons forming the graph

// main
int main(int argc, char **argv){
	int i;

	if(initialize(argc,argv)==-1) return 1;
	create_neurons();
	grow_axons();
	outputs();
	finish();

	return 1;
}

void outputs(){

	FILE *fout;
	fprintf(stderr,"Printing outputs...\n");
	int i,j;
	fout=fopen("outputs/stats.dat","w");
	for(i=0;i<N;i++){
		fprintf(fout,"%d %d %d\n",i,neu[i].dendritic_radius,neu[i].k_out);
	}
	fclose(fout);
	
	/* Network data as a list of edges with weights */
	fout=fopen("outputs/network.dat","w");
	fprintf(fout,"%d\n",N);
	for(i=0;i<N;i++)
	for(j=0;j<neu[i].k_out;j++)
		fprintf(fout,"%d %d %d\n",i,neu[i].neighbours_out[j],neu[i].weights_out[j]);
	fclose(fout);
	
	/* Coordinates x and y of the center of each node and dendritic tree radius*/
	fout=fopen("outputs/coordinates.dat","w");
	for(i=0;i<N;i++)
		fprintf(fout,"%d %d %d\n",neu[i].coord_x, neu[i].coord_y, neu[i].dendritic_radius);
	fclose(fout);

	/* Coordinates x,y,dx,dy of the edges, to plot using gnuplot*/
	fout=fopen("outputs/links.dat","w");
	int x0,y0,xf,yf,k,w;
	for(i=0;i<N;i++)
	for(j=0;j<neu[i].k_out;j++){
		x0=neu[i].coord_x;
		y0=neu[i].coord_y;
		k=neu[i].neighbours_out[j];
		xf=neu[k].coord_x;
		yf=neu[k].coord_y;
		w=neu[i].weights_out[j];
		fprintf(fout,"%d %d %d %d %d\n",x0,y0,xf-x0,yf-y0,w);
	}
	fclose(fout);

	fprintf(stderr,"done!\n");
	return ;
}

int initialize(int argc, char **argv){
	fprintf(stderr,"Initializing...\n");
	int i;

	if(argc!=7){
		fprintf(stderr,"USAGE: ./nettopology <soma_radius> <mean_dendritic_tree_radius> <std_dev_dendritic_tree_radius> <mean_axon_length> <std_dev_axon_growth_variation> <scale>\n");
		return -1;
	}
	
	//-------- Random number generator -----------//
        gsl_rng_env_setup();

        T = gsl_rng_default;
        rrr = gsl_rng_alloc(T);

        gsl_rng_set(rrr,time(0));
	//--------------------------------------------//
	
	//------- CONTROL ------------
	max_list=500;
	//----------------------------

	// -------SYSTEM SIZE---------
	N=3037;
	nx=1500;
	ny=1500;
	//----------------------------

	neu=(Neuron *) malloc(sizeof(Neuron)*N);
	for(i=0;i<N;i++){
		neu[i].neighbours_out=(int *) malloc(sizeof(int)*N);
		neu[i].weights_out=(int *) malloc(sizeof(int)*N);
	}
	
	lattice=(Square *) malloc(sizeof(Square)*nx*ny);
	for(i=0;i<nx*ny;i++){
		lattice[i].n=0;
		lattice[i].list=(int *) malloc(sizeof(int)*max_list);
	}

	//------- PARAMETERS--------- 
	soma_radius=atoi(argv[1]) ;		//soma radius, fixed to 16  
	mean_dr=atoi(argv[2]); 			//average radius of dendritic tree; 
	sigma_dr=atoi(argv[3]); 		//std dev of dendritic tree radius, fixed to 40
	sigma_a=sqrt(2/M_PI)*atoi(argv[4]);	//scaling of Rayleigh distribution of axon length=sqrt(2/pi)*mean, fixed to mean=500
	sigma_angle=atoi(argv[5]);		//standar deviation of the angle change on the axon growth, fixed to 6 (degrees).
	scale=atof(argv[6]);			//scale of the spine density (vals_ys)* scale of the dendrite density (vals_yd). Both scales are  w.r.t. WT. 
	//---------------------------
	fprintf(stderr,"done!\n");
	return 1;
}

void finish(){
	fprintf(stderr,"Freeing memory...\n");
	int i;
	for(i=0;i<N;i++){
		free(neu[i].neighbours_out);
		free(neu[i].weights_out);
	}
	free(neu);
	for(i=0;i<nx*ny;i++)
		free(lattice[i].list);
	free(lattice);
	
	fprintf(stderr,"done!\n");
	return ;
}


// Creates and initializes each neuron of the network
void create_neurons(){
	fprintf(stderr,"Setting neurons...\n");
	int i,j;
	double xx,yy,x0,y0,dd;
	for(i=0;i<N;i++){
		neu[i].type=excitatory;
		do{					// non-overlap somas
			neu[i].coord_x=RANDI(nx);				// position x
			neu[i].coord_y=RANDI(ny);				// position y
			x0=neu[i].coord_x;
			y0=neu[i].coord_y;
			for(j=0;j<i;j++){
				xx=neu[j].coord_x;
				yy=neu[j].coord_y;
				dd=sqrt((xx-x0)*(xx-x0)+(yy-y0)*(yy-y0));
				if(dd < soma_radius ) break;
			}
		}while(j<i);
		do{
			neu[i].dendritic_radius=GAUSSIAN(mean_dr,sigma_dr);	// dendritic radius
		}while(neu[i].dendritic_radius<0);

		neu[i].axon_length=RAYLEIGH(sigma_a); 			// axon length
		neu[i].k_out=0;						// out degree
		paint_lattice(i);					// remember the area occupied by the dendritic tree
	}
	fprintf(stderr,"done!\n");
	return ;
}

// Updates the list that links  each pixel of the lattice with the neurons that have dendrites there
void paint_lattice(int id){
	int r=neu[id].dendritic_radius;
	int x0=neu[id].coord_x;
	int y0=neu[id].coord_y;
	int i,j,ii,jj;
	for(i=x0-r;i<=x0+r;i++){
		ii=i;
		ii=(ii>=nx ? ii-nx : ii); ii=(ii<0 ? ii+nx : ii); 
	for(j=y0-r;j<=y0+r;j++){
		jj=j;
		jj=(jj>=ny ? jj-ny : jj); jj=(jj<0 ? jj+ny : jj); 
		if( sqrt((i-x0)*(i-x0)+(j-y0)*(j-y0)) <= r ){
			LATTICE(ii,jj).list[LATTICE(ii,jj).n]=id;
			LATTICE(ii,jj).n++;
			if(LATTICE(ii,jj).n>max_list){
				fprintf(stderr,"WARNING: overflow at x=%d y=%d with %d \n",i,j,LATTICE(ii,jj).n);
				LATTICE(ii,jj).list=(int *)realloc(LATTICE(ii,jj).list, sizeof(int)*5*max_list);
			}
		}
	}
	}
	return ;
}

// Establishes the links of the network
void grow_axons(){
	fprintf(stderr,"Growing axons and stablishing synapses...\n");
	int i,j,k,kk,index;
	int length;
	double x,y, aux_x,aux_y;
	double rad, angle, dx,dy,rr;
	int x_int,y_int;
	FILE *fout=fopen("outputs/axons.dat","w");	// output the axons for visualization

	for(i=0;i<N;i++){ 			// for each neuron
		length=neu[i].axon_length; 	// take the length of the axon
		x=neu[i].coord_x;
		y=neu[i].coord_y;

		angle=2*M_PI*RAND;
		dx=cos(angle);
		dy=sin(angle);
		for(j=0;j<length;j++){ 		// grow the axon as follows:
			if(j%10==0 && j>1){  		// each 10 steps, compute new angle
				angle=GAUSSIAN(angle*180/M_PI,sigma_angle); // var_angle=\sigma_a=6 degrees
				angle=angle*M_PI/180.0;
				dx=cos(angle);
				dy=sin(angle);
			}
			x=x+dx;	x=(x>=nx ? x-nx : x);	x=(x<0 ? x+nx : x); // grow accordingly...
			y=y+dy; y=(y>=ny ? y-ny : y);	y=(y<0 ? y+ny : y); // ...checking periodic cond.

			x_int=SQUARE(x); x_int=(x_int >= nx ? x_int-nx : x); x_int=(x_int < 0 ? x_int+nx : x_int);
			y_int=SQUARE(y); y_int=(y_int >= ny ? y_int-ny : y); y_int=(y_int < 0 ? y_int+ny : y_int);
			kk=LATTICE(x_int,y_int).n; 	// # of dendritic trees from different neurons are there
			for(k=0;k<kk;k++){	// roll a dice for each, and link if corresponds
				index=LATTICE(x_int,y_int).list[k];
				aux_x=x-neu[index].coord_x;
				aux_y=y-neu[index].coord_y;
				//rr=mean_dr/(1.*neu[index].dendritic_radius); // scale on x 
				rr=218.0/(1.*neu[index].dendritic_radius); // scale on x 
				rad=sqrt( aux_x*aux_x + aux_y*aux_y );
				if(synapse(rad,rr)) create_link(i,index); 
			}
			fprintf(fout,"%lf %lf\n",x,y);
		}
		fprintf(fout,"\n\n");
	}
	fclose(fout);
	fprintf(stderr,"done!");
	return ;
}

// Determines whether there should be a synapse or not
bool synapse(double radius, double rr){
	if(radius < soma_radius) return false;
	double p=RAND;
	double a=alpha(radius,rr);
	return (p <= a ?  true : false );
}

// Probability of connection as a function of the distance to the soma (results from fitting the data)
double alpha(double r, double bb){ 
	double x=r*bb;
	
        double b0= -3.3939e-09;
        double b1= 0.45811;
	double a=b1*x*exp(b0*x*x*x*x);
        
        double a0= 0.0431565;
        double a1= -0.258122*x;
        double a2= 0.0243603*x*x;
        double a3= -0.000420021*x*x*x;
        double a4= 3.12164e-06*x*x*x*x;
        double a5= -1.0826e-08*x*x*x*x*x;
        double a6= 1.43744e-11*x*x*x*x*x*x;
	
	double b=a0+a1+a2+a3+a4+a5+a6;

	return (a*b < 0 ? 0 : scale*a*b/(2*M_PI*10*r));
}

// Updates the list of links of neuron i
void create_link(int i,int j){ // neuron i is conecting to j, so i sends the input to j
	if(i==j) return; //avoid autapses
	int kk=neu[i].k_out,k=0;
	for(k=0;k<kk;k++){
		if(neu[i].neighbours_out[k]==j){	// if there is a connection from i to j already;
			neu[i].weights_out[k]++;	// increase the weight
			break;
		}
	}
	if(k==kk){				// if no connection exists yet;
		neu[i].k_out++;			// increase node degree,
		neu[i].neighbours_out[kk]=j;	// set the new link,
		neu[i].weights_out[kk]=1;	// and the new wheight
	}

	return ;
}
