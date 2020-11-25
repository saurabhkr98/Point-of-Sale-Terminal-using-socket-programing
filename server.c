#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
	
#define PORT 4950
#define BUFSIZE 1024
double amount[100];
char * get_item_name(char*  );
double get_item_price(char*  ) ;
void send_to_all(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master, struct sockaddr_in *client_addr)
{       
	if(recv_buf[0] ==  '1'){
		 char send_res[30];
	sprintf(send_res, "Total Amt = %f", amount[i]);
	send_res[30]='\0';
	send(j, send_res, strlen(send_res), 0);
	amount[i] = 0.0;
	return;
	}
	char *prod_name, prod_id[10];
	char * token = strtok(recv_buf, ",");
	token = strtok(NULL, ",");
	sprintf(prod_id,"%s",token);
	token = strtok(NULL, ",");
	int quan = atoi(token);
	double price = get_item_price(prod_id);
	
	
	prod_name=get_item_name(prod_id);
	amount[i] = amount[i] + 1.0*price*quan;
	 
	 
	 
	 struct sockaddr_in addr;
         socklen_t length = sizeof( struct sockaddr_in);
         getpeername(i, (struct sockaddr *)&addr, &length)  ;
     
	 
	 
	 
	printf("%i client purchased %d quantity of %s, Current Amount = %f\n", ntohs(addr.sin_port),  quan, prod_name, amount[i]);
	 
	 char send_res[30];
	sprintf(send_res, "Current Amt = %f", amount[i]);
	send_res[30]='\0';
	send(j, send_res, strlen(send_res), 0);
 
}
		
void send_recv(int i, fd_set *master, int sockfd, int fdmax, struct sockaddr_in *client_addr)
{
	int nbytes_recvd, j;
	char recv_buf[BUFSIZE], buf[BUFSIZE];
	
	if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
	        
		if (nbytes_recvd == 0) {
			printf("socket %d hung up\n", i);
		}else {
			perror("recv");
		}
		close(i);
		FD_CLR(i, master);
	}else {               
	        //printf("%s\n", recv_buf);
			send_to_all(i, i, sockfd, nbytes_recvd, recv_buf, master, client_addr);
		
	}
		
}
		
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
	socklen_t addrlen;
	int newsockfd; 
	addrlen = sizeof(struct sockaddr_in);
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
		perror("accept");
		exit(1);
	}else {
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax){
			*fdmax = newsockfd;
		}
		printf("%d", newsockfd);
		printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
	}
}
	
void connect_request(int *sockfd, struct sockaddr_in *my_addr)
{
	int yes = 1;
		
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
		
	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(4950);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, '\0', sizeof my_addr->sin_zero);
		
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
		
	if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	if (listen(*sockfd, 10) == -1) {
		perror("listen");
		exit(1);
	}
	printf("\nTCPServer Waiting for client on port 4950\n");
	fflush(stdout);
}
int main()
{	
	memset(amount, 0, sizeof(amount));
	fd_set master;
	fd_set read_fds;
	int fdmax, i;
	int sockfd= 0;
	struct sockaddr_in my_addr, client_addr;
	
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	connect_request(&sockfd, &my_addr); 
	
	
	
	
	
	
	 
	FD_SET(sockfd, &master);
	
	fdmax = sockfd;
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		
		if(FD_ISSET(sockfd, &read_fds)){
			connection_accept(&master, &fdmax, sockfd, &client_addr); }
		 
		for (i = 0; i <= fdmax; i++){
			if (i!=sockfd && FD_ISSET(i, &read_fds)){
				send_recv(i, &master, sockfd, fdmax, &client_addr);
			}
		}
	}
	return 0;
}





double get_item_price(char* product_id)
{
    FILE *products;
    products = fopen("./products.txt","r");
    char UPC_code[4],line[200];
    int price;
    while(fgets(line,100,products)!=NULL)
    {
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
        int price = 0;
        for(int ii=0; pp[ii ]!='\0';ii++)
        {
            price = price*10 + (pp[ii]-'0');
        } 
        
        if(  strcmp(upc_c,product_id)==1)
        {
            return price;
        }
    }
    return -1;    
}


char * get_item_name(char* product_id)
{
    FILE *products;
    products = fopen("./products.txt","r");
    char UPC_code[4],line[200];
    char *nn;
    nn = (char *)malloc(100*sizeof(char));
    
    int price;
    while(fgets(line,100,products)!=NULL)
    {


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
        if(strcmp(upc_c,product_id)==1)
        {
            return nn;
        }
    }
    return NULL;
}
