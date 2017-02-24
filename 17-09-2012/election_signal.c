/* 17-09-12 solution by E.Sorbellini */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>

#define M 10


typedef struct _best{
	int value;
	int th;
	pthread_mutex_t mutex;
}Best;

typedef struct _barrier{
	pthread_mutex_t mutex;
	sem_t sem;
	int count;
}Barrier;


Barrier barrier;
Best best;

sem_t sem;


void sigfunc(int signo);
void fill_rand(int *vektor, int size, int max, int flag);
void *thFunc(void *arg);

int main(){
	
	signal(SIGUSR1, sigfunc);
	int rank[M],i,selected[M-3];
	pthread_t th[M];
	
	best.value = 0;
	pthread_mutex_init(&(best.mutex), NULL);
	pthread_mutex_init(&(barrier.mutex), NULL);
	sem_init(&sem, 0, 0);
	sem_init(&(barrier.sem), 0, 0);
	
	fill_rand(rank, M, 100, 0);

	
	for(i=0; i<M; i++){
		int *j = (int *)malloc(sizeof(int));
		*j = rank[i];
		pthread_create(&th[i], NULL, thFunc, (void *)j);
	}
	
	while(1){
		printf("\n");
		sleep(2 + (rand() % 3));
		fill_rand(selected, M-3, M-1, 1);
		for(i=0; i<M-3; i++){
			pthread_kill(th[selected[i]], SIGUSR1);
		}
	}
	
	printf("Main exit\n");
	
	pthread_exit(0);
}

void *thFunc(void *arg){
	pthread_detach(pthread_self());
	int *rankPtr = (int *)arg;
	int rank = *rankPtr;
	int bestRank;
	int bestTh;
	int i;
	
	
	while(1){
		//wait SIGUSR1 handler do the job
		sem_wait(&sem);
		while(1){
			pthread_mutex_lock(&(best.mutex));
			if(best.value < rank){
				best.value = rank;
				best.th = (int)pthread_self();
			}
			bestRank = best.value;
			bestTh = best.th;
			pthread_mutex_unlock(&(best.mutex));
			
			pthread_mutex_lock(&(barrier.mutex));
			barrier.count++;
			if(barrier.count == M-3){
				printf("Thread %d rank = %d, Best rank = %d Best Th = %d\n", (int)pthread_self(), rank, bestRank, bestTh);
					
					barrier.count = 0;
					bestRank = 0;
					bestTh = 0;
					best.th = 0;
					best.value = 0;
					for(i=0; i<M-3; i++)
						sem_post(&(barrier.sem));
					sem_wait(&(barrier.sem));
					pthread_mutex_unlock(&(barrier.mutex));
					break;	
			}
			else{
				//printf("Thread %d rank = %d\n", (int)pthread_self(), rank);
				pthread_mutex_unlock(&(barrier.mutex));
				//printf("waiting to be unlocked\n");
				sem_wait(&(barrier.sem));
				break;
			}
		}
	}
}
	
	
void fill_rand(int *vektor, int size, int max, int flag){
	
	
	int in=0, im=0, i=0;

	srand(time(NULL));

	im = 0;

	for (in = 0; in < max && im < size; ++in) {
		int rn = max - in;
		int rm = size - im;
		if (rand() % rn < rm){    
		/* Take it */
			if(flag == 0){
				vektor[im++] = in + 1; /* +1 since your range begins from 1 */
			}
			else{
				vektor[im++] = in;
			}
		}
	}
    
    //if you want not ordered shuffle array
	for (i = 0; i < size; i++) { 
		int temp = vektor[i];
		int randomIndex = rand() % size;

		vektor[i] = vektor[randomIndex];
		vektor[randomIndex] = temp;
	}
	
	return;
    
}

void sigfunc(int signo){
	//printf("Post\n");
	sem_post(&sem);
}
