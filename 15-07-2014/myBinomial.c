/* 15-07-14 solution by E.Sorbellini */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int *nVal;
int nSize;
int *dVal;
int dSize;
float result=1;
float numer=1;
float denom=1;


void *nFunc(void *arg);
void *dFunc(void *arg);

typedef struct barrier{
	int count;
	pthread_mutex_t lock;
	sem_t bar;
}Barrier;

Barrier b;

int main(int argc, char *argv[]){
	
	if(argc != 3){
		printf("Usage %s: <n> <k>\n",argv[0]);
		return -1;
	}
	
	pthread_t numerator,denominator;
	int n;
	int i;
	int k;
	int diff;
	int val;
	
	n = atoi(argv[1]);
	k = atoi(argv[2]);
	
	if(k > n){
		printf("Error: n should be >= k\n");
		return -1;
	}
	
	if(n == 0 || k == 0){
		printf("Result = 1\n");
		return -1;
	}
	
	diff = n-k;
	
	nSize = n-diff;
	dSize = k;
	
	nVal = (int *)malloc((n - diff) * sizeof(int));
	dVal = (int *)malloc(k * sizeof(int));
	
	//populate numerator values
	val = n;
	for(i=0; i < (n-diff); i++){
		nVal[i] = val;
		val--;
	}
	
	//debug
	/*
	for(i=0; i < (n-diff); i++){
		printf("%d ",nVal[i]);
	}
	
	printf("\n");
	*/
	//populate denominator
	val = k;
 	for(i=0; i < k; i++){
		dVal[i] = val;
		val--;
	}
	
	//debug
	/*
	for(i=0; i < k; i++){
		printf("%d ",dVal[i]);
	}
	
	printf("\n");
	*/
	b.count = 0;
	sem_init(&b.bar, 0 ,0);
	pthread_mutex_init(&b.lock, NULL);
	
	pthread_create(&numerator, NULL, nFunc, NULL);
	pthread_create(&denominator, NULL, dFunc, NULL);
	
	pthread_join(numerator, NULL);
	pthread_join(denominator, NULL);
	
	printf("Result = %.2f\n", result);
	
	
	
	
	return 0;
}

void *nFunc(void *arg){
	
	int i =0;
	int first;
	int second;
	int last = 0;
	while(1){
		first = nVal[i];
		i++;
		if(i >= nSize){
			second = 1;
			last = 1;
		}
		else{
			second = nVal[i];
			i++;
			if(i >= nSize)
				last = 1;
		}
		//compute product and save in numer
		numer *= first;
		numer *= second;
		
		pthread_mutex_lock(&b.lock);
		b.count++;
		if(b.count == 2){
			result *= (numer/denom);
			numer = 1;
			denom = 1;
			b.count = 0;
			sem_post(&b.bar);
			sem_post(&b.bar);
		}
		pthread_mutex_unlock(&b.lock);
		
		if(last == 1)
			break;
		
		sem_wait(&b.bar);
		

		
		
	}
	
	pthread_exit(0);
}
	
	
void *dFunc(void *arg){
	int i =0;
	int first;
	int second;
	int last = 0;
	while(1){
		first = dVal[i];
		i++;
		if(i >= dSize){
			second = 1;
			last = 1;
		}
		else{
			second = dVal[i];
			i++;
			if(i >= dSize)
				last = 1;
		}
		//compute product and save in numer
		denom *= first;
		denom *= second;
		
		pthread_mutex_lock(&b.lock);
		b.count++;
		if(b.count == 2){
			result *= (numer/denom);
			numer = 1;
			denom = 1;
			b.count = 0;
			sem_post(&b.bar);
			sem_post(&b.bar);
		}
		pthread_mutex_unlock(&b.lock);
		
		if(last == 1)
			break;
		
		sem_wait(&b.bar);
		

		
		
	}
	pthread_exit(0);
}	
		
	
