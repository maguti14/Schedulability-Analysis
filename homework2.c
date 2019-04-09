#include "homework2.h"
#define M 3
#define USIZE 11
// ========================================= //
/* ------------- Function Area ------------- */
void initialize_tasks(struct task* tasks,int numTask){
	for(int i = 0; i < numTask; i++) {
		tasks[i].WCET = 0.0;
		tasks[i].deadline = 0.0;
		tasks[i].period = 0;
	}
}


int EDF(struct task* tasks, int numTask_i) {			//argument is an array of tasks and the number of Task Sets

	printf("******************************** EDF **********************************\n");
	double task_density = 0.0;
	double tot_task_dens = 0.0;
	int schedulable = -1;
	iff = 1; 											// set sufficient and necessary flag
	
	// perform utilization analysis
	printf("Performing utilization analysis!\n");
	for(int i = 0; i < numTask_i; i++) {
		task_density = tasks[i].WCET / min(tasks[i].deadline,tasks[i].period);
		//printf("Task: %d, WCET = %f, period = %d, deadline = %f;\n", i, tasks[i].WCET, tasks[i].period, tasks[i].deadline);
		tot_task_dens += task_density;
	}
	printf("Total Util = %f\n", tot_task_dens);			
	
	// Theorem: A set of n periodic tasks can be scheduled by EDF iff total utilization <= 1
	// Howewver, for sporadic tasks (Di < Pi) this is only sufficent therefore we require futher analysis of load factor
	if(tot_task_dens <= 1)
	{
		printf("Schedulable by EDF. Utilization = %f\n", tot_task_dens);
		schedulable = 1;
	}
	else if (tot_task_dens > 1 && iff == 0) 
	{										
		printf("Test is inconclusive, perform analysis of load factor!\n");
		schedulable = analysisL(tasks,numTask_i);
	}
	else if(tot_task_dens > 1 && iff == 1)
	{
		printf("Not Schedulable by EDF. Utilization = %f\n", tot_task_dens);
		schedulable = 0;
	}
	else // error
	{
		schedulable = -2;
		printf("EDF ERROR\n");
		exit(1);
	}

	//print_schedulable(schedulable,0);
	printf("***********************************************************************\n");
	return schedulable;
}

int RM(struct task* tasks, int numTask_i) {
	printf("********************************* RM **********************************\n");
	iff = -1;

	int schedulable = -1;
	int Di_eq_Pi = 1;		// assume that they all have equal Di == Pi
	
	sort_RM(tasks,numTask_i);						//sort the array of tasks wrt their priorities
	//for(int i = 0; i < 10; i++) {
	//	printf("[RM] After sorted tasks[%d]: WCET = %f\t deadline = %f\t period = %d\n",i,tasks[i].WCET, tasks[i].deadline, tasks[i].period);
	//	printf("\n");
	//}

	for(int i = 0; i < numTask_i; i++) {
		if(tasks[i].deadline < tasks[i].period) { 		// if one of the tasks has deadline less than period
			Di_eq_Pi = 0;								// then switch Di_eq_Pi flag to 0
		}
	}
	
	double tot_util = 0.0;
	int index = 0;


	for(int i = 0; i < numTask_i; i++) { // do we need this for loop or can we just do it for numTask_i
		
		//double ultilization = tasks[i].WCET / min(tasks[i].period, tasks[i].deadline); // min for when Di < Pi
		double ultilization = tasks[i].WCET / tasks[i].period;
		tot_util += ultilization;
		
		printf("Performing utilization analysis for task %d!\n", i);
		double UB = (i + 1)*(pow(2.0,1.0 / (i+1)) - 1); // i + 1 because the index starts at 0
		//printf("tot_util = %f\n",tot_util);
		//printf("UB = %f\n",UB);
		
		if(tot_util > 1) 		//NOT schedulable
		{
			printf("Not schedulable by RM. Utilization = %f\n", tot_util);
			schedulable = 0;	//Done
			break;
		}
		else if((Di_eq_Pi == 1) && (tot_util < UB)) {
			printf("Schedulable by RM. Utilization = %f\n", tot_util);
			schedulable = 1;	//Done
			//printf("tasks[%d]\n",i);
		}
		else {	//if(tot_util > UB) or/and Di < Pi 
			printf("UB test is inconclusive, perform response time analysis!\n");
			index = i;
			schedulable = analysisRT(tasks,numTask_i,index);
			if(schedulable == 0)
			{
				break;
			}
		}
		/*
		index = i;
		schedulable = analysisRT(tasks,numTask_i,index);
		if(schedulable == 0)
		{
			break;
		}
		*/
	}

	//print_schedulable(schedulable,1);
	printf("***********************************************************************\n");
	return schedulable;
}

