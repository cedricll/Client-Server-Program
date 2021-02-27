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
#define MAXLINE 30

struct date {
    char date_str[10];
    int year;
    int month;
    int day;
};

struct stock {
    int price;
};

int parse_date(char *str, struct date *dates, int index) { // minor fix: adjust to months with 30 or 31 days, february with 28 and 29?
    // printf("|parse_date function ran| ");
    // Extract the first token, the year
    int curr = 0; // 0 for year, 1 for month, 2 for day
    char *found; // pointer to parse the given date
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

void collect_data(char data_to_read[], int size, FILE** fp, struct date *dates, struct stock *stocks) { // collects data from a file
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

void prices(char* date, struct date *company_dates, struct stock *company_stocks, int size) { // FIX
    // display stock prices on a given date
    // if params dont exists, server responds with "Unknown" to client screen
    printf("Date: %s", date);
    int i;
    for (i = 0; i < size; i++) {
        if ( strcmp(company_dates[i].date_str, date) == 0 ) {
            char buffer[30];
            printf("struct price: %s", company_stocks[i].price);
            // itoa(company_stocks[i].price, buffer, 10);
            sprintf(buffer, "%d", company_stocks[i].price);
            printf("Price: $s", buffer);
        }
    }

}

void max_profit(char* stock_name) {
    // calc max profit of a share
}

int check_stock_name(char *token, char *company1, char *company2) {
    return strcmp(token, company1) == 0 || strcmp(token, company2) == 0;
}

int check_date_format(char* date) { // TODO
    return 1;
}

void copyCompanyName(char *name, const char *file_name) { // AAPL.csv (8 - 4)
    int i;
    for (i = 0; i < ( strlen(file_name) - 4) ; i++) {
        name[i] = file_name[i];
    }
    name[( strlen(file_name) - 4)] = '\0';
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

    char company1[MAXLINE];
    char company2[MAXLINE];

    copyCompanyName(company1, argv[1]);
    copyCompanyName(company2, argv[2]);

    printf("company1: %s\n", company1);
    printf("company2: %s\n", company2);

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
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, // | SO_REUSEPORT
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( atoi(argv[3]) ); // PORT
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    if (listen(server_fd, 3) < 0) // 2nd param: defines the maximum length to which the queue of pending connections for sockfd may grow. 
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

    // use AAPL_dates and AAPL_stocks structs to respond to user input
    // char buffer[1024] = {0}; 
    char result[1024]; // sent to client screen
    // modify this:

    while (1) {
        char buffer[1024] = {0}; 
        valread = read( new_socket , buffer, 1024); // client sends content to show server screen
        printf("buffer: %s\n", buffer ); // parse buffer

        // buffer functon
        char *buf = buffer;
        // printf("buf: %s\n", buf);

        char *found;
        char cmd[30];
        char stock_name[30];
        char date[30];

        int curr = 0;


        while( (found = strsep(&buf," ")) != NULL ) {
            // printf("found: %s\n",found);

            if (curr == 0) {
                //strcat(found, "\0");

                // clear cmd

                strcpy(cmd, found);
                
                printf("command: %s\n", cmd);
            }
            else if (curr == 1) {
                if ( check_stock_name(found, company1, company2) ) {
                    strcpy(stock_name, found);
                }
                else {
                    printf("Invalid");
                    strcpy(result, "Invalid syntax");
                }
                // }
                printf("stock_name: %s\n", stock_name);
            }
            else if (curr == 2) {
                // if ( check_date_format(token) ) { // NEED THIS
                //strcat(found, "\0");

                // clear date

                strcpy(date, found);
                
                // }
                printf("date %s\n", date);
            }
            else {
                break;
            }
            curr++;
        }

        printf("While loop ended\n");

        if (strcmp(cmd, "Prices") == 0) {
            printf("Prices Function");
            // assign result
            if (stock_name == company1) {
                prices(date, AAPL_dates, AAPL_stocks, size_of_AAPL);
            }
            else if (stock_name == company2) {
                prices(date, TWTR_dates, TWTR_stocks, size_of_TWTR);
            }
            else {
                strcpy(result, "Invalid syntax");
            }
            
        }

        else if (strcmp(cmd, "MaxProfit") == 0) {
            printf("MaxProfit Function");
        }
        else {
            printf("Invalid command");
            // assign result
        }
        
        strcpy(result, "Testing Server Sent Message");

        send(new_socket , result , strlen(result) , 0 ); // result of calc is recorded in "hello"
        // printf("Server: Hello message sent\n"); 
    } 
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