#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>    
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void startListeningAndPrintMessagesOnNewThread(int socketFD);
void* listenAndPrint(void* args);

int main(){

    // create a socket that returns a socket file directory (same concept as reading and writing to files but now to a socket)
    // specify the address, the TCP protocol, and the IP layer as params
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);

    // constructing an adress to a server, this is the client side code
    struct sockaddr_in address;
    char* ip = "127.0.0.1";

    // port number is an usigned short (2 byte number)
    // because the network protocol expect the Big endian version of the bytes we use the htons function to put the bytes in the right order
    address.sin_port = htons(2000);
    address.sin_family = AF_INET;

    // function that will take an ipv4 addr (INET), an ipv4 address in a char* form, and the pointer to an address struct
    // will create the uint version of the ip address to use as the field in the address struct (needed to make the connection)
    inet_pton(AF_INET, ip, &address.sin_addr.s_addr);


    printf("Attempting connection to %s:80...\n", ip);

    // a non 0 result means the connection failed
    int result = connect(socketFD, (struct sockaddr*) &address, sizeof address);
    if (result == 0)
    {
        printf("Connection was sucessful!\n"); 
    } else {
        printf("connection failed");
    }
    
    char* line = NULL;
    // Creates a pointer to char, initialized to NULL
    // This will hold the address of memory where user input is stored
    // getline() will allocate memory and make 'line' point to it

    size_t lineSize = 0;
    // Creates a variable to track the size of allocated memory
    // size_t is an unsigned integer type for memory sizes
    // getline() will update this to show how much memory it allocated

    printf("Type and we will send... (type 'q' to quit)\n");
    // Prints instructions to the user on the screen


    // function that will create another thread that is listening to incoming messages and will print them
    // must be on another thread because we are blockign for sending messages.
    startListeningAndPrintMessagesOnNewThread(socketFD);

    while (true)
    // Starts an infinite loop that will run until we explicitly break out
    // 'true' means this condition is always satisfied
    {
        ssize_t charCount = getline(&line, &lineSize, stdin);   
        // getline() reads one complete line from keyboard (stdin)
        // &line: gives getline the ADDRESS of the line pointer so it can modify it
        // &lineSize: gives getline the ADDRESS of lineSize so it can update it
        // stdin: tells getline to read from keyboard input
        // Returns: number of characters read (including newline), or -1 if error
        // ssize_t: signed integer type that can hold -1 for errors
        
        if (charCount > 0){
        // Checks if getline successfully read some characters
        // If charCount is 0 or negative, something went wrong
        
            if (strcmp(line, "q\n") == 0)
            // strcmp() compares two strings character by character
            // line: the user's input (allocated by getline)
            // "q\n": the quit command (q followed by newline)
            // Returns 0 if strings are identical
            {
                break;  
                // Exits the while loop immediately
                // Program continues after the closing brace of while loop
            }
            
            ssize_t amountWasSent = send(socketFD, line, charCount, 0);
            // send() transmits data through the network socket
            // socketFD: the socket file descriptor (connection to server)
            // line: pointer to the user's input data
            // charCount: how many bytes to send (the length of user input)
            // 0: flags (no special options)
            // Returns: number of bytes actually sent, or -1 if error
        }
    }

    // only close the socket in the main function
    close(socketFD);    

    return 0; 
}

void startListeningAndPrintMessagesOnNewThread(int socketFD)
{
    pthread_t id;
    pthread_create(&id, NULL, listenAndPrint, (void*)(intptr_t) socketFD); 
 
}

void* listenAndPrint(void* args){
    // cast arg back to int
    int socketFD = (int)(intptr_t)args;

    // create a buffer to receive information from the client and print it
    char buffer[1024];
    while (true)
    {
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
            printf("Message to everyone is: %s", buffer);
        }
        
        if (amountReceived == 0)
        // Check for client closed (the close function in the client will send a tcp data with this 0 vlaue)
        {
            break;  
        }
    }   
    return NULL;
}
