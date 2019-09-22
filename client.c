#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <poll.h>



#define MAXDATASIZE 1000 
void *sender(void *ptr)
{
	int sent_bt;
	int sfd=*(int *)ptr;
	//puts("s\n");
	char buf[1000];
	//printf("enter msg::");
	while(1)
	{
		memset(&buf,'\0',sizeof(buf));
		
 		gets(buf);
 		//printf("sentt----\n");
		if((sent_bt=send(sfd,buf,strlen(buf),0))==-1)
		{
			perror("send");
		}
		if(strcmp(buf+2,"exit")==0)
		{
			close(sfd);
			exit(3);
		}
 		memset(&buf,'\0',sizeof(buf));
 		//printf("enter msg::");
 	}
}

void *receiver(void *p)
{
	
	int recv_bt;
	//puts("ssss");
	int sfd=*(int *)p;
	char buf[1000];
	memset(&buf,'\0',sizeof(buf));
	while((recv_bt=recv(sfd,buf,1000,0 )))
 	{
 		printf(":::%s\n",buf);
 		memset(&buf,'\0',sizeof(buf));
 	}
	if(recv_bt==0)
	{
		close(sfd);
		printf("Server left\n");
		exit(1);
	}

}


int main(int argc, char *argv[])
{
    int PORT = atoi(argv[1]);
    int sockfd, numbytes,arr[1];
    char buf[MAXDATASIZE];
    pthread_t tid1,tid2;
	struct sockaddr_in servaddr;
    memset(&buf,'\0',sizeof(buf));
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) 
    {
            perror("client: socket");
			exit(1);
    }
	printf("client socket created\n");
	memset(&servaddr,'\0',sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	inet_pton(AF_INET,argv[2],&(servaddr.sin_addr));
	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))==-1)
 	{
 		close(sockfd);
 		printf("error in connection\n");
		perror("client: connect");
 		exit(1);
 	}
 	printf("connected to server---\n");
	int *ne;
	ne=malloc(1);*ne=sockfd;
 	arr[0]=sockfd;
    pthread_create(&tid1,NULL,sender,(void *)ne); 		
	pthread_create(&tid2,NULL,receiver,(void *)ne);
 	//pthread_join(tid1,0);
	pthread_join(tid2,0);
 	
    return 0;
}
