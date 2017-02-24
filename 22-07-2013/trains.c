/* 22-07-13 solution by E.Sorbellini */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct _station{
	pthread_mutex_t track0;
	pthread_mutex_t track1;
}Station;

typedef struct _train{
	int station;
	int track;
}Train;

Station *stations;
Train *trains;
int nStations;
int nTrains;

pthread_mutex_t *connections;

void *thFunc(void *arg);

void select_station_and_track(int *station, int *train);

int main(int argc, char *argv[]){
	
	int i;
	pthread_t *th;
	
	if(argc != 3){
		return -1;
	}
	
	nStations = atoi(argv[1]);
	nTrains = atoi(argv[2]);
	
	if(nStations < nTrains){
		return -1;
	}
	
	stations = (Station *)malloc(nStations * sizeof(Station));
	trains = (Train *)malloc(nTrains * sizeof(Train));
	connections = (pthread_mutex_t *)malloc(nStations * sizeof(pthread_mutex_t));
	th = (pthread_t *)malloc(nTrains * sizeof(pthread_t));
	
	for(i=0; i<nStations; i++){
		pthread_mutex_init(&(stations[i].track0), NULL);
		pthread_mutex_init(&(stations[i].track1), NULL);
		pthread_mutex_init(&connections[i], NULL);
	}
	
	for(i=0; i<nTrains; i++){
		//select_station_and_track(&(trains[i].station), &(trains[i].track));
		trains[i].station = i;
		trains[i].track = i % 2;
		if(trains[i].track == 0){
			pthread_mutex_lock(&(stations[trains[i].station].track0));
		}
		else{
			pthread_mutex_lock(&(stations[trains[i].station].track1));
		}
	}
	
	for(i=0; i<nTrains; i++){
		int *j = malloc(sizeof(int));
		*j = i;
		pthread_create(&th[i], NULL, thFunc, j);
	}

	pthread_exit(0);
}

void *thFunc(void *arg){
	int *iPtr = (int *)arg;
	int i = *iPtr;
	int current_station;
	int current_track;
	int next_station;
	int next_connection;
	
	while(1){
		//stay in station for a while
		sleep( 1 + (rand() % 5));
		current_station = trains[i].station;
		current_track = trains[i].track;
		//clockwise
		if(current_track == 0){
			printf("Train %d in station %d going CLOCKWISE\n", i, current_station);
			//select next station and track
			next_station = (current_station + 1);
			if(next_station >= nStations)
				next_station = 0;
			next_connection = current_station;
			//lock track
			pthread_mutex_lock(&connections[next_connection]);
			//lock next station
			pthread_mutex_lock(&(stations[next_station].track0));
			//train start
			printf("Train %d travelling toward station %d\n", i, next_station);
			sleep(10);
			//unlock current station track
			pthread_mutex_unlock(&(stations[current_station].track0));
			//unlock track
			pthread_mutex_unlock(&connections[next_connection]);
			//update station
			trains[i].station = next_station;
			printf("Train %d arrived at station station %d\n", i, next_station);
		}
		if(current_track == 1){
			printf("Train %d in station %d going COUNTERCLOCKWISE\n", i, current_station);
			//select next station and track
			next_station = (current_station - 1);
			if(next_station < 0)
				next_station = nStations - 1;
			next_connection = current_station - 1;
			if(next_connection < 0)
				next_connection = nStations - 1;
			//lock track
			pthread_mutex_lock(&(connections[next_connection]));
			//lock next station
			pthread_mutex_lock(&(stations[next_station].track1));
			//train start
			printf("Train %d travelling toward station %d\n", i, next_station);
			sleep(10);
			//unlock current station track
			pthread_mutex_unlock(&(stations[current_station].track1));
			//unlock track
			pthread_mutex_unlock(&connections[next_connection]);
			//update station
			trains[i].station = next_station;
			printf("Train %d arrived at station station %d\n", i, next_station);
		}
	}
	pthread_exit(0);
}
		
			
			
			
	

void select_station_and_track(int *station, int *train){
}