int DM(struct task* tasks, int numTask_i) {
	printf("********************************* DM **********************************\n");
	iff = -1;
	int schedulable = -1;
	int Di_eq_Pi = 1;	

	//for(int i = -; i < 5; i++) {
	//	printf("Before sorted tasks tasks[%d]: WCET = %f\t deadline = %f\t period = %f\n",i,tasks[i].WCET, tasks[i].deadline, tasks[i].period);
	//}
	sort_DM(tasks, numTask_i);							// sort differently

	//for(int i = 0; i < 5; i++) {
	//	printf("[DM] After sorted tasks[%d]: WCET = %f\t deadline = %f\t period = %d\n",i,tasks[i].WCET, tasks[i].deadline, tasks[i].period);
	//	printf("\n");
	//}

	for(int i = 0; i < numTask_i; i++) {
		if(tasks[i].deadline < tasks[i].period) { 		// if one of the tasks has deadline less than period
			Di_eq_Pi = 0;								// then switch Di_eq_Pi flag to 0
		}
	}
	
	double tot_util = 0.0;
	int index = 0;

	for(int i = 0; i < numTask_i; i++) { // do we need this for loop or can we just do it for numTask_i
		
		//double ultilization = tasks[i].WCET / min(tasks[i].period, tasks[i].deadline); // min for when Di < Pi
		double ultilization = tasks[i].WCET / tasks[i].period;
		tot_util += ultilization;
		
		printf("Performing utilization analysis for task %d!\n", i);
		double UB = (i + 1)*(pow(2.0,1.0 / (i+1)) - 1); // i + 1 because the index starts at 0

		//printf("tot_util = %f\n",tot_util);
		//printf("UB = %f\n",UB);	
		
		if(tot_util > 1) //NOT schedulable
		{
			schedulable = 0;	//Done
			printf("Not schedulable by RM. Utilization = %f\n", tot_util);
			break;
		}
		else if((Di_eq_Pi == 1) && (tot_util < UB)) { 
			printf("Schedulable by RM. Utilization = %f\n", tot_util);
			schedulable = 1;	//Done
			//printf("tasks[%d]\n",i);
		}
		else {	//if(tot_util > UB) or/and Di < Pi 
			printf("UB test is inconclusive, perform response time analysis!\n");
			index = i;
			schedulable = analysisRT(tasks,numTask_i,index);
			if(schedulable == 0)
			{
				break;
			}
		}
		
		/*
		index = i;
		schedulable = analysisRT(tasks,numTask_i,index);
		if(schedulable == 0)
		{
			break;
		}
		*/
	}

	//print_schedulable(schedulable,1);
	printf("***********************************************************************\n");
	return schedulable;
}

unsigned int period_URD(unsigned int low, unsigned int high) {
	// Reference https://stackoverflow.com/questions/11641629/generating-a-uniform-distribution-of-integers-in-c
	//int r = low + (rand()/(RAND_MAX * 1.0)) * (high-low);
	

	// Referencehttp://eternallyconfuzzled.com/arts/jsw_art_rand.aspx
	//unsigned int r = rand()/(RAND_MAX*1.0)*(high-low)+low;

	// Most recent reference http://c-faq.com/lib/randrange.html
    unsigned int r = low + rand() / (RAND_MAX / (high - low + 1) + 1);
	//printf("low = %d\n",low);
	//printf("high = %d\n",high);
	//printf("r = %d\n",r);

    return r;
	//int scaled = (double)rand()/RAND_MAX; // reference https://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
    //return (high - low +1) * scaled + low;
}

double deadline_URD( double low, double high) {

	//double r = low + (rand()/(RAND_MAX*1.0)) * (high-low);
	//double r = rand()/(RAND_MAX*1.0)*(high-low)+low;
  	double r = low + rand() / (RAND_MAX * 1.0 / (high - low + 1.0) + 1.0);
  	//double r = rand()/(RAND_MAX*1.0)*(high-low)+low;
  	//double r =  low + (rand()/(RAND_MAX*1.0)) * (high-low);
  	//double scaled = (double)rand()/RAND_MAX; // reference https://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
    //return (high - low +1) * scaled + low;
    //printf("low = %f\n",low);
	//printf("high = %f\n",high);
	//printf("r = %f\n",r);
    return r;
}

// This function will generate the respective percent schedulable tasks for the given level of utilization
void gen_point(int numTasks, int D, double U_bar, int percent_index) {

	// Arrays to keep track of which tasksets are schedulable for each alg
	int* EDF_schedulable = (int*)malloc(5000*sizeof(int));
	int* RM_schedulable = (int*)malloc(5000*sizeof(int));
	int* DM_schedulable = (int*)malloc(5000*sizeof(int));
	double* vectorU = NULL;											// Populated by UUnifast

	//initialization
	for(int i = 0; i < 5000; i++) {
		EDF_schedulable[i] = 0;
		RM_schedulable[i] = 0;
		DM_schedulable[i] = 0;
	}

	// Synthesize the 5000 task sets with the specified WCET using UUnifast and deadline within the given distribution D
	for(int i = 0; i < 5000; i++) {
		vectorU = (double*)malloc((numTasks) * sizeof(double));
		UUniFast(vectorU,numTasks,U_bar);
		for(int j = 0; j < numTasks; j++) {
			
			taskSet[i].tasks[j].WCET = vectorU[j] * taskSet[i].tasks[j].period;
			
			if(D == 0) { // Ci, Ti
				double l = taskSet[i].tasks[j].WCET;
				double u = taskSet[i].tasks[j].period;
				taskSet[i].tasks[j].deadline = deadline_URD(l,u);
			}
			else { //other one
				double l = taskSet[i].tasks[j].WCET + (taskSet[i].tasks[j].period - taskSet[i].tasks[j].WCET) / 2.0;
				double u = taskSet[i].tasks[j].period;			
				taskSet[i].tasks[j].deadline = deadline_URD(l,u);
			}
		}// end inner for loop
		free(vectorU);
	} // end outer for loop

	// DEBUG
	/*
	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < numTasks; j++) {
			printf("taskSet[%d].tasks[%d].WCET = %f\n",i,j,taskSet[i].tasks[j].WCET);
			printf("taskSet[%d].tasks[%d].deadline = %f\n",i,j,taskSet[i].tasks[j].deadline);
			printf("taskSet[%d].tasks[%d].period = %d\n",i,j,taskSet[i].tasks[j].period);
		}
	}
	*/
	
	// Now run analyses
	for(int i = 0; i < 5000; i++) {
		EDF_schedulable[i] = EDF(taskSet[i].tasks,numTasks);
		RM_schedulable[i] = RM(taskSet[i].tasks,numTasks);
		DM_schedulable[i] = DM(taskSet[i].tasks,numTasks);
	} 

	//now calculate percentages here!!!
	double sum_EDF = 0.0;
	double sum_RM = 0.0;
	double sum_DM = 0.0;
	
	for(int i = 0; i < 5000; i++) {
		if(EDF_schedulable[i] == 1) {
			sum_EDF++;
		}
		else if(EDF_schedulable[i] == 0) {
			//do nothing
		}
		else{
			printf("EDF ERROR!\n");
		}
		
		if(RM_schedulable[i] == 1) {
			sum_RM++;
		}
		else if(RM_schedulable[i] == 0) {
			//do nothing
		}
		else{
			printf("RM ERROR!\n");
		}
		
		if(DM_schedulable[i] == 1) {
			sum_DM++;
		}
		else if(DM_schedulable[i] == 0) {
			//do nothing
		}
		else{
			printf("DM ERROR!\n");
		}
	}	

	//printf("sum_EDF = %f\n",sum_EDF);
	EDF_percent[percent_index] = sum_EDF / 5000.0;
	RM_percent[percent_index] = sum_RM / 5000.0;
	DM_percent[percent_index] = sum_DM / 5000.0;
	
	free(EDF_schedulable);
	free(RM_schedulable);
	free(DM_schedulable);
}


