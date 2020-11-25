#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
	
#define BUFSIZE 1024


void show_database()
{

    FILE *product_file;
    product_file = fopen("./products.txt","r");
    char  line[200];
    double price;
    printf("===================================\n");
    printf("|| Code ||   Item Name   || Rate ||\n");
    printf("===================================\n");
    while(fgets(line,50,product_file)!=NULL)
    {
        char *nn;
        nn = (char *)malloc(50*sizeof(char));
        int iii = 0;
        char pp[10];
        
        int ttt=0;
        char upc_c[10];
        while(line[iii] != ',')
        {
            upc_c[ttt] = line[iii];
            ttt =ttt+1;
            iii=iii+1;
        }
        upc_c[ttt]='\0';
        ttt=0;
        iii=iii+1;  
        while(line[iii]!=','  )
        {
            pp[ttt] = line[iii];
            ttt=ttt+1;
            iii=iii+1;
        } 
        pp[ttt]='\0';
        iii=iii+1;
        ttt=0;
        while(line[iii] != '\0' && line[iii] != '\n')
        {
            nn[ttt] = line[iii];
            ttt=ttt+1;
            iii=iii+1;
        }
        nn[ttt]='\0';
        printf("|| %-6s||   %-15s|| %-6s||\n", upc_c, nn       ,pp      );
    }
}



void send_recv(int i, int sockfd)
{
	char send_buf[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;
	
	if (i == 0){
	
		
		int request_type;
		
		scanf("%d",&request_type); 
		if(request_type == 0){
	        char upccode[10];
              	char quantity[20];
            	char datatosend[25];
		
		scanf("%s",upccode);
		
		scanf("%s",quantity);
		sprintf(datatosend,"0,%s,%s",upccode,quantity);
		int temp = strlen(upccode)+ strlen(quantity)+3;
		datatosend[temp] = '\0';
		
		
		send(sockfd, datatosend, temp, 0); 
		
		struct sockaddr_in addr;
	 socklen_t length =  sizeof( struct   sockaddr_in); 
	 getpeername( sockfd, (struct sockaddr*)&addr, &length);
	 
	  printf("%s", inet_ntoa(addr.sin_addr));
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		printf("\nRESPONSE FROM SERVER : %s\n",recv_buf); 
		}
		else{
			char datatosend[25];
            		sprintf(datatosend,"1");   
            		datatosend[1]='\0';
            		send(sockfd, datatosend, 2, 0);
			nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
			recv_buf[nbyte_recvd] = '\0';
			printf("\nRESPONSE FROM SERVER : %s\n", recv_buf ); 
			printf("\n%s\n\n","***Your connection will be closed now****" );
             close(sockfd);
		}
	}
}
		
		
void connect_request(int *sockfd, struct sockaddr_in *server_addr, char* IP)
{
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(4950);
	server_addr->sin_addr.s_addr = inet_addr(IP); 
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);
	
	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}
}
	
int main(int argc, char *argv[])
{	
	char * IP = argv[1];
	int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;
	
	connect_request(&sockfd, &server_addr ,  IP);
	FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
	fdmax = sockfd;
	show_database();  
	puts("\n\tEnter '0' for purchase \n\tEnter '1' for closing connection \n\tChoose :\n");
	
	puts("\tEnter upccode of the item from the above table :");
	puts("\tEnter quantity of the item :");
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		
		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
				send_recv(i, sockfd);
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}
