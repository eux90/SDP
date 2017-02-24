/* 29-06-11 solution by E.Sorbellini */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#define N 20

void *myTh(void *arg);
char dirname[256];
pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]){
	
	pthread_t *th;
	int n,i,fd;
	char fname[256];
	
	if(argc != 3){
		return -1;
	}
	//number of threads
	n = atoi(argv[1]);
	//set directory passed in command line as working directory
	chdir(argv[2]);
	//save directory passed in command line
	sprintf(dirname, "%s", argv[2]);
	//make temp directory
	mkdir("tmp", 0777);
	
	th = (pthread_t *)malloc(sizeof(pthread_t) * n);
	
	//generate N files each one has it's name written inside
	for(i=0; i<N; i++){
		sprintf(fname, "file%d", i);
		fd = open(fname, O_CREAT | O_RDWR | O_TRUNC, 0777);
		write(fd, fname, strlen(fname) * sizeof(char));
		close(fd);
	}
	
	
	for(i=0; i<n; i++){
		int *j = (int *)malloc(sizeof(int));
		*j = i;
		pthread_create(&th[i], NULL, myTh, (void *)j);
	}
	
	
	pthread_exit(0);

}

void *myTh(void *arg){
	
	pthread_detach(pthread_self());
	
	int *n = (int *) arg;
	int count =0;
	int fd[2];
	
	//to store the 2 filename
	char fname[2][256];
	//the name of cat file
	char catName[256];
	//path + name temp file
	char tmpFileName[256];
	//buffer for content of file 1 and 2
	char *buf[2];
	//sizes of 2 buffers
	int bufSize[2];
	
	struct stat stat_buf;
	DIR *dp;
	struct dirent *dirp;
	
	while(1){
	
		pthread_mutex_lock(&mutex);
		dp = opendir(".");
		
		//read 2 file entries in dir inode if there are 2, else read less
		while((dirp = readdir(dp)) != NULL && count < 2){
			
			//get stat info of file (useful to know if is a file or a dir and for file size
			lstat(dirp->d_name, &stat_buf);
			
			//switch action if is a dir go ahead, if is a file store it's name
			switch(stat_buf.st_mode & S_IFMT){
				case S_IFDIR:
				break;
				case S_IFREG:
				sprintf(fname[count], "%s", dirp->d_name);

				bufSize[count] = stat_buf.st_size;
				buf[count] = (char *)malloc(bufSize[count] * sizeof(char));
				fd[count] = open(fname[count], O_RDONLY, 0777);
				//read(fd, buf[count], bufSize[count]);
				//close(fd);
				//unlink(fname[count]);
				//printf("%s\n", fname[count]);
				count++; 
				break;
			}
			
		}
		closedir(dp);
		
		//if found less than 2 files unlock mutex and terminate
		if(count < 2){
			count = 0;
			free(buf[count]);
			pthread_mutex_unlock(&mutex);
			break;
		}
		
		//if there were 2 files first unlink files so other threads can't see them then unlock mutex
		//to allow other thread to search for their files and start elaboration of temp file
		if(count == 2){
			unlink(fname[0]);
			unlink(fname[1]);
			pthread_mutex_unlock(&mutex);
			
			//read data in files
			read(fd[0], buf[0], bufSize[0]);
			read(fd[1], buf[1], bufSize[1]);
			close(fd[0]);
			close(fd[1]);
			
			//generate cat name
			sprintf(catName, "%s%s", fname[0], fname[1]);
			sprintf(tmpFileName, "tmp/%s", catName);
			printf("Thread %d conacatenate %s %s\n", *n, fname[0], fname[1]);
			
			//create a temp file with cat name
			fd[0] = open(tmpFileName, O_CREAT | O_TRUNC | O_RDWR, 0777);
			//write data inside temp file
			write(fd[0], buf[0], bufSize[0]);
			write(fd[0], buf[1], bufSize[1]);
			close(fd[0]);
			free(buf[0]);
			free(buf[1]);
			
			//makes hard link of temp file in main folder
			link(tmpFileName, catName);
			//remove temp file from temp folder
			unlink(tmpFileName);
			//reset file counter for next iteration
			count = 0;
		}
		
		
	}
			
	
	return 0;
	
}
