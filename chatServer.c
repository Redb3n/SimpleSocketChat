#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>

int main(){
	int dss,dsc,nbDs,maxDs,i,j;
    struct sockaddr_in aSrv,aClt;
    socklen_t lgaClt = sizeof(aClt);
    int lstSocket[20];
	char* msg = calloc(1024,sizeof(char));
	char* sent = calloc(1024,sizeof(char));
    fd_set lstDs;
    aSrv.sin_family = AF_INET;
    aSrv.sin_port = htons(60);
    inet_aton("127.0.0.1",&(aSrv.sin_addr));
    memset(&(aSrv.sin_zero),'0',8);
    dss = socket(AF_INET,SOCK_STREAM,6); 
	if(bind(dss,(struct sockaddr*) &aSrv,sizeof(struct sockaddr)) != -1){
		if(listen(dss,20) !=-1){
            lstSocket[0]=dss;
            nbDs = 1;
            maxDs = dss;
			while(1){
                FD_ZERO(&lstDs);
                for(i=0;i<nbDs;i++){
                    FD_SET(lstSocket[i],&lstDs);
                    maxDs = lstSocket[i] > maxDs ? lstSocket[i] : maxDs;
                }
                select(maxDs+1,&lstDs,NULL,NULL,NULL);
                if(FD_ISSET(dss,&lstDs)){
                    dsc = accept(dss,(struct sockaddr *) &aClt, (socklen_t *) &lgaClt);
                    printf("== Client %d is connected ==\n",dsc);
                    maxDs = dsc > maxDs ? dsc : maxDs;
                    FD_SET(dsc,&lstDs);
                    lstSocket[nbDs++] = dsc;
                }else{
                    for(i=0;i<nbDs;i++){
                        if(FD_ISSET(lstSocket[i],&lstDs)){
                            if(recv(lstSocket[i],msg,1024,0) <= 0){
                                printf("== Client %d is disconnected ==\n",lstSocket[i]);
                                for(j=i;j<nbDs;j++) lstSocket[j] = lstSocket[j+1];
                                nbDs--;
                            }else{
                                printf("Client %d : %s\n",lstSocket[i],msg);
                                for(j=0;j<nbDs;j++){
                                    if(lstSocket[j] != dss && lstSocket[j] != lstSocket[i] )
                                        send(lstSocket[j],msg,strlen(msg),0);
                                }
                            }
                        }
                    }
                }
			}
		}else printf("\nCan't listen");
	}else printf("Can't bind\n");
	printf("Closing program ...\n");
    	return 0;
}