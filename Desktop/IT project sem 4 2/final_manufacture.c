/* MANUFACTURING PLANT : Manufacture of Sulphuric Acid 
                        - 5 stage process 
		        - Concepts used : Threads, Mutex, Pipes and Signals  

         MANUFACTURERS : PRASHANT PANDEY (201001067)
      	               : SWENA GUPTA     (201001104)

     Method to compile : gcc -pthread final_manufacture.c

*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define total_threads 5

char *task_ongoing[total_threads];
char *pipe_content[total_threads];

void *control_machine (void *quantity);
void *func_for_thread0 (void *ptr);
void *func_for_other_threads (void *ptr);

// variables :

char write_msg[40];	// for writing from pipe 
char read_msg[40];	// for reading from pipe
int j;
int p[2];		// named-pipe for passing msg from previous machine to next :file descriptors

int COUNT;		// takes care that which machine number is currently operating : used in implementing mutex
int num_of_times=0;
int quantity;		

int signo;
static struct sigaction act;
void catchint(int);

// mutex and conditional variables declarations :

pthread_mutex_t count_lock;
pthread_cond_t count_cond;		// count_cond is conditional variable 

// structures :

struct machine_content
{
  int thread_id;
  char *pipe_msg;
  char *message;
  float temperature;
};

struct machine_content machine[total_threads];
pthread_t thread[total_threads];
pthread_t control_thread;

void main (int argc, char* argv[]) 	// used command-line arguments : to give the input as the 'quantity of acid' needed to be produced.
{

	time_t now;
	time (&now);

 	printf ("\n\n%s\n\n", ctime (&now));
 	//printing of DATE done

	//now signal handling starts
	act.sa_handler = catchint;
	sigfillset(&(act.sa_mask));

	sigaction(SIGINT , &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGALRM, &act, NULL);
  	sigaction(SIGWINCH, &act, NULL);
  	sigaction(SIGTSTP, &act, NULL);	
	sigaction(SIGUSR1 , &act, NULL);
	

	printf("\t\t\t\t\t\t WORKING MACHINE SPECIFICATIONS\n");
	printf("\t\t\t\t\t\t________________________________\n\n");
	printf("\t\t\t\t\t       (all temperatures in degree celsius)\n\n");
	printf("Machine No.\tInitial Temperature\t1lt incr temp by\tMax temp under which Machine works\tTemp Decrs'd in Cooling Machine\n\n");
	printf("    1\t\t\t450\t\t\t20\t\t\t\t500\t\t\t\t\t20\n");
	printf("    2\t\t\t30 \t\t\t10\t\t\t\t55 \t\t\t\t\t10\n");
	printf("    3\t\t\t500\t\t\t30\t\t\t\t575\t\t\t\t\t30\n");
	printf("    4\t\t\t25 \t\t\t5 \t\t\t\t39 \t\t\t\t\t5 \n");
	printf("    5\t\t\t25 \t\t\t5 \t\t\t\t39 \t\t\t\t\t5 \n");


	
	printf(" Note: Assumptions made : \n");
	printf(" ________________________\n\n");
	printf("1. The Initial & Favourable specifications of machines are pre-defined AS ABOVE . \n\n");
	printf("2. According to the code made by us , the  specifications  has  been  set, \n");
	printf("   so that if the required amount is > 3 litres , all the 5 stage proccess \n");
	printf("   machines gets overheated & production of 4th lts onwards acid needs  to \n");
	printf("   cool each machine.THIS HAS BEEN SHOWN APPROPRIATELY WHILE PRODUCTION OF \n");
	printf("   4th LITRE (onwards) ACID.\n\n");
	printf("3. This  has  been  done deliberately to introduce and show the concept of  \n");
	printf("   temperature in our machine automated manufacturing-plant system.  \n\n");
	printf("4. Following SIGNALS have been handled in our program : \n\n");
	printf("   a) SIGINT  : signal called for handling (Ctrl+C). \n");
	printf("   b) SIGQUIT : signal called for handling (Ctrl+\\). \n");
	printf("   c) SIGALRM : signal called for production of next litre of acid. \n");
	printf("   d) SIGWINCH: signal called when output terminal window size is changed. \n");
	printf("   e) SIGTSTP : signal called for handling (Ctrl+Z). \n");
	printf("   f) SIGUSR1 : signal called if the user enters invalid input. \n");
	int j;
	
	
	for(j=0;j<strlen(argv[1]);j++)
	
		if( (argv[1][j]>=32 && argv[1][j]<=47) || (argv[1][j]>=58 && argv[1][j]<=126))	
		{
			raise(SIGUSR1);
		}
	

	// Here, we've checked if any of the characters entered via cmd-line-arg is non-digit then, we send signal SIGUSR1

	
	quantity=atoi(argv[1]);
	
	
	if(pthread_create(&control_thread, NULL, control_machine, (void *) quantity)!=0)
	{
		printf("Failed to create a thread\n"); 
       		exit(1);
	}

	pthread_join(control_thread, NULL); 	// main CONTROL-MACHINE that controls all other 5 processing machines

}

void catchint(int signo)	// handler for sigaction
{

	if(signo==SIGINT)
	{
		printf("\n\n\'Ctrl+C\' pressed...Interrupt generated and ignored!!!\n\n");
	}

	if(signo==SIGQUIT)
	{
		printf("\n\nEXIT COMMAND RECEIVED : exiting.......\n\n");
		exit(0);
	}

	if(signo==SIGALRM)
	{
		printf("ALARM !!! Proceeding for generation of next litre of Acid . . .\n\n\n");
	}

	if(signo==SIGWINCH)
	{
		printf("\nALERT: WINDOW SIZE CHANGED...PROGRAM CONTINUING ANYWAYS  !!!\n\n\n");
	}

	if(signo==SIGTSTP)
	{
		printf("\n\n\'Ctrl+Z\' pressed...Interrupt generated and ignored!!!\n\n");
	}

	if(signo==SIGUSR1)
	{
		printf("\n\nWrong value entered!! kindly re-execute the program and enter a valid value on re-execution.\n\n");
		exit(0);
	}

		
}







void *control_machine (void *quantity)
{

	int qty=(int)quantity;

	printf("\n\t\tREQUIRED: %d LITRES OF SULPHURIC ACID TO BE PRODUCED\n\n ", qty);

	printf ("\t\t\tManufacture of Sulphuric Acid\n");  
	printf ("\t\t\t-------------------------------\n");
	printf ("\t\t\t-------------------------------\n\n");
	
	pthread_mutex_init (&count_lock, NULL);
	pthread_attr_t attr;
	pthread_attr_init (&attr);	


	pipe_content[0] = "Sulphur Dioxide (SO2) prepared !!!";
	pipe_content[1] = "Sulphur Dioxide purified !!!";
	pipe_content[2] = "Oxidation of Sulphur Dioxide DONE & SO3 produced !!!";
	pipe_content[3] = "SO3 absorbed in H2SO4 & Oleum produced !!!";
	pipe_content[4] = "";

	task_ongoing[0] = "Burning Sulphur ...";
	task_ongoing[1] = "Sulphur Dioxide purifying...";
	task_ongoing[2] = "Oxidation of Sulphur Dioxide being carried out ...";
	task_ongoing[3] = "Absorbing Sulphur Trioxide in chamber ...";
	task_ongoing[4] = "Diluting Oleum...to produce acid ... ";

	int i,t;		// local loop variables


	machine[0].thread_id = 0;
  	machine[0].pipe_msg = pipe_content[0];
  	machine[0].message = task_ongoing[0];


	for(i=1;i<=qty;i++)	// for production of every new litre of acid.
	{

		COUNT=2;	// needs to be re-initialized at start of every new loop
		pipe (p);


		if (pthread_create(&thread[0], NULL, func_for_thread0, (void *) &machine[0])!=0)
		{
			printf("Failed to create a thread\n"); 
        		exit(1);
		}


	  	/* Create independent threads each of which will execute function */

  
		for (t = 1; t < total_threads; t++)
    		{
      			machine[t].thread_id = t;
      			machine[t].pipe_msg = pipe_content[t];
      			machine[t].message = task_ongoing[t];

      			if(pthread_create (&thread[t], NULL, func_for_other_threads,(void *) &machine[t])!=0)
			{
				printf("Failed to create a thread\n"); 
        			exit(1);
			}	


    		}

	/* Wait till threads are complete before main continues. Unless we wait we run the risk of executing an exit which will terminate the 		 process and all threads before the threads have completed.*/

		pthread_join (thread[0], NULL);
		pthread_join (thread[1], NULL);
		pthread_join (thread[2], NULL);
		pthread_join (thread[3], NULL);
		pthread_join (thread[4], NULL);


		printf("\n\n\t\t\t|                         |\n");
		printf(    "\t\t\t|                         |\n");
		printf(    "\t\t\t|                         |\n");
		printf(    "\t\t\t| %d lt(s). acid produced  |\n",i);
		printf(    "\t\t\t|_________________________|\n\n\n");



		if(i!=qty)		// i.e not for the last time
		{
			alarm(1);	// after 1 seconds alarm tunes, that prints corresponding msg 
			sleep(1);
		}

	}	
	
  	sleep(2);			// after final production of required quantity

  	printf("\tTask Accomplished, %d Litres of SULPHURIC ACID PRODUCED !!! :)\n",qty);
  	printf("\t--------------------------------------------------------------\n");
  	printf("\t--------------------------------------------------------------\n\n");

  	exit (0);			// graceful exit 

}









