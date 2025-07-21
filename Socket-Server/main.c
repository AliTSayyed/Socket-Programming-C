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

    // using port 2000, and all available network interface using INADDR_ANY (local host, wifi, ethernet, etc.)
    serverAddress.sin_port = htons(2000);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // bind this file descripter to the serverAddress (operating system will now know that this file descriptor is a server)
     int result = bind(serverSocketFD, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
     if (result == 0)   
     {
        printf("socket was bound successfully!\n");
     }

     // function that "listens to incoming requests" and 10 is the total "queue" of requests it can listen to before accepting them all at once (backlog).
     int listenResult = listen(serverSocketFD, 10);   
  
     // the accept function must have the client address (which we initilize below) and the pointer to the size of the client address
     struct sockaddr_in clientAddress;     
     socklen_t clientAddressSize = sizeof (struct sockaddr_in);

     // returns the file descriptor of the connecting client, does this for each connecting socket. 
     int clientSocketFD = accept(serverSocketFD, (struct sockaddr*) &clientAddress, &clientAddressSize); 

     // create a buffer to recieve information from the client and print it
     char buffer[1024];
     recv(clientSocketFD, buffer, 1024, 0);
      
     printf("Response was %s\n", buffer);
}