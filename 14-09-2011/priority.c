/* 14-09-11 exam solution by E.Sorbellini */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>



int fp[2];

typedef struct _message{
	int id;
	int class;
	int n;
}Message;

typedef struct _queue{
	pthread_mutex_t mutex;
	int count;
}Queue;

Queue q1,q2;

pthread_mutex_t me;

sem_t sem1,sem2,sem3;


void *thFunc(void *arg);
void fill_rand(int *vektor, int size, int max, int flag);

int main(int argc, char *argv[]){
	
	int k,i;
	int fd;
	pthread_t *th;
	int *priorities;
	Message m;
	char message[200];
	srand(time(NULL));
	time_t rawtime;
	struct tm * timeinfo;
	
	if(argc != 3){
		return -1;
	}
	
	k = atoi(argv[1]);
	printf("K: %d\n", k);
	
	fd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0777);

	th = (pthread_t *)malloc(k * sizeof(pthread_t));
	priorities = (int *)malloc(k * sizeof(int));
	
	//initializes mutexes and counters
	pthread_mutex_init(&(q1.mutex), NULL);
	pthread_mutex_init(&(q2.mutex), NULL);
	pthread_mutex_init(&me, NULL);
	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 1);
	sem_init(&sem3, 0, 1);
	
	q1.count = 0;
	q2.count = 0;
	
	for(i=0;i<k;i++){
		priorities[i] = 1 + (rand()  % 3);
	}
	
	pipe(fp);
	
	for(i=0; i<k; i++){
		int *j = (int *)malloc(sizeof(int));
		*j = priorities[i];
		//printf("%d ", *j);
		pthread_create(&th[i], NULL, thFunc, j);
	}
	
	
	while(1){
		read(fp[0], &m, sizeof(m));
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		sprintf(message, "%d %d %d %s\n", m.id, m.class, m.n, asctime(timeinfo));
		write(fd, message, strlen(message));
	}
}


void *thFunc(void *arg){
	pthread_detach(pthread_self());
	int *pPtr = (int *)arg;
	int p = *pPtr;
	Message m;
	
	while(1){
		
		switch(p){
			case 1:
				pthread_mutex_lock(&(q1.mutex));
				q1.count++;
				pthread_mutex_unlock(&(q1.mutex));
				
				//write message requesting access
				m.id = (int)pthread_self();
				m.class = p;
				m.n = 0;
				write(fp[1], &m, sizeof(Message));
				
				pthread_mutex_lock(&me);
				//write message inside requesting access
				m.id = (int)pthread_self();
				m.class = p;
				m.n = 1;
				write(fp[1], &m, sizeof(Message));
				pthread_mutex_unlock(&me);
				

				pthread_mutex_lock(&(q1.mutex));
				q1.count--;
				pthread_mutex_unlock(&(q1.mutex));
				sleep(2 + (rand() % 9));
				break;
			
			case 2:
			
				pthread_mutex_lock(&(q2.mutex));
				q2.count++;
				pthread_mutex_unlock(&(q2.mutex));
			
				while(1){
					//write message requesting access
					m.id = (int)pthread_self();
					m.class = p;
					m.n = 0;
					write(fp[1], &m, sizeof(Message));
					
					pthread_mutex_lock(&(q1.mutex));
					if(q1.count == 0){
						pthread_mutex_unlock(&(q1.mutex));
						
						pthread_mutex_lock(&me);
						//write message inside requesting access
						m.id = (int)pthread_self();
						m.class = p;
						m.n = 1;
						write(fp[1], &m, sizeof(Message));
						pthread_mutex_unlock(&me);
						
						break;
					}
					else{
						pthread_mutex_unlock(&(q1.mutex));
						sleep(2 + (rand() % 9));
					}
				}

				pthread_mutex_lock(&(q2.mutex));
				q2.count--;
				pthread_mutex_unlock(&(q2.mutex));
				sleep(2 + (rand() % 9));
				break;
			
			case 3:
			
				while(1){
					//write message requesting access
					m.id = (int)pthread_self();
					m.class = p;
					m.n = 0;
					write(fp[1], &m, sizeof(Message));
					
					pthread_mutex_lock(&(q1.mutex));
					pthread_mutex_lock(&(q2.mutex));
					if(q1.count == 0 && q2.count ==0){
						pthread_mutex_unlock(&(q2.mutex));
						pthread_mutex_unlock(&(q1.mutex));
						
						pthread_mutex_lock(&me);
						//write message inside requesting access
						m.id = (int)pthread_self();
						m.class = p;
						m.n = 1;
						write(fp[1], &m, sizeof(Message));
						pthread_mutex_unlock(&me);
						
						break;
					}
					else{
						pthread_mutex_unlock(&(q2.mutex));
						pthread_mutex_unlock(&(q1.mutex));
						sleep(2 + (rand() % 9));
					}
				}
				sleep(2 + (rand() % 9));
				break;
		}
	}
}
		
	

		
