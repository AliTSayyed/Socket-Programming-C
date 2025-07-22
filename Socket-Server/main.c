#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>    
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

struct AcceptedSocket 
{
 int accpetedSocketFD;
 struct sockaddr_in address;
 int error;
 bool accpetedSuccessfully;   
};

struct AcceptedSocket acceptedSockets[10];
int acceptedSocketsCount = 0;

struct AcceptedSocket* acceptIncomingConnection(int serverSocketFD);  
void* receiveAndPrintIncomingData(void* socketFD);
void startAcceptingIncomingConnections(int serverSocketFD);
void receiveAndPrintIncomingDataOnSeperateThread(struct AcceptedSocket* pSocket);
void sendAndReceivedMessageToOtherClients(char* buffer, struct AcceptedSocket* pSocket);

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

     // wrapper function that will create a new thread per client connection 
     startAcceptingIncomingConnections(serverSocketFD);
    
     shutdown(serverSocketFD, SHUT_RDWR);
     // Gracefully shutdown the main server listening socket
     // SHUT_RDWR = shutdown both read and write operations
     // More polite than close() - gives network time to clean up
    
     return 0;
}

// Accepts and creates a new client socket file descriptor for incoming connections
void startAcceptingIncomingConnections(int serverSocketFD)
{
    // this while true loop keeps the function 'alive' in the stack
    while (true) 
     {
         struct AcceptedSocket* clientSocketFD = acceptIncomingConnection(serverSocketFD);
         // store the accepted client into the array (max 10 connections)
         acceptedSockets[acceptedSocketsCount++] = *clientSocketFD;
         receiveAndPrintIncomingDataOnSeperateThread(clientSocketFD);

     }
     
}

// we must create a new thread for each client to print the data from their client socket fd
void receiveAndPrintIncomingDataOnSeperateThread(struct AcceptedSocket* pSocket)
{
    pthread_t id;
    // function to create a new thread (must type cast void*)
    pthread_create(&id, NULL, receiveAndPrintIncomingData,(void*)pSocket);
}

void* receiveAndPrintIncomingData(void* arg)
{
     // Cast back to get the original struct pointer
     struct AcceptedSocket* pSocket = (struct AcceptedSocket*)arg;
     int socketFD = pSocket->accpetedSocketFD;

    // create a buffer to receive information from the client and print it
    char buffer[1024];
    while (true){
        // Start infinite loop to continuously listen for client messages
        
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);
        // recv() reads raw bytes from the client connection
        // Returns: number of bytes received, 0 if client closed, -1 if error
        // Important: recv() does NOT automatically null-terminate the data
        
        if(amountReceived > 0)
        // Check if we successfully received some data
        {
            buffer[amountReceived] = 0;
            // This adds a null terminator ('\0') after the received data
            printf("Server Response was: %s", buffer);

            sendAndReceivedMessageToOtherClients(buffer, pSocket);
        }
        
        if (amountReceived == 0)
        // Check for client closed (the close function in the client will send a tcp data with this 0 vlaue)
        {
            break;  
        }
      }   
     // close the connection once client disconnects
     close(socketFD);
     // free the accepted client struct
     free(pSocket);
     return NULL;
}

// send the received message from one client to all other clients 
// we send to clients that have a different file descriptor number than the one received
void sendAndReceivedMessageToOtherClients(char* buffer, struct AcceptedSocket* pSocket)
{
    for(int i = 0; i < acceptedSocketsCount; i++)
    {
        if (acceptedSockets[i].accpetedSocketFD != pSocket->accpetedSocketFD)
        {
            send(acceptedSockets[i].accpetedSocketFD, buffer, strlen(buffer), 0); 
        }
    }
}

// since we are going to accept multiple clients to the same socket, wrap the accept function to make it cleaner and add extra functionality
struct AcceptedSocket* acceptIncomingConnection(int serverSocketFD)
{
     struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(struct sockaddr_in);   
     int clientSocketFD = accept(serverSocketFD, (struct sockaddr*) &clientAddress, &clientAddressSize); 

     struct AcceptedSocket* acceptedSocket = malloc(sizeof(struct AcceptedSocket));

     acceptedSocket->address = clientAddress;
     acceptedSocket->accpetedSocketFD = clientSocketFD;
     acceptedSocket->accpetedSuccessfully = clientSocketFD > 0;

     if (!acceptedSocket->accpetedSuccessfully)
     {
        acceptedSocket->error = clientSocketFD;
     }
     return acceptedSocket;

}