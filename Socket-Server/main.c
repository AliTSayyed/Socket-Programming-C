#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>    
#include <arpa/inet.h>
#include <string.h>


int main(){

    // create a socket that returns a socket file directory (same concept as reading and writing to files but now to a socket)
    // specify the serverAddress, the TCP protocol, and the IP layer as params
    // A file descriptor is always tied to a specific process that the operating system is running. 
    int serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);

    // constructing an serverAddress for the server 
    struct sockaddr_in serverAddress;
    char* ip = INADDR_ANY;

    // using port 2000
    serverAddress.sin_port = htons(2000);
    serverAddress.sin_family = AF_INET;

    // function that will take an ipv4 addr (INET), an ipv4 serverAddress in a char* form, and the pointer to an serverAddress struct
    // will create the uint version of the ip serverAddress to use as the field in the serverAddress struct (needed to make the connection)
    inet_pton(AF_INET, ip, &serverAddress.sin_addr.s_addr);

    // bind this program to the serverAddress
     int result = bind(serverSocketFD, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
     if (result == 0)   
     {
        printf("socket was bound successfully!\n");
     }

     // function that "listens to incoming requests" and 10 is the total "queue" of requests it can listen to before accepting them all at once (backlog).
     int listenResult = listen(serverSocketFD, 10);   
  
     // the accept function must have the client address (which we initilize below) and the pointer to the size of the client address
     struct sockaddr_in clientAddress;     
     int clientAddressSize = sizeof (struct sockaddr_in) ;

     // returns the file descriptor of the connecting client, does this for each connecting socket. 
     int clientSocketFD = accept(serverSocketFD, &clientAddress, &clientAddressSize); 
}