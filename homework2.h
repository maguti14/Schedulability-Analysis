#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <asm/unistd_64.h>
#include <regex.h>
#include <sys/types.h>
#include <signal.h>
#include <linux/input.h>
#include <fcntl.h>
#include <math.h>

#define BUFFSIZE 2048
#define SIZE 4096


// ======================================= //
/* -------- Struct and Var Area --------- */
int iff; 											// sufficient and necessary flag
struct taskset *taskSet = NULL;

//task structure
struct task {
	double WCET;
	double deadline;
	int period;
};

// Tasksets are comprised of an array of tasks
struct taskset {
	struct task *tasks;
};	

//int analysisRT(struct task* tasks, int numTasks, int Di_index);


double EDF_percent[10] = {0.0}; 					//Array for computed % schedulable tasks under EDF
double RM_percent[10] = {0.0};						//Array for computed % schedulable tasks under RM
double DM_percent[10] = {0.0};						//Array for computed % schedulable tasks under DM

/* ================================== */
/* -------- Helper Functions -------- */
char* readline() {
	char* buffer = (char*)malloc(BUFFSIZE * sizeof(char));
	fgets(buffer, BUFFSIZE, stdin);
	int buff_len = strlen(buffer);
	char* line = (char*)malloc(buff_len * sizeof(char));

	for(int i = 0; i < strlen(buffer)-1; i++) {
        line[i] = buffer[i]; 
    }
    line[strlen(buffer)-1] = '\0';

	free(buffer);
	return line;
}

//for debugging
void print_schedulable(int schedulable, int scheduler) {
	
	if(scheduler == 0)
		printf("[EDF] ");
	else if(scheduler == 1)
		printf("[RM] ");
	else if(scheduler == 2)
		printf("[DM] ");


	if(schedulable == 1)
		printf("This task is schedulable!\n");
	else
		printf("This task is NOT schedulable!\n");
}


// Insertion sort for RM ordering
void sort_RM(struct task* tasks, int numTask_i)
{
	// Reference from https://www.geeksforgeeks.org/insertion-sort/
   int i, j;
   struct task key;
   for (i = 1; i < numTask_i; i++)
   {
       key = tasks[i];
       j = i-1;
 
       /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */
       while (j >= 0 && tasks[j].period > key.period)
       {
           tasks[j+1] = tasks[j];
           j = j-1;
       }
       tasks[j+1] = key;
   }
}

//Inserstion sort for DM ordering
void sort_DM(struct task* tasks, int numTask_i)
{
	// Reference from https://www.geeksforgeeks.org/insertion-sort/
   int i, j;
   struct task key;
   for (i = 1; i < numTask_i; i++)
   {
       key = tasks[i];
       j = i-1;
 
       /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */
       while (j >= 0 && tasks[j].deadline > key.deadline)
       {
           tasks[j+1] = tasks[j];
           j = j-1;
       }
       tasks[j+1] = key;
   }
}

double min(double a, double b) {
	double min;
	if(a < b)
	{
		min = a;
		iff = 0; // Di < Pi and so this is test is only sufficient, clear flag - inconclusive.
	}
	else
	{
		min = b;
	}
	return min;
}

double max(double a, double b) {
	return (a > b) ? a : b;									// return a if a > b; else return b
}