void *func_for_thread0 (void *ptr)	// machine[0]'s corresponding function
{	

	int taskid;
	char *hello_msg;
	struct machine_content *my_data;
	char *pipe_mssg;
  

	my_data = (struct machine_content *) ptr;
	taskid = my_data->thread_id;
	pipe_mssg = my_data->pipe_msg;
	hello_msg = my_data->message;
  
	strcpy(write_msg,"");	

	
	if(num_of_times==0)		// i.e while 1st litre production, the initialization of machine's temperature takes place
	{
		my_data->temperature=450;
	}
	
	if(my_data->temperature>500)	// since prescribed condition on 1st machine is that it works only if its temp <=500
	{
		printf("\n Machine I over-heated...\nIt needs to be cooled...\n");
		sleep(3);
		printf("cooling....\n");
		printf(".\n");
		sleep(1);
		printf(".\n");
		sleep(1);
		printf("Machine cooled by 20 degree celsius\n");
		sleep(2);	
		printf("\nRestarting Machine I...\n\n");
		sleep(2);
		my_data->temperature = my_data->temperature-20;
	}

	if(my_data->temperature<=500)
	{
		printf ("Machine %d: %s\n", taskid+1, hello_msg);
		strcpy (write_msg, pipe_mssg);
		write (p[1], write_msg, 60);

	/*Above we are writing in pipe so that the data written in Machine(thread) 'n' could be extracted and taken as an input(read from pipe) in 		Machine  'n+1' . This is how the concept of pipes has been implemented*/
	
	}
	
	my_data->temperature = my_data->temperature+20;		// while exiting from machine[0] , increment its temperature 
	pthread_exit (NULL);					// exit from thread[0]
   
}







