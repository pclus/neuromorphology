#include "netdynamics.h"

int main(int argc, char **argv ){
	if(system_initialize(argc, argv)==-1) return -1;
	int i;

	FILE *fout,*fout_lfp,*fout_rp;
	char *namm=malloc(sizeof(char)*200);
	sprintf(namm,"outputs/spykes_%s.dat",argv[2]);
	fout=fopen(namm,"w");
	sprintf(namm,"outputs/lfp_%s.dat",argv[2]);
	fout_lfp=fopen(namm,"w");
	sprintf(namm,"outputs/rp_%s.dat",argv[2]);
	fout_rp=fopen(namm,"w");
	free(namm);

	long unsigned count=0;
	while( t<.5e4 ){
		if(count%10000==0) fprintf(stderr,"Integrating t=%.0f\r",t);

		euler();
		count++;
		
		if(t>500)
		{
			if(count%200==0)
			{
				fprintf(fout_lfp,"%lf %lf\n",t,activity/(1.0*N));
				fprintf(fout,"%lf %lf\n",t,acti*1.0);
			}
			acti=0;
                	for(i=0;i<N;i++)
			{
                      		if(new_spyke[i]==1)
					fprintf(fout_rp,"%lf %d %d\n",spyke_times[i],label[i],inhibitory[i]);
                      	}
		}
		if(count%1000==0)
		{
			fflush(fout);
			fflush(fout_lfp);
			fflush(fout_rp);
		}


	}
	fprintf(stderr,"\n");
	fclose(fout);
	fclose(fout_lfp);
	fclose(fout_rp);
	system_finish();
	return 1;
}