//Function to compute the busy period and perdorm loading analysis on given taskset
int analysisL(struct task* tasks, int numTasks) 
{
	int schedulable = 1;									// assume yes until you find a Ui > 1
	double t[SIZE] = {0.0};									// t is all the instances of deadlines that occur within our time interval (0, L]
	double h[SIZE] = {0.0};									// processor demand per given time interval in t
	double u[SIZE] = {0.0};									// loading factor per given time interval in t
	

	int index = 0;											// to be used to keep track of our 3 arrays
	double L = 0.0;											// busy period
	double tempL = 0.0;
	int count = 0;
	int withinInterval = 1;									// flag to keep track of deadlines
	double deadline = 0;
	double firstMissingD = -1.0;
	
	// initial L0
	for(int i = 0; i < numTasks; i++)
	{
		 L += tasks[i].WCET;
	}
	
	// Now iterate through to find the length of the busy period (0, L]
	while(tempL != L) // when L(m) = L(m+1) then you have your busy period
	{
		tempL = L;
		L = 0.0;
		for(int i = 0; i < numTasks; i++)
		{
			L += ceil(tempL/tasks[i].period)*tasks[i].WCET;
		}
	}

	// Now calculate t, h, u and see if every ui <= 1 then the taskset is schedulable
	// First t is just initial deadlines:
	for(int i = 0; i < numTasks; i ++)
	{
		t[i] = tasks[i].deadline;
		index = i; // keep track of index for h and u
	}

	while(withinInterval) // task deadline is within time interval keep adding deadline to array t
	{
		count++; // to keep track of how many times the period has passed before deadline
		withinInterval = 0; // clear flag
		// now keep adding periodic deadlines until L
		for(int i = 0; i < numTasks; i++) 
		{
			deadline = tasks[i].deadline + (tasks[i].period*count);
			if(deadline <= L) //task deadline is within interval
			{
				index++;
				t[index] = deadline; // add deadline to t array
				withinInterval = 1; // signal that there are still tasks within interval
			}
		}
	}

	withinInterval = -1;
	// Second, calculate h for every t
	for(int i = 0; i < index; i++)
	{
		for(int j = 0; j < numTasks; j++)
		{
			withinInterval = 1;
			count = 0;
			while(withinInterval)
			{
				withinInterval = 0; // clear flag
				deadline = tasks[j].deadline + (tasks[j].period*count);
				if(deadline <= t[i]) // job is part of processor demand
				{
					h[i] += tasks[j].WCET; // add to processor demand
					withinInterval = 1; // signal that task is still within interval
				}
				count++;
			}
		}
	}
	
	// Third, calculate u for every t
	for(int i = 0; i < index; i++)
	{
		u[i] = h[i] / t[i]; // calculate loading factor for each given interval (0, t[i]]
		//Finally, determine is it is schedulable (all Ui <= 1)
		if(u[i] > 1) // if ANY u[i] > 1
		{
			schedulable = 0; // NOT schedulable!
			if(t[i] < firstMissingD || firstMissingD == -1.0)
			{
				firstMissingD = t[i]; //get first missed deadline
			}
		}
	}
	//DEBUG print all array t, h, u
	/*
	for(int i = 0; i < 10; i++){
		printf(" t[%d] = %.2f   ",i,t[i]);
		printf(" h[%d] = %.2f   ",i,h[i]);
		printf(" u[%d] = %.2f\n",i,u[i]);
	}
	*/
	
	if(schedulable == 1)
		printf("Loading factor Analysis: Schedulable!\n");
	else
		printf("Loading factor Analysis: Not Schedulable!\n First missing deadline: %f\n", firstMissingD);
	
	return schedulable;
}

// Function to perform Response Time analysis on given task set, reports WCRT
int analysisRT(struct task* tasks, int numTasks, int Di_index) {	//Rt stands for response time
	//printf("RT\n");
	//for(int i = 0; i < Di_index; i++) {
	//	printf("[RT] tasks[%d]: WCET = %f\t deadline = %f\t period = %d\n",i,tasks[i].WCET, tasks[i].deadline, tasks[i].period);
	//}

	//printf("Di_index = %d\n",Di_index);
	double a = 0.0;
	double temp_a = 0.0;
	int schedulable = -1;
	double d = 0.0;
	
	// WC Response time must be less that the period of our slowest task or its deadline, whichever comes first.
	if(tasks[Di_index].deadline < tasks[Di_index].period)
	{
		d = tasks[Di_index].deadline;
	}
	else
	{
		d = tasks[Di_index].period;
	}

	// base case find a0
	for(int i = 0; i < Di_index; i++) {
		a += tasks[i].WCET;
	}
	//printf("a = %f\n",a);
	// Now iterate through and calculate WCRT
	while(temp_a != a) {
		temp_a = a;
		a = 0.0;
		for(int i = 0; i <= Di_index; i++) {	
			if(i == Di_index) {
				a += tasks[i].WCET;
			}
			else {
				a += ceil(temp_a / tasks[i].period) * tasks[i].WCET;
			}
		}
		//printf("temp_a = %f\n",temp_a);
		//printf("a = %f\n",a);
	}

	//printf("tasks[Di_index].period = %.2f\n",d);
	//check if WCRT is less than our deadline/period, if it is then we can schedule
	// else not
	if(a <= d) {
		schedulable = 1;
		printf("Schedulable by RT Analysis!\n");
		printf("Worst Case Response Time %f meets necessary finish time of %f\n", a, d);
	}
	else {
		schedulable = 0;
		printf("Not schedulable by RT Analysis!\n");
		printf("Worst Case Response Time %f exceeds necessary finish time of %f\n", a, d);
		
	}
	
	return schedulable;
}


// UUnifast algorithm for generating synthetic test tasksets from paper [2]
void UUniFast(double* vectorU, int numTasks, double U_bar) {								// U_bar = summation of U
	//generate value of sum n - 1 variables

	double sumU = U_bar;
	//printf("U_bar = %f\n", U_bar);
	for(int i = 1; i <= numTasks - 1; i++) {	// in matlab code loop goes from 1 <= n - 1, we start from 0 
		double nextSumU = sumU * pow(rand()/(RAND_MAX*1.0),(1.0 / (numTasks - i))); //rand()/RAND_MAX to normalized it other wise overflow
		vectorU[i-1] = sumU - nextSumU;
		//printf("vectorU[%d] = %f\n",i,vectorU[i]);
		sumU = nextSumU;
	}
	vectorU[numTasks-1] = sumU;
}


/* ------ End Helper Functions ------ */
/* ================================== */