void *func_for_other_threads (void *ptr)			// machine[n]'s corresponding function, where n= 1 to 4
{

	int taskid;
	char *hello_msg;
	struct machine_content *my_data;
	char *pipe_mssg;

	my_data = (struct machine_content *) ptr;
	taskid = my_data->thread_id;
	pipe_mssg = my_data->pipe_msg;
	hello_msg = my_data->message;


	pthread_mutex_lock (&count_lock);		// mutex implementation

	while (taskid >= COUNT)				// i.e for 'n'th thread/machine, all threads/machines from (n+1) till last are in waiting 
	{

		pthread_cond_wait (&count_cond, &count_lock);

	} 

	
	strcpy(read_msg,"");
	read (p[0], read_msg, 60); 			// Here above, we've read the data from the previous Machine's(thread's) write (output)
	strcpy(write_msg,"");	
	strcpy (write_msg, pipe_mssg);
	
	sleep(2);
  	printf ("-->%s\n", read_msg);
  	printf ("----------------------------------------------------------\n");
  	printf ("----------------------------------------------------------\n\n");
  	sleep(1);

	switch(taskid)					
	{
		case 1:
			if(num_of_times==0)	// i.e while 1st litre production, the initialization of machine's temperature takes place
			{
				my_data->temperature=30;
			}
	
			if(my_data->temperature>55)
			{
				printf("\n Machine II over-heated...\nIt needs to be cooled...\n");
				sleep(3);
				printf("cooling....\n");
				printf(".\n");
				sleep(1);
				printf(".\n");
				sleep(1);
				printf("Machine cooled by 10 degree celsius\n");
				sleep(2);
				printf("\nRestarting Machine I...\n\n");
				sleep(2);
				my_data->temperature = my_data->temperature-10;
			}

			if(my_data->temperature<=55)
			{
    				printf ("Machine %d: %s\n", (taskid+1), hello_msg);
				write (p[1], write_msg, 60);
				COUNT = COUNT + 1;
				my_data->temperature = my_data->temperature+10;
			}
			break;
	
		case 2:
		{
			if(num_of_times==0)	// i.e while 1st litre production, the initialization of machine's temperature takes place
			{
				my_data->temperature=500;
			}
	
			if(my_data->temperature>575)
			{
				printf("\n Machine III over-heated...\nIt needs to be cooled...\n");
				sleep(3);
				printf("cooling....\n");
				printf(".\n");
				sleep(1);
				printf(".\n");
				sleep(1);
				printf("Machine cooled by 30 degree celsius\n");
				sleep(2);
				printf("\nRestarting Machine III...\n\n");
				sleep(2);
				my_data->temperature = my_data->temperature-30;
			}

			if(my_data->temperature<=575)
	
			{
    				printf ("Machine %d: %s\n", (taskid+1), hello_msg);
				write (p[1], write_msg, 60);
				COUNT = COUNT + 1;
				my_data->temperature = my_data->temperature+30;
			}
			break;
		}
	

		case 3:				
		{
	
			if(num_of_times==0)	// i.e while 1st litre production, the initialization of machine's temperature takes place
			{
				my_data->temperature=25;
			}

			if(my_data->temperature>39)
			{
				printf("\n Machine IV over-heated...\nIt needs to be cooled...\n");
				sleep(3);
				printf("cooling....\n");
				printf(".\n");
				sleep(1);
				printf(".\n");
				sleep(1);
				printf("Machine cooled by 5 degree celsius\n");
				sleep(2);
				printf("\nRestarting Machine IV...\n\n");
				sleep(2);
				my_data->temperature = my_data->temperature-5;
			}

			if(my_data->temperature<=39)
			{
			    	printf ("Machine %d: %s\n", (taskid+1), hello_msg);
				write (p[1], write_msg, 60);
				COUNT = COUNT + 1;
				my_data->temperature = my_data->temperature+5;
			}
	
			break;
	
		}

		case 4:
		{

			if(num_of_times==0)	// i.e while 1st litre production, the initialization of machine's temperature takes place
			{
				my_data->temperature=25;
			}

			if(my_data->temperature>39)
			{
				printf("\n Machine V over-heated...\nIt needs to be cooled...\n");
				sleep(3);
				printf("cooling....\n");
				printf(".\n");
				sleep(1);
				printf(".\n");
				sleep(1);
				printf("Machine cooled by 5 degree celsius\n");
				sleep(2);
				printf("\nRestarting Machine V...\n\n");
				sleep(2);
				my_data->temperature = my_data->temperature-5;
			}

			if(my_data->temperature<=39)
			{
    				printf ("Machine %d: %s\n", (taskid+1), hello_msg);
				write (p[1], write_msg, 60);
				COUNT = COUNT + 1;
				my_data->temperature = my_data->temperature+5;
				num_of_times=num_of_times+1;
			}
			break;
		}
	
	}

	pthread_cond_broadcast (&count_cond);		// broadcasts (releases) all waiting threads simultaneously
	pthread_mutex_unlock   (&count_lock);		// unlocks the critical region to be used by other threads 
	pthread_exit (NULL);
	
}

