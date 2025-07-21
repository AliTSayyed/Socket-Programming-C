#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>    
#include <arpa/inet.h>
#include <string.h>


int main(){

    // create a socket that returns a socket file directory (same concept as reading and writing to files but now to a socket)
    // specify the address, the TCP protocol, and the IP layer as params
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);

    // constructing an adress to a server, this is the client side code
    struct sockaddr_in address;
    char* ip = "74.125.136.138";

    // port number is an usigned short (2 byte number)
    // because the network protocol expect the Big endian version of the bytes we use the htons function to put the bytes in the right order
    address.sin_port = htons(80);
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

    char* message;
    message = "GET \\ HTTP/1.1\r\nHost:google.com\r\n\r\n";   
    send(socketFD, message, strlen(message), 0);

    char buffer[1024];
    recv(socketFD, buffer, 1024, 0);

    printf("Response was %s\n ", buffer);

    return 0; 
}