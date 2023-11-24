#include<stdio.h>
#include<string.h>
 //strlen
#include<sys/socket.h> //socket libery
#include<arpa/inet.h> //inet_addr
#include<unistd.h>
 //write
int main(int argc , char *argv[])
{
int socket_desc , client_sock , c , read_size;
struct sockaddr_in server , client;
char client_message[2000],message[2000];

//Create socket
socket_desc = socket(AF_INET , SOCK_STREAM , 0);
if (socket_desc == -1)
{
printf("Could not create socket");
}
puts("Socket created");

//Prepare the sockaddr_in structure
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons( 8888 );
//Bind
if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) <
0) //bind new concxion ma3a client
{
perror("Error mfcmh cnx");
return 1;
}
puts("bind done");

//ytsanet 3al resaux lin yconnctewa7ed
listen(socket_desc , 3);

//Accept incoming connection
puts("Waiting connections...");
c = sizeof(struct sockaddr_in);

//accept connection from client
client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c); ///accepte le demande

if (client_sock < 0)
{
perror(" failed mfch client ");
return 1;
}
puts("Connection accepted ");
printf("port du client : %d \n", &client.sin_port);

//Receive a message from client
while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0)
{

//Send the message back to client
puts( client_message);
printf("Enter message ");
scanf("%s" , message);
//printf ("%s",client_message);
write(client_sock ,message , strlen(client_message));
}
if(read_size == 0)
{
puts("Client disconnected ");
fflush(stdout);
}
else if(read_size == -1)
{
perror("recving failed");
}
return 0;
}