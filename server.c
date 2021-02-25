#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

#include <sys/types.h>
#include <netdb.h>

// #include <conio.h> // for reading csv files

#define PORT 30000
#define size_of_AAPL 503

struct date {
    char date_str[10];
    int year;
    int month;
    int day;
}

struct stock {
    int price;
}

int main(int argc, char const *argv[]) // ./server AAPL.csv TWTR.csv 30000
{ 
    printf("argv[1]:%s\n", argv[1]); //AAPL.CSV
    printf("argv[2]:%s\n", argv[2]); //TWTR.csv
    printf("argv[3]:%s\n", argv[3]); //30000
    /*TODO:
        1.) read from apple and twitter files; 7/2/2018 - 6/30/2020
            -> interested 1st and 5th column
            -> gaps in data
            -> use "date" and "stock" structs to collect the data (array of structs for date and another for stock)
        2.) start listening to client reuqests
    */

    printf("server started\n");

    // read from the csv files
    FILE *filePointer; // for AAPL, need to make another pointer for TWTR
    
    filePointer = fopen(argv[1], "r");
    if (filePointer == NULL) {
        printf("Failed to open %s\n", argv[1]);
    }
    else {
        printf("File opened: %s\n", argv[1]);
    }

    char data_to_read[size_of_AAPL];
    int row = 0;
    int col = 0;
    
    while (fgets(data_to_read, size_of_AAPL, filePointer) != NULL) {
        col = 0;
        row++;

        if (row == 1) { // avoid reading the data headers
            continue;
        }

        char *value = strtok(data_to_read, ", ");
        while (value) {
            if (col == 0) {
                printf("col 1: %s", value); // date 
            }

            if (col == 4) {
                printf("col 5 %s", value);  // price
            }
            value = strtok(NULL, ", ");
            col++;
        }
        // printf("%s", data_to_read);
        printf("\n");
    }

    fclose(filePointer);
    // printf("End of file reading");

    // START OF SOCKET TUTORIAL
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, // SO_REUSEPORT
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
    valread = read( new_socket , buffer, 1024); 
    printf("%s\n",buffer ); 
    send(new_socket , hello , strlen(hello) , 0 ); 
    printf("Hello message sent\n"); 
    return 0; 
} 