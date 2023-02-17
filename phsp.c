#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>

enum state {HUNGRY, EATING, THINKING};
enum distribution {exponential, uniform};
sem_t mutex;
sem_t next;


int num_phsp;
int min_think;
int max_think;
int min_dine;
int max_dine;
int num;
char distr[10];


enum distribution dst;

struct monitor_dp{
	pthread_mutex_t *mutex;
	pthread_cond_t *status;
	enum state *philosopher_state;
}sticks;

//Philosophers initially in thinking state
void init_monitor(int num){
	for(int i =0 ; i<num; i++){
		sticks.philosopher_state[i] = THINKING;
	}
}


//Generates a random value for thinking and dining times according to the type of distribution
int rand_time(int max, int min){
    
    double randomized_time = 0;

    if(max == min){
        return max;
    }
  
    if(dst == uniform){
        while(randomized_time > max || randomized_time < min){
            double random_number = ((double)rand() / RAND_MAX);
            randomized_time = max * random_number;
        }
    } else if(dst == exponential) {
        while(randomized_time > max || randomized_time < min){
            double random_number = ((double)rand() / RAND_MAX);
            randomized_time =(-((double)(min + max)) / 2) * log(random_number);
        }
    } else {
        exit(0);
    }

    return (int)randomized_time;

}


void test(int i)
{
	//If neighbours are not eating, phps(i) can eat
	if (sticks.philosopher_state[i] == HUNGRY && sticks.philosopher_state[(i+num_phsp-1)% num_phsp] != EATING && 		sticks.philosopher_state[(i+1)% num_phsp] != EATING){
	
	sticks.philosopher_state[i] = EATING;
		
	printf("Philosopher %d is eating\n", i + 1);
	
	pthread_cond_signal(&sticks.status[i]);
	}
}


int take_chopsticks(int i)
{
	sticks.philosopher_state[i] = HUNGRY;
    
    printf("Philoshoper %d is hungry. \n", i + 1);
    
    clock_t start = clock();
    test(i);
    int lock = -1;
    
    if(sticks.philosopher_state[i] != EATING){
        
        if(sticks.philosopher_state[(i + 1) % num_phsp] == EATING){
        
            lock = (i + 1) % num_phsp;
            pthread_cond_wait(&sticks.status[i], &sticks.mutex[(i + 1) % num_phsp]);
        
        } 
        else{   
        
            lock = i;
            pthread_cond_wait(&sticks.status[i], &sticks.mutex[i]);
        
        }

    } 
    else{ 
        
        pthread_mutex_lock(&sticks.mutex[i]);
        pthread_mutex_lock(&sticks.mutex[(i + 1) % num_phsp]);
    
    }

    
    if(lock > 0){
        
        if(lock == i){
        
            pthread_mutex_lock(&sticks.mutex[(i + 1) % num_phsp]);
        
        } else{
          
            pthread_mutex_lock(&sticks.mutex[i]);
        
        }
    }

    return (clock() - start) * 1000 / CLOCKS_PER_SEC;
}

void put_chopsticks(int i)
{
	//sem_wait(&mutex);
	sticks.philosopher_state[i] = THINKING;
	pthread_mutex_unlock(&sticks.mutex[i]);
    pthread_mutex_unlock(&sticks.mutex[(i + 1) % num_phsp]);

    printf("Philosopher %d is thinking. \n", i + 1);

    test((i + num_phsp - 1) % num_phsp);
    test((i + 1) % num_phsp);
}

void* create_philosopher(void* arg){

    int id = *((int *)arg);
    int current_rpt = 0;
    int total_ht = 0;

    printf("Philosopher %d started to think. \n", (id + 1));

    while(current_rpt < num){
        
        usleep((useconds_t)(rand_time(max_think, min_think) * 1000));
        
        total_ht = total_ht + take_chopsticks(id);
        
	// Generate random number for eating time
        int eating_time = rand_time(max_dine, min_dine);
        
        clock_t start = clock();
        clock_t elapsed;
        
        int milisec;
        
        while(eating_time>= milisec){

            elapsed = clock() - start;
            milisec = elapsed * 1000 / CLOCKS_PER_SEC;

        }

        
        put_chopsticks(id);

        current_rpt = current_rpt + 1;
    }

    
    printf("Philosopher %d stayed in hungry state for %d\n", (id + 1), total_ht);

    pthread_exit(NULL);

}


int main(){
    
    char str2[] = "uniform";
    char str3[] = "exponential";
   
   //-----------------------------------------------------
    printf("Enter the number of philosophers:");
    scanf("%d", &num_phsp);
    if(num_phsp % 2 == 0 || num_phsp > 27){
        printf("Number of philosophers cannot be more than 27.\n"); 
        exit(0);
    }
    
    if(num_phsp % 2 == 0 || num_phsp > 27){
        printf("Number of philosophers cannot be more than 27.\n"); 
        exit(0);
    }
    //-------------------------------------------------
    printf("Enter minimum thinking time:\n");
    scanf("%d", &min_think);
    if(min_think < 1){
        printf("Thinking time cannot be less than than 1 ms. \n");        
        exit(0);    
    }
    //------------------------------------------------
    printf("Enter maximum think time:\n");
    scanf("%d", &max_think);
    if(max_think > 60000){
        printf("Thinking time caanot be more than 60 seconds. \n");        
        exit(0);
    }
    //--------------------------------------------------
    printf("Enter minimum dining time:\n");
    scanf("%d", &min_dine);       
    if(min_dine < 1){
        printf("Dining time cannot be less than than 1 ms. \n");        
        exit(0);
    }
    //----------------------------------------------
    printf("Enter maximum dining time:\n");
    scanf("%d", &max_dine);
    if(max_dine > 60000){
        printf("Dining time cannot be more than 60 seconds. \n");        
        exit(0);
    }
    //-----------------------------------------------
    printf("Enter dining repetition:\n");
    scanf("%d", &num);
    //------------------------------------------------
    printf("Enter distribution:\n");
    scanf("%s",distr);
     if(strcmp(distr, str3) == 0){
        dst = exponential;
    }
    else if(strcmp(distr, str2) == 0){
        dst = uniform;
    }
    else {
        printf("Please enter a valid distribution parameter. \n");
        exit(0);
    }
    //--------------------------------------------------------
    



   sticks.mutex=(pthread_mutex_t *)calloc(10, sizeof(pthread_mutex_t) * num_phsp);
   sticks.status = (pthread_cond_t *)calloc(10, sizeof(pthread_cond_t) * num_phsp);
    sticks.philosopher_state = (enum state *)calloc(10,sizeof(enum state) * num_phsp);

    init_monitor(num_phsp);

    for(int i = 0; i < num_phsp; i++){
        pthread_mutex_init(&sticks.mutex[i], NULL);
        pthread_cond_init(&sticks.status[i], NULL);
    }

    
    

    pthread_t threads[num_phsp];
    int Number[num_phsp];
    for(int i = 0; i < num_phsp; i++){
        Number[i] = i;
    }

    for(int i = 0; i < num_phsp; i++){
        pthread_create(&threads[i], NULL, create_philosopher, (void *)&Number[i]);
    }

    
    for(int i = 0; i < num_phsp; i++){
        pthread_join(threads[i], NULL);
    }

    for(int i = 0; i < num_phsp; i++){
        pthread_mutex_destroy(&sticks.mutex[i]);
        pthread_cond_destroy(&sticks.status[i]);
    }
    
    exit(0);
}
