#include "netdynamics.h"

int main(int argc, char **argv ){
	int pfd[2];
	if(pipe(pfd) == -1){//create pipe
	        fprintf(stderr,"Pipe call error\n");
	        return -1;
	}
	pid_t flag=fork(); //create child process: whatever is father, 0 is child, -1 is error!
	if(flag==0)
		child_thread(pfd);
	else{
		parent_thread(argc, argv, pfd);
	        kill(flag,SIGTERM);
        	wait(NULL);  //wait for child to finish
	}
	return -1;
}

int child_thread(int *pfd){
        char c='c';
        read(pfd[0], &c, 1);//waits for parent to say something
        close(pfd[0]);/* Read end is unused */
        while(c!='q'){ // while q is not inserted
                c=getchar(); //keep reading
                write(pfd[1], &c, 1); //and sending to the parent
        }
        //close(pfd[0]);
        close(pfd[1]);
        wait(NULL);
        return 1;
}

int parent_thread(int argc, char **argv, int *pfd){
	if(system_initialize(argc, argv)==-1) return -1;
	int i;
	//vflag=atoi(argv[8]);
	//----------------- SET VISUALIZATION -----------
	//FILE *pip=visual_initialize_video();
	FILE *pip=visual_initialize_network();
	FILE *pip2=visual_initialize_ts();
	//FILE *pip=visual_initialize();
	int count=0; 
	visual=false;
	current=false;
	//------------------------------------------------

	//----------------- SET USER INTERFACE -----------
	char c='c';
      	int flags = fcntl(pfd[0], F_GETFL, 0); //just gets the flags that are already in pdf
        fcntl(pfd[0], F_SETFL, flags | O_NONBLOCK);//set the nonblock flag
	//------------------------------------------------

	//------------------ SET OUTPUT FILES ------------
	FILE *fout,*fout_lfp;
	char *namm=malloc(sizeof(char)*200);
	sprintf(namm,"outputs/spykes_%s",argv[2]);
	fout=fopen(namm,"w");
	sprintf(namm,"outputs/lfp_%s",argv[2]);
	fout_lfp=fopen(namm,"w");
	free(namm);
	//-----------------------------------------------

	fprintf(stderr,"Press 'q' to quit, 's' to skip, 'p' to pause, 'l' to perturb\n");
        write(pfd[1], &c, 1); 	//tell child to start
	close(pfd[1]);		//write is not used after this


//	for(i=0;i<N;i++){
//		if(new_spyke[i]==1) fprintf(fout,"%lf %d %d\n",t,i,inhibitory[i]);
//	}
	int se=0,si=0;
	while(c!='q' && t<.5e4 ){
	//while(c!='q' ){
		if(count%10000==0) fprintf(stderr,"Integrating t=%.0f\r",t);

		euler();
		count++;
                read(pfd[0], &c, 1);
		c=visualize(c,count,pfd, pip,pip2);
		
		//fprintf(stdout,"%lf %lf\n",t,x[50]);
		//fprintf(stdout,"%lf %lf %lf\n",t,outputs[50]-Routputs[50],x[50]);
	
		//for(i=0;i<N;i++)
		//{
		//	if(new_spyke[i]==1)
		//	{
		//		if(inhibitory[i]==1) se++; else si++;
		//	}
		//}
		//if(count%50==0)  // this is because 50=1/dt 
		if(count%200==0)
		{
			if(t>500)
			{
				fprintf(fout_lfp,"%lf %lf\n",t,activity/(1.0*N));
				fprintf(fout,"%lf %lf\n",t,acti*1.0);
			}
			acti=0;
		}
		if(count%1000==0)
		{
			fflush(fout);
			fflush(fout_lfp);
		}
		//if(t>500){
		//	for(i=0;i<N;i++){
		//		if(new_spyke[i]==1) fprintf(fout,"%lf %d %d\n",spyke_times[i],label[i],inhibitory[i]);
		//	}
		//	if(count%1000==0){fflush(fout);fflush(fout_lfp);}
		//	if(count%50==0){fprintf(fout_lfp,"%lf %lf\n",t,activity);}
		//}


	}
	fprintf(stderr,"\n");
	visual_finish(pip);
	visual_finish(pip2);
	fclose(fout);
	fclose(fout_lfp);
	system_finish();
	close(pfd[0]);
	return 1;
}


char visualize(char c, int count, int *pfd, FILE *pip, FILE *pip2){
	int step_sample=20;
	int i;

        if(c=='v'){
                visual=(visual ? false : true);
                fprintf(stderr,"\nVisualization %s\n", (visual ? "on" : "off"));
                 c='c';
        }
        //if(c=='l'){
	//	if(!current){
	//		current=true;
	//		fprintf(stderr,"\nEntering current\n");
	//		for(i=0;i<N;i++)
	//			IC[i]=(RAND<1.5 ? 5+0*RANDIFF : 0);
	//	}
	//	else{	
	//		current=false;
	//		for(i=0;i<N;i++)
	//			IC[i]=0;
	//		fprintf(stderr,"\nNo current\n");
	//	}
	//	c='c';
	//}

        if(visual==true){
                if(count%step_sample==0){
                        //visual_plot(pip);
			visual_network(pip);
			//visual_video(pip);
			//visual_ts(pip2);
                }
                if(c=='p'){
                        fprintf(stderr,"Press 'c' to continue, 't' to print distribution\n");
                        while(c!='c' && c!='q'){ 
                                read(pfd[0], &c, 1);
                                if(c=='t'){
                                        fprintf(stderr,"Nothing is being done!\n");
                                        c='p';
                                }
                        }
                }
        }
	return c;
}
