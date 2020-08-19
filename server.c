//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <sys/time.h> 
#include <signal.h>
#include <stdlib.h> 
#include <errno.h> 
#include <sys/socket.h> 
#include <ctype.h>
#include <arpa/inet.h>    
#include <sys/types.h> 
#include <string.h>   
#include <unistd.h>   
#include <netinet/in.h> 



#define max_client 10 
// #define PORT 8888 



int addrlen , new_socket;
int i;
int maximum_clients = max_client;
int master_socket;
int client_socket[10];      
int PORT;
int opt = 1;  
double amount_total[10];

//FUnction to compare 2 strings
int compare_strings(char str1[], char str2[])
{
    int c = 0;
    
    while (str1[c] == str2[c]) 
    {

        if (str1[c]=='\0')
            break;
        if(str2[c]=='\0')
            break;
        c = c+1;
    }
    
    if (str1[c]=='\0'&&str2[c]=='\0')
        return 1;
    else
        return  0 ;
}

//Get item name from products whode product id is given
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
        if(compare_strings(upc_c,product_id)==1)
        {
            return nn;
        }
    }
    return NULL;
}



//concat 2 strings
char* string_concat(char* str1, const char* str2)
{ 
    char* ptr = str1 + strlen(str1);
    while (*str2 != '\0')
        *ptr++ = *str2++;
    *ptr = '\0';
    return str1;
}





void Handler(int sig_num)
{
    signal(SIGINT, Handler);
    for (int i = 0; i < maximum_clients;  i++ )
    { 
       if(client_socket[i]!=0)
       {
        printf("Closing the socket :%d\n",  client_socket[i]);
        close(client_socket[i]) ;
        amount_total[i] = 0.0;
    }
}
close(master_socket);
}
//get item price from products whose product id is given
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
        
        if(compare_strings(upc_c,product_id)==1)
        {
            return price;
        }
    }
    return -1;    
}



