#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>    
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
# include <unistd.h>

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
     while (true){
        // Start infinite loop to continuously listen for client messages
        
        ssize_t amountReceived = recv(clientSocketFD, buffer, 1024, 0);
        // recv() reads raw bytes from the client connection
        // Returns: number of bytes received, 0 if client closed, -1 if error
        // Important: recv() does NOT automatically null-terminate the data
        
        if(amountReceived > 0)
        // Check if we successfully received some data
        {
            buffer[amountReceived] = 0;
            // This adds a null terminator ('\0') after the received data
     
            
            printf("Response was %s", buffer);
        }
        
        if (amountReceived <= 0)
        // Check for network errors (connection lost, socket error, etc.)
        {
            break;  
        }
    }
    
    close(clientSocketFD);
    // Close this client's connection - frees up the file descriptor
    
    shutdown(serverSocketFD, SHUT_RDWR);
    // Gracefully shutdown the main server listening socket
    // SHUT_RDWR = shutdown both read and write operations
    // More polite than close() - gives network time to clean up
      
     printf("Response was %s\n", buffer);
}