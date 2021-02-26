#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

#include <sys/types.h>
#include <netdb.h>

#define PORT 30000
#define size_of_AAPL 503
#define size_of_TWTR 503

struct date {
    char date_str[10];
    int year;
    int month;
    int day;
};

struct stock {
    int price;
};

int parse_date(char *str, struct date *dates, int index) {
    // printf("|parse_date function ran| ");
    // Extract the first token, the year
    int curr = 0; // 0 for year, 1 for month, 2 for day
    char *found;
    char str_copy[10];
    strcpy(str_copy, str);

    while( (found = strsep(&str,"-")) != NULL ) {
        if (curr == 0) { // year
            if (atoi(found) > 2020 || atoi(found) < 2018) {
                return -1;
            }
            else {
                dates[index].year = atoi(found);
                // printf("year: %s\n",found);
            }
        }

        else if (curr == 1) { // month
            if (atoi(found) > 12 || atoi(found) < 1) {
                return -1;
            }
            else {
                dates[index].month = atoi(found);
                // printf("month: %s\n", found);
            }
        }

        else if (curr == 2) { // day
            if (atoi(found) > 31 || atoi(found) < 1) {
                return -1;
            }
            else {
                dates[index].day = atoi(found);
                // printf("day: %s\n", found);
            }
        }
        curr++;
    }

    // printf("str: %s\n", str_copy);
    strcpy(dates[index].date_str, str_copy);
    return 1;
}

void collect_data(char data_to_read[], int size, FILE** fp, struct date *dates, struct stock *stocks) { 
    int row = 0;
    int col = 0;
    int index = -1;
    
    while (fgets(data_to_read, size, *fp) != NULL) {         // reads the whole file
        col = 0;
        row++;

        if (row == 1) { // avoid reading the data headers  
            continue;
        }

        index++;

        char *value = strtok(data_to_read, ", ");           // reads a line
        while (value) {                                     // reads a single word
            // printf("col #: %d", col);                               
            if (col == 0) { // date column
                // printf("col 1: %s ", value); 

                if (parse_date(value, dates, index) == -1) {
                    // not a valid date
                    index--;
                    break;
                } 
            }

            if (col == 4) { // price column
                // printf("col 5 %s", value);  
                stocks[index].price = atoi(value);
            }
            
            value = strtok(NULL, ", ");
            col++;
        }
        // printf("%s", data_to_read);
        // printf("\n");
    }
}

int main(int argc, char const *argv[]) // ./server AAPL.csv TWTR.csv 30000
{ 
    printf("argv[1]:%s\n", argv[1]); //AAPL.CSV
    printf("argv[2]:%s\n", argv[2]); //TWTR.csv
    printf("argv[3]:%s\n", argv[3]); //30000 // NEED TO MODIFY CLIENT/SERVER connection code from socket tutorial
    /*TODO:
        1.) read from apple and twitter files; 7/2/2018 - 6/30/2020
            -> gaps in data?
            -> use "date" and "stock" structs to collect the data (array of structs for date and another for stock)
        2.) start listening to client requests

        > check if csv files are valid
    */

    printf("server started\n");

    struct date AAPL_dates[size_of_AAPL];
    struct stock AAPL_stocks[size_of_AAPL];

    struct date TWTR_dates[size_of_TWTR];
    struct stock TWTR_stocks[size_of_TWTR];
    
    // read from csv files
    FILE *AAPL_file_pointer; 
    FILE *TWTR_file_pointer;
    
    AAPL_file_pointer = fopen(argv[1], "r");
    TWTR_file_pointer = fopen(argv[2], "r");

    if (AAPL_file_pointer == NULL || TWTR_file_pointer == NULL) {
        printf("Failed to open one or both of the following:\n%s\n%s\n", argv[1], argv[2]);
        // throw error?
    }
    else {
        printf("Files opened:\n%s\n%s\n", argv[1], argv[2]);

        char AAPL_to_read[size_of_AAPL];
        char TWTR_to_read[size_of_TWTR];

        collect_data(AAPL_to_read, size_of_AAPL, &AAPL_file_pointer, AAPL_dates, AAPL_stocks);
        collect_data(TWTR_to_read, size_of_TWTR, &TWTR_file_pointer, TWTR_dates, TWTR_stocks);

        fclose(AAPL_file_pointer);
        fclose(TWTR_file_pointer);
    }

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


// printf("loop through structs:\n");
    // int i;
    // for (i = 0; i < size_of_TWTR; i++) {
    //     printf("Apple:\n");
    //     printf("date: %s " , AAPL_dates[i].date_str);
    //     printf("day: %d ", AAPL_dates[i].day);
    //     printf("month: %d ", AAPL_dates[i].month);
    //     printf("year: %d ", AAPL_dates[i].year);
        
    //     printf("price: %d\n\n", AAPL_stocks[i].price );
        
    //     printf("Twitter:\n");
    //     printf("date: %s " , TWTR_dates[i].date_str);
    //     printf("day: %d ", TWTR_dates[i].day);
    //     printf("month: %d ", TWTR_dates[i].month);
    //     printf("year: %d ", TWTR_dates[i].year);
        
    //     printf("price: %d\n\n", TWTR_stocks[i].price );
    // }