int main(int argc , char *argv[])  
{ 
    struct sockaddr_in server_address;  

    //The arguments should be equal to 2
    if(argc > 2 || argc <2) 
    {
        printf("Number of arguments entered by the user is not 2\n");
        printf("Server is exiting now as this is a mistake from user\n");
        exit(0);
    }
    //port number of socket
    if(argv[1])
    {
        char* port = argv[1];
        sscanf(port,"%d",&PORT);
    }
    //default port number
    else
    {
        PORT = 8080;
    }
    signal(SIGINT, Handler);


    int  activity,   valread , socket_des , max_sd;  
    char buffer[1025];  //creating a buffer of 1KB     
    master_socket = socket(AF_INET , SOCK_STREAM , 0);//creating a master socket 

    if(master_socket== 0)  
    {  
        perror("The SOCKET has failed try again");  
        exit(EXIT_FAILURE);  
    }   
    
    fd_set readfds;  //set of socket descriptors

    char message[150] ;  //a message 

    //We have to initialize all client socket to 0
    for (i = 0; i < maximum_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
    
    
    //set master socket to allow multiple connections
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    
    //Creating the socket
    server_address.sin_family = AF_INET;  
    server_address.sin_addr.s_addr = INADDR_ANY;  
    server_address.sin_port = htons( PORT );  
    
    // Check if the socket is binded or not
    if (bind(master_socket, (struct sockaddr *)&server_address, sizeof(server_address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
    //Listning to the clients
    if (listen(master_socket, maximum_clients) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
    
    //accept the incoming connection 
    addrlen = sizeof(server_address);  
    puts("Waiting for connections ...");  
    
    //Running the loop till wait for any client to close the connection
    while(1)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
        
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
        
        //add child sockets to set 
        i = 0;
        while(i < maximum_clients)
        {  
            //descriptor of socket
            socket_des = client_socket[i];  
            
            //If it is valid socket descripter then add it to socket set
            if(socket_des > 0)  
                FD_SET( socket_des, &readfds);  
            
            //highest file descriptor number, need it for the select function 
            if(socket_des > max_sd)  
                max_sd = socket_des;  
            i++;
        }  
        //wait for an activity on one of the sockets , timeout is NULL
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
        //If the activity is less than 0 then there is error
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
        
        char received_data[25],ufccode[4],qty[10];
        int type,quantity=0    ;
        double price = 0.0;
        char * name;
        name = (char *)malloc(100*sizeof(char));
        if (FD_ISSET(master_socket, &readfds))  
        {  
            //Filling 0's in received_data
            for(int iii=0;iii<sizeof(received_data);iii++)
            received_data[iii]=0;
            //If socket is not accepted then exit
        if ((new_socket = accept(master_socket, (struct sockaddr *)&server_address, (socklen_t*)&addrlen))<0)  
        {  
            perror("accept");  
            exit(EXIT_FAILURE);  
        }   
            //Reading from the client
        int read_length = read(new_socket,received_data,25);
        printf("New connection is established , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(server_address.sin_addr) , ntohs(server_address.sin_port));  
            // If unable to read anything then the host has disconnected
        if(read_length == 0      ) 
        {
            printf("Host disconnected , ip %s , port %d \n" ,  
                inet_ntoa(server_address.sin_addr) , ntohs(    server_address.sin_port     )); 
            close( new_socket      );
            continue;
        }
            //For sanity check of quantity
        int valid_quantity = 1;
        int iii = 0;
        sscanf(received_data,"%d,%[^,],%s",&type,ufccode,qty);
        sscanf(qty,"%d",&quantity);
            // checking if qty is a valid number or not
        for ( iii = 0; qty[iii]!='\0'; ++iii)
        {
            if(!isdigit(qty[iii]))
            {
               valid_quantity = 0;
               break;
           }
       }
       printf("\t\tData sent by client is : %s\n",received_data);   
       printf("Your First request is %d,%s,%d\n",type,ufccode,quantity);
            //Sanity Checks
       if(type == 0)
       {
        price = get_item_price(ufccode);
        name = get_item_name(ufccode);
        if(quantity<0)
        {
            strcpy(message, "1,The quantity cannot be negative\n");
        }
        if(valid_quantity==0)
        {
            strcpy(message, "1,The quantity should be of int type\n");
        }
        
        if(price>=0 && name!=NULL && quantity >=0 && valid_quantity==1)
        { 
         for(int iii=0;iii<sizeof(message);iii++)
            message[iii]=0;
        strcpy(message, "0,");
        char pricetemp[10];
        snprintf (pricetemp, sizeof(pricetemp), "%0.2f",price);
        printf("price of current upc code %s\n",pricetemp);   
        string_concat(message,pricetemp);
        printf("name of current upc code %s\n",name);   
        string_concat(message,",");
        string_concat(message,name);
        printf("\t\tmessage to send : %s\n",message);
    }
    else
    {
        strcpy(message, "1,The upc code cannot be found in the database\n");
    }
    


    if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
    {  
        perror("send");  
    }  
    for (i = 0; i < maximum_clients; i++)  
    {  
                    //if position is empty 
        if( client_socket[i] == 0 )  
        {  
            client_socket[i] = new_socket;
            if( price!=-1 && quantity>=0 )  
            {
               amount_total[i] += price*quantity;
            }
            printf("Adding to list of sockets as  %d and initialise total amt as %0.2f\n" , i,amount_total[i]);  
            break;  
       }  
   }  
}
else if(type == 1)
{

    for(int iii=0;iii<sizeof(message);iii++)
        message[iii]=0;
    strcpy(message, "0,0");
    printf("\t\tmessage to send %s\n",message );
    if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
    {  
        perror("send");  
    }  
}
puts("Response sent to client successfully");  
}  
        //else its some IO operation on some other socket
for (i = 0; i < maximum_clients; i++)  
{  
    socket_des= client_socket[i];  
    
    if (FD_ISSET(socket_des , &readfds))  
    {  

        for(int iii=0;iii<sizeof(buffer);iii++)
            buffer[iii]=0;
        valread = read(socket_des,buffer,1024);
        if (valread == 0)  
        {  
                    //Somebody disconnected , get his details and print 
            getpeername(socket_des, (struct sockaddr*)&server_address ,(socklen_t*)&addrlen);  
            printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(server_address.sin_addr) , ntohs(server_address.sin_port));  
            close(socket_des);  
            client_socket[i] = 0;
            amount_total[i]=0;

        }                      
        else
        {  
            buffer[valread] = '\0';  
            printf("\t\tData from client : %s\n",buffer); 
            sscanf(buffer,"%d,%[^,],%s",&type,ufccode,qty);
            sscanf(qty,"%d",&quantity);
            int valid_quantity = 1;
            int iii = 0;
                    // Sanity check checking if qty is a number or
            for ( iii = 0; qty[iii]!='\0'; ++iii)
            {
               if(!isdigit(qty[iii]))
               {
                   valid_quantity = 0;
                   break;
               }
           }

                      
           if(type == 0)
           {
             name = get_item_name(ufccode);
             price = get_item_price(ufccode);
             if(price >=0 && name!=NULL && quantity>=0 && valid_quantity)
             {
                 for(int iii=0;iii<sizeof(message);iii++)
                     message[iii]=0;
                 strcpy(message,"0,");
                 printf("price: %0.2f\n",price);
                 char pricetemp[10]; 
                 snprintf (pricetemp, sizeof(pricetemp), "%0.2f",price);
                 printf("price of current upc code : %s\n",pricetemp);   
                 string_concat(message,pricetemp);
                 printf("name of current upc code %s\n",name);   
                 string_concat(message,",");
                 string_concat(message,name);
                 printf("\t\tmessage to send : %s\n",message);
                 amount_total[i] = amount_total[i] + price*quantity;
                 printf("AMOUNT_TOTAL AS OF NOW IS %0.2f\n",amount_total[i]);
             }
             else
             {
                 if(quantity<0)
                 {
                     strcpy(message, "1,The quantity cannot be negative\n");
                 }
                 else if(valid_quantity==0)
                 {
                     strcpy(message, "1,The quantity should be of int type\n");
                 }
                 else
                 {
                     strcpy(message, "1,The upc code cannot be found in the database\n");
                 }
             }
             if( send(socket_des, message, strlen(message), 0) != strlen(message) )  
             {  
               perror("send");  
           }  
           message[0] = '\0';
       }
       //Type is 1 so print the final price
       else if(type == 1)
       {
           char totalamttemp[10]; 
           snprintf (totalamttemp, sizeof(totalamttemp), "%0.2f",amount_total[i]);
           for(int iii=0;iii<sizeof(message);iii++)
               message[iii]=0;
           strcpy(message, "0,");
           string_concat(message,totalamttemp);
           printf("\t\tmessage to send %s\n",message );
           if( send(socket_des,  message, strlen(message), 0) != strlen(message) )  
           {  
               perror("send");  
           }  
       }
       puts("Response sent to client successfully");  
   }  
}  
}  
}  

return 0;  
}  
