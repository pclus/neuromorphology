#ifndef VISUAL_H
#define VISUAL_H
        #include "visualization.h"
#endif

int vc;

FILE *visual_initialize(){
  FILE *pip=popen("gnuplot","w");

  fprintf(pip,"set terminal qt noraise\n");
  fprintf(pip,"set title 'Stuart-Landau'\n");
  fprintf(pip,"set xlabel 'Voltage Potential'\n");
  fprintf(pip,"set ylabel 'Recovery '\n");
  fprintf(pip,"set xrange[-80:35]\n");
  fprintf(pip,"set yrange[-15:10]\n");
  fprintf(pip,"unset key\n");
  fprintf(pip,"set grid\n");
  fprintf(pip,"set param\n");
  fflush(pip);

  return pip;
}


int visual_finish(FILE *pip){
  	fprintf(pip,"exit\n");
  	fflush(pip);
	fclose(pip);
	return 1;
}

int visual_plot(FILE *pip){
	int i;
  	fprintf(pip,"set title 'Time t=%g'\n",t);
	fprintf(pip,"plot '-' pt 7\n");
	for(i=0;i<N;i++)
		fprintf(pip,"%.16g %.16g\n",x[i],y[i]);
	fprintf(pip,"e\n");
	fflush(pip);
	return 1;
}


FILE *visual_initialize_network(){
  	FILE *pip=popen("gnuplot","w");
  	fprintf(pip,"set terminal qt noraise; set size square\n");
  	fprintf(pip,"unset key\n");
  	fprintf(pip,"set for[k=1:10] lt k ps 2.5\n");
	fprintf(pip,"set palette defined ( -1 '#0000ff',  0 '#000000', 1 '#ff0000')\n");
	fprintf(pip,"set tics tc 'white' front; set border lc 'white'\n");
	fprintf(pip,"@BLACK\n");
	
	fflush(pip);
	return pip;
}


FILE *visual_initialize_video(){
        FILE *pip=popen("gnuplot","w");
        //fprintf(pip,"set terminal qt noraise size 1800,1800 position 1800,0\n");
        fprintf(pip,"set terminal svg size 1200,900 background 'black'\n");
        fprintf(pip,"unset key\n");
        fprintf(pip,"set for[k=1:10] lt k ps 2.5\n");
        fprintf(pip,"set palette defined ( -1 '#0000ff',  0 '#000000', 1 '#ff0000')\n");
        fprintf(pip,"set tics tc 'white' front; set border lc 'white'\n");
        //fprintf(pip,"set multiplot layout 2,1\n");
        //fprintf(pip,"@BLACK\n");
        fflush(pip);
        vc=0;
        return pip;
}

int visual_video(FILE *pip){
        int i;
        double x;
	
	//fprintf(pip,"set size 0.8,0.8\n");
	
        fprintf(pip,"set output 'Frames%d/frame%d.svg'\n",vflag,vc++); fflush(pip);
        fprintf(pip,"set multiplot\n");

  	fprintf(pip,"set size square 0.7,0.7\n");
	fprintf(pip,"set origin 0.15,0.3\n");
	//fprintf(pip,"clear; set multiplot previous\n");
	//fprintf(pip,"clear; \n");
        fprintf(pip,"set xrange[0:1500]\n");
        fprintf(pip,"set yrange[0:1500]\n");
        fprintf(pip,"set cbrange[-1:1]\n");
        fprintf(pip,"set title 'Time t=%1.0f' tc 'white'\n",t);
	//fprintf(pip,"set object 2084 rectangle from screen 0,0.2 to screen 1,1 back fc 'black' fs noborder\n");
	fprintf(pip,"unset xlabel; unset ylabel;\n");
	fprintf(pip,"set xtics 200\n");
	fprintf(pip,"set ytics 200\n");
        fprintf(pip,"plot '-'  volatile pt 7 ps 1 lc pale, '-' volatile pt 7 ps 1 lc palette\n");
        for(i=0;i<N;i++)
        {
                x=outputs[i]*(inhibitory[i]==-1 ? 1.0/bottom : 1.0/top);
        	fprintf(pip,"%d %d %lf\n",coordx[i],coordy[i],x);
        }
        fprintf(pip,"e\n");
        fprintf(pip,"50 50 0.0\n");
        for(i=0;i<N;i++)
        {
                x=outputs[i]*(inhibitory[i]==-1 ? 1.0/bottom : 1.0/top);
                if(fabs(x)>1e-2)
                        fprintf(pip,"%d %d %.16g\n",coordx[i],coordy[i],x);

        }
        fprintf(pip,"e\n\n");
        //fflush(pip);

       	//fprintf(pip,"unset object 2084\n");


	fprintf(pip,"unset title; set xlabel 'Time [ms]' tc 'white'; set ylabel 'Activity [spikes/ms]' tc 'white' \n");
	fprintf(pip,"set xtics 100\n");
	fprintf(pip,"set ytics 200\n");
	fprintf(pip,"set size nosquare 1,0.3\n");
 	fprintf(pip,"set origin 0,0\n");
	//fprintf(pip,"clear; set multiplot next\n");
        fprintf(pip,"set xrange[%lf:%lf]\n",t-500,t);
        fprintf(pip,"set yrange[0:600]\n");
        fprintf(pip,"plot 'outputs/lfp_temp%d.dat' w l lc 'red', '' u 1:3 w l lc 'blue'\n",vflag );
        fprintf(pip,"unset multiplot\n");
        fflush(pip);

	usleep(200000);


        return 1;
}

int visual_network(FILE *pip){
        int i; 
        double x;

        fprintf(pip,"set xrange[0:1500]\n");
        fprintf(pip,"set yrange[0:1500]\n");
        fprintf(pip,"set cbrange[-1:1]\n");
        fprintf(pip,"set title 'Time t=%1.0f' tc 'white'\n",t);
        fprintf(pip,"plot '-'  volatile pt 7 lc pale, '-' volatile pt 7 lc palette\n");
        for(i=0;i<N;i++)
        {       
                x=outputs[i]*(inhibitory[i]==-1 ? 1.0/bottom : 1.0/top);
                fprintf(pip,"%d %d %lf\n",coordx[i],coordy[i],x);
        }
        fprintf(pip,"e\n");
        fprintf(pip,"50 50 0.0\n");
        for(i=0;i<N;i++)
        {       
                x=outputs[i]*(inhibitory[i]==-1 ? 1.0/bottom : 1.0/top);
                if(fabs(x)>1e-4)
                        fprintf(pip,"%d %d %.16g\n",coordx[i],coordy[i],x);
        
        }
        fprintf(pip,"e\n\n");
        
        fflush(pip);

        return 1;
}


FILE *visual_initialize_ts(){
  	FILE *pip=popen("gnuplot","w");
  	//fprintf(pip,"set terminal qt noraise\n");
  	fprintf(pip,"set terminal qt noraise size 1800,600 position 1800,1400\n");
  	fprintf(pip,"unset key\n");
	fprintf(pip,"@BLACK\n");
	fprintf(pip,"set xlabel 'Time t [ms]' tc 'white'\n");
	fprintf(pip,"set ylabel 'Activity [spikes/ms]' tc 'white'\n");
	fprintf(pip,"set xtics 100\n");
	
	fflush(pip);
	return pip;
}


int visual_ts(FILE *pip)
{
	
	fprintf(pip,"set xrange[%lf:%lf]\n",t-500,t);
	fprintf(pip,"plot 'outputs/lfp_temp.dat' w l lc 'red', '' u 1:3 w l lc 'blue'\n" );
	fflush(pip);

	return 1;
}