void gen_plot(int numTasks, int D) {

	double U_bar[USIZE] = {0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95, 0.99}; // the different levels of util we will be testing

	taskSet = (struct taskset*)malloc(5000 * sizeof(struct taskset)); //create 5000 tasksets

	for(int i = 0; i < 5000; i++) {

		taskSet[i].tasks = (struct task*)malloc(numTasks * sizeof(struct task));	// create an array of struct tasks;
		initialize_tasks(taskSet[i].tasks,numTasks);
		
		// Fill in the periods with an M = 3 as per [1]
		for(int j = 0; j < numTasks; j++) {
			if((numTasks % M) == 0) {	//M should be 3
				taskSet[i].tasks[j].period = period_URD(1000,10000);
			}
			else if((numTasks % M) == 1) {
				taskSet[i].tasks[j].period = period_URD(10000,100000);
			}
			else {
				taskSet[i].tasks[j].period = period_URD(100000,1000000);
			}
		}
	} // after this for loop every task will have a period

	// now generate each point
	for(int i = 0; i < USIZE; i++) {
		gen_point(numTasks, D, U_bar[i],i);
	} // after this we should have complete plot info

	//Print out results so we can we generate plots in excel
	printf("EDF_percent\n");
	for(int i = 0; i < USIZE; i++) {
		
		printf("%f\n",EDF_percent[i]);
	}
	printf("RM_percent\n");
	for(int i = 0; i < USIZE; i++) {
		
		printf("%f\n",RM_percent[i]);
	}
	printf("DM_percent\n");
	for(int i = 0; i < USIZE; i++) {
		printf("%f\n",DM_percent[i]);
	}
	
	free(taskSet);
}	


int main(int argc, char* argv[]) {	
	srand(time(NULL));
	
	if(argc == 1)
	{
		char* str_ptr = NULL;                                   // Only need to create one str_pointer instead of one at each strtok_r
    	char* firstLine = readline();                           // Get a line; represents how many task sets
		str_ptr = strtok_r(firstLine," ",&firstLine);           // get taskSet

		int numTaskSets = atoi(str_ptr);                        // convert to int
		//printf("convert numTaskSets = %d\n",numTaskSets);

		int* schedEDF = (int*)malloc(numTaskSets * sizeof(int));// array to keep track of which task sets are schedulable under EDF
		int* schedRM = (int*)malloc(numTaskSets * sizeof(int)); // array to keep track of which task sets are schedulable under RM
		int* schedDM = (int*)malloc(numTaskSets * sizeof(int));   // array to keep track of which task sets are schedulable under DM

		for(int i = 0; i < numTaskSets; i++){                    // loop to get all task sets

		    char* line = readline();                            // Get a line
		    str_ptr = strtok_r(line," ",&line);                 // Number of tasks within Task_i
		    int numTask_i = atoi(str_ptr);                      // convert Number of tasks to int
		    //printf("numTask_i = %d\n",numTask_i);

		    struct task* tasks = (struct task*)malloc(numTask_i * sizeof(struct task)); // create an array of struct tasks
		    printf("Taskset number %d\n",i);
		    for(int j = 0; j < numTask_i; j++) {             // Loop until number of tasks in Task_i
		        //initialize_task(&tasks[j]);                     // initialize each task

		        char* nextline = readline();

		        str_ptr = strtok_r(nextline," ",&nextline);
		        //double WCET = atof(str_ptr);
		        tasks[j].WCET = atof(str_ptr);
		        //printf("task[%d].WCET = %f\n",j,tasks[j].WCET);

		        str_ptr = strtok_r(nextline," ",&nextline);
		        tasks[j].deadline = atof(str_ptr);
		        //printf("task[%d].deadline = %f\n",j,tasks[j].deadline);


		        str_ptr = strtok_r(nextline," ",&nextline);
		        tasks[j].period = atoi(str_ptr);
		        //printf("task[%d].period = %d\n",j,tasks[j].period);
		    }

		    schedEDF[i] = EDF(tasks,numTask_i);
		    schedRM[i] = RM(tasks,numTask_i);
		    schedDM[i] = DM(tasks,numTask_i);
		    // pass the tasks array in the analysis engines HERE
		    // pass these tasks into EDF, RM, and DM for analysis????

		    free(tasks);    // mem management because we're cool!!!
		}
    }
    else
    {
		//int D = 0; // D = 0 => [Ci,Ti], D = 1 => []
	
		//Note that the report is pretty lengthy, if you want to be be able to see the plotting information at the end maybe only generate one plot at a time!
		printf("--------- Plot 1: [10 Tasks] with [Ci, Ti] ---------\n");
		gen_plot(10,0);	// [Ci,Ti]
	
		printf("--------- Plot 2: [25 Tasks] with [Ci, Ti] ---------\n");
		gen_plot(25,0);	// [Ci,Ti]

		printf("--------- Plot 3: [10 Tasks] with [Ci + (Ti - Ci) / 2, Ti] ---------\n");
		gen_plot(10,1);	// [Ci + (Ti-Ci)/2, Ti]
	
		printf("--------- Plot 4: [25 Tasks] with [Ci + (Ti - Ci) / 2, Ti] ---------\n");
		gen_plot(25,1);	// [Ci + (Ti-Ci)/2, Ti]
	}
}
