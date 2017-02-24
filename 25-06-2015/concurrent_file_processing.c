/* 25-06-15 solution by E.Sorbellini */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int N;

typedef struct _counter{
	int *v;
	pthread_mutex_t *lock;
}Counter;

Counter ca, cb;


void initialize_array(int *array, int N);
void *thFuncA(void *arg);
void *thFuncB(void *arg);

int main(int argc, char *argv[]){
	
	pthread_t *tha, *thb;
	int i;
	
	
	
	if(argc != 2){
		return -1;
	}
	
	
	N = atoi(argv[1]);
	
	if((N % 2) !=  0){
		return -1;
	}
	
	//initialize data structures
	tha = (pthread_t *)malloc(N * sizeof(pthread_t));
	thb = (pthread_t *)malloc(N * sizeof(pthread_t));
	ca.v = (int *)malloc(N * sizeof(int));
	cb.v = (int *)malloc(N * sizeof(int));
	ca.lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	cb.lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	initialize_array(ca.v, N);
	initialize_array(cb.v, N);
	
	//create A threads
	for(i=0; i<N; i++){
		int *j = (int *)malloc(sizeof(int));
		*j = i;
		pthread_create(tha, NULL, thFuncA, j);
	}
	
	//create B threads
	for(i=0; i<N; i++){
		int *j = (int *)malloc(sizeof(int));
		*j = i;
		pthread_create(thb, NULL, thFuncB, j);
	}
	
	
	
	pthread_exit(0);
	
}

void *thFuncA(void *arg){
	pthread_detach(pthread_self());
	
	int *kPtr = (int *)arg;
	int k = *kPtr;
	int i;
	int flag = 0;
	int A1 = k;
	int A2 = -2;
	int B1 = -2;
	int B2 = -2;
	
	//sleeps from 1 to 3 seconds
	sleep(1 + (rand() % 4));
	
	//file has already been concatenated
	pthread_mutex_lock(ca.lock);
	if(ca.v[k] >= 0 || ca.v[k] == -4){
		pthread_mutex_unlock(ca.lock);
		return 0;
	}
	
	for(i=0; i<N; i++){
		//is a second thread that has to concatenate
		if(ca.v[i] == -1 && i != k){
			ca.v[k] = i;
			ca.v[i] = k;
			A2 = i;
			printf("A%d cats A%d A%d\n", A1, A1, A2);
			flag = 1;
			pthread_mutex_unlock(ca.lock);
			break;
		}
	}
	//is a first thread mark a file as processed
	if(flag == 0){
		ca.v[k] = -1;
		pthread_mutex_unlock(ca.lock);
		return 0;
	}
	
	pthread_mutex_lock(ca.lock);
	pthread_mutex_lock(cb.lock);
	for(i=0; i<N; i++){
		if(cb.v[i] >= 0){
			B1 = i;
			B2 = cb.v[i];
			cb.v[B1] = -4;
			cb.v[B2] = -4;
			ca.v[A1] = -4;
			ca.v[A2] = -4;
			printf("A%d merge A%d A%d B%d B%d\n", A1, A1, A2, B2, B1);
			break;
		}
	}
	pthread_mutex_unlock(cb.lock);
	pthread_mutex_unlock(ca.lock);
	
			
	

	return 0;

}



void *thFuncB(void *arg){
	pthread_detach(pthread_self());
	
	int *kPtr = (int *)arg;
	int k = *kPtr;
	int i;
	int B1 = k;
	int B2 = -2;
	int A1 = -2;
	int A2 = -2;
	int flag = 0;
	
	//sleeps from 1 to 3 seconds
	sleep(1 + (rand() % 4));
	
	//file already processed
	pthread_mutex_lock(cb.lock);
	if(cb.v[k] >= 0 || cb.v[k] == -4){
		pthread_mutex_unlock(cb.lock);
		return 0;
	}
	
	for(i=0; i<N; i++){
		//is a second thread that has to concatenate
		if(cb.v[i] == -1 && i != k){
			cb.v[k] = i;
			cb.v[i] = k;
			B2 = i;
			printf("B%d cats B%d B%d\n", B1, B1, B2);
			flag = 1;
			pthread_mutex_unlock(cb.lock);
			break;
		}
	}
	//is a first thread mark a file as processed
	if(flag == 0){
		cb.v[k] = -1;
		pthread_mutex_unlock(cb.lock);
		return 0;
	}
	
	pthread_mutex_lock(ca.lock);
	pthread_mutex_lock(cb.lock);
	for(i=0; i<N; i++){
		if(ca.v[i] >= 0){
			A1 = i;
			A2 = ca.v[i];
			cb.v[B1] = -4;
			cb.v[B2] = -4;
			ca.v[A1] = -4;
			ca.v[A2] = -4;
			printf("B%d merge B%d B%d A%d A%d\n", B1, B1, B2, A2, A1);
			break;
		}
	}
	pthread_mutex_unlock(cb.lock);
	pthread_mutex_unlock(ca.lock);
	
	return 0;

}



void initialize_array(int *array, int N){
	int i;
	
	for(i=0; i<N; i++){
		array[i] = -2;
	}
}





