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
#include<pthread.h>
#include <poll.h>


#define BACKLOG 20

int hash[BACKLOG];
char left[]="RECEIVER has Already left";
pthread_t thread_id;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//RECEIVER
void * recv_handler(void *p)
{
    pthread_detach(pthread_self());
    int sock=*(int*)p;
    int sent_bt,recv_bt;
    char buf[100];
    int i,idx,d,dest;
    pthread_mutex_lock(&mutex);
    for(i=0;i<BACKLOG;i++)
    {
        if(hash[i]==sock){idx=i;break;}
    }
    pthread_mutex_unlock(&mutex);
    sprintf(buf, "Your No. %d", idx);
    if((sent_bt=send(sock,buf,strlen(buf),0))==-1)
    {
        perror("send");
    }
    memset(buf,'\0',sizeof(buf));
    while(1)
    {
        //printf("recv4\n");
        
        if((recv_bt=recv(sock,buf,1000,0 ))==0)
        {
            //MUTEX127.0.0.1
            pthread_mutex_lock(&mutex);
            hash[idx]=-1;
            pthread_mutex_unlock(&mutex);
            //MUTEX
            printf("CLIENT[%d] disconnected\n",idx);
            close(sock);
            pthread_exit(NULL);
        }
        //printf("recv6== %d\n",recv_bt);
        //sleep(1);
        if(recv_bt==-1){perror("recv");continue;}
        d=(int)buf[0]-48;
        if((d>=0)&&(d<BACKLOG))
        {
            //printf("recv7\n");
            //MUTEX
            pthread_mutex_lock(&mutex);
            dest=hash[d];
            pthread_mutex_unlock(&mutex);
            //MUTEX
            if(dest==-1)
            {
                send(sock,left,strlen(left),0);
            }
            else
            {
                send(dest,buf+2,strlen(buf)-2,0);
            }
            memset(buf,'\0',sizeof(buf));
        }
        //printf("recv8\n");
        
    }

}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) 
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char * argv[])
{
    
    int PORT = atoi(argv[1]);
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    struct sockaddr_in servaddr;
    socklen_t sin_size;
    //struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    /*
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo("192.168.42.105", NULL, &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    p=servinfo;
    if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
    {
            perror("server: socket");
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) 
    {
            perror("setsockopt");
            exit(1);
    }
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
    {
            close(sockfd);
            perror("server: bind");
    }
    freeaddrinfo(servinfo); // all done with this structure
    */
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) 
    {
            perror("server: socket");
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) 
    {
            perror("setsockopt");
            exit(1);
    }
    memset(&servaddr,'\0',sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	//inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));
    servaddr.sin_addr.s_addr= INADDR_ANY;
    if (bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1) 
    {
            close(sockfd);
            perror("server: bind");
    }

    if (listen(sockfd, BACKLOG) == -1) 
    {
        perror("listen");
        exit(1);
    }
    //Initialize Hash by -1's
    int i,idx,arr[1],*ne;
    //memset(&hash,-1,sizeof(hash));
    for(i=0;i<BACKLOG;i++)
    {
            hash[i]=-1;
    }
    printf("server: waiting for connections...\n");
    while(1) 
    { 
        // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) 
        {
            perror("accept");
            continue;
        }
        ne=malloc(1);*ne=new_fd;
        arr[0]=new_fd;
        //MUTEX
        pthread_mutex_lock(&mutex);
        for(i=0;i<BACKLOG;i++)
        {
            if(hash[i]==-1)
            {
                hash[i]=new_fd;
                idx=i;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
        //MUTEX
        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
        printf("server: got connection from %s\n", s);
        printf("Its CLIENT[%d]\n",idx);

        pthread_create(&thread_id,NULL,recv_handler,(void *)ne);
        //close(new_fd); // parent doesn't need this
    }
    return 0;
}