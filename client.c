#include <stdio.h> 
#include <netinet/in.h> 
#include <sys/time.h>  
#include <unistd.h>   
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h> 
#include <arpa/inet.h>       
#include <stdlib.h> 
#include <sys/types.h> 

// For displaying the database
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


int main(int argc, char *argv[])
{
    char * IP = argv[1];
    char * p = argv[2];
    int port;
    sscanf(p,"%d",&port);
    printf("The ip address is : %s \n",IP );
    printf("The port number is this: %d\n",port);
    struct sockaddr_in address;
    int valread,client_socket;
    struct sockaddr_in serv_addr;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[1024] = {0};
    //Check if the socket can be created or not
    if (client_socket == -1)
    {
        printf("\n Socket cannot be created error \n");
        return -1;
    }
    printf("Socket Descriptor: %d\n",client_socket); 
    memset(&serv_addr, '0', sizeof(serv_addr));
    //initializing the socket
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(port); 
    // Sanity Check for IP address
    if(inet_pton(AF_INET, IP, &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address \n");
        return -1;
    }
    if(connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    else 
    {
        printf("Connected to the server..you may now proceed\n");
    }
    show_database();  
    while(1)
    {
        int request_type;
        printf("\n\tEnter '0' for purchase \n\tEnter '1' for closing connection \n\n\tChoose :");
        scanf("%d",&request_type);
        if(request_type == 0)
        {
            char upccode[10];
            char quantity[20];
            char datatosend[25];
            printf("\tEnter upccode of the item from the above table :");
            scanf("%s",upccode);
            printf("\tEnter quantity of the item :");
            scanf("%s",quantity);
            sprintf(datatosend,"0,%s,%s",upccode,quantity);
            int temp = strlen(upccode)+ strlen(quantity)+3;
            datatosend[temp] = '\0';
            printf("\n\tRequest we will be sending is %s\n",datatosend);
            if( send(client_socket , datatosend, temp+1, 0) != temp+1)  
            {  
                perror("\terror in send in client");  
            } 
            printf("\n\tRequest of 0 request_type sent\n"); 
            for(int iii=0;iii<sizeof (buffer);iii++)
            {
                buffer[iii] = 0;
            }   
            valread = read( client_socket , buffer,sizeof(buffer));       
            if (valread ==0)
            {
                printf("\terror in client while reading response from server\n");
            }
            printf("\nRESPONSE FROM SERVER : %s\n",buffer );
            buffer[0]='\0';
        }
        else if(request_type == 1)
        {
            char datatosend[25];
            sprintf(datatosend,"1");   
            datatosend[1]='\0';
            printf("\n\tRequest we will be sending is %s\n",datatosend);
            if( send(client_socket , datatosend, 2, 0) != 2 )  
            {  
                perror("\terror in send in client");  
            } 
            printf("\n\tRequest of 1 request_type sent\n");
            
            for(int iii=0;iii<sizeof (buffer);iii++)
            {
                buffer[iii] = 0;
            } 
            valread = read( client_socket , buffer, sizeof(buffer));            
            if (valread ==0)
            {
                printf("\terror in client while reading response from server\n");
            }
            printf("\nRESPONSE FROM SERVER : %s\n",buffer);
            buffer[0]='\0';
            printf("\n%s\n\n","***Your connection will be closed now****" );
            close(client_socket);
            break;
        }

    }
    return 0;
}
