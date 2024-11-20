#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#define MAX 80
#define PORT 43454
#define SA struct sockaddr

void goBackN(int sockfd)
{
    int total_frames, window_size;
    read(sockfd, &total_frames, sizeof(total_frames));
    read(sockfd, &window_size, sizeof(window_size));

    char buff[MAX];
    while (1)
    {
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("\nReceived from server: %s\n", buff);

        if (strncmp(buff, "exit", 4) == 0)
        {
            printf("Client Exit...\n");
            break;
        }
        int ack;
        printf("Enter the last frame number received correctly: ");
        scanf("%d", &ack);
        write(sockfd, &ack, sizeof(ack));
    }
}

void stopAndWait(int sockfd)
{
    int total_frames;
    read(sockfd, &total_frames, sizeof(total_frames));

    char buff[MAX];
    for (int i = 0; i < total_frames; i++)
    {
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("\nReceived from server: %s\n", buff);

        char ack;
        printf("Acknowledge received for frame %d (Y/N): ", i + 1);
        scanf(" %c", &ack);
        write(sockfd, &ack, sizeof(ack));
    }
}

int sender(int arr[], int n)
{
    int checksum, sum;
    for (int i = 0; i < n; i++)
        sum += arr[i];
    checksum = ~sum;
    return checksum;
}

void hammingCode(int sockfd)
{
    int n, i;
    int input[32];
    int code[32];

    // Enter the length of the Data Word
    printf("Please enter the length of the Data Word: ");
    scanf("%d", &n);
    // Enter the Data Word
    printf("Please enter the Data Word:\n");
    for (i = 0; i < n; i++)
    {
        scanf("%d", &input[i]);
    }

    // Send the length of the Data Word to the server
    write(sockfd, &n, sizeof(n));
    // Send the Data Word to the server
    write(sockfd, input, sizeof(int) * n);

    // Receive the calculated Code Word from the server
    read(sockfd, code, sizeof(int) * (n + (int)ceil(log2(n + 1))));

    printf("\nThe calculated Code Word is: ");
    for (i = 0; i < n + (int)ceil(log2(n + 1)); i++)
        printf("%d", code[i]);
    printf("\n");

    // Enter the received Code Word
    printf("Please enter the received Code Word:\n");
    for (i = 0; i < n + (int)ceil(log2(n + 1)); i++)
        scanf("%d", &code[i]);

    // Send the received Code Word to the server
    write(sockfd, code, sizeof(int) * (n + (int)ceil(log2(n + 1))));

    // Receive the error position from the server
    int error_pos;
    read(sockfd, &error_pos, sizeof(error_pos));

    if (error_pos == 0)
        printf("The received Code Word is correct.\n");
    else
        printf("Error at bit position: %d\n", error_pos);
}

void func(int sockfd)
{
    char buff[MAX];
    int n;

    for (;;)
    {
        bzero(buff, sizeof(buff));
        printf("\nChoose an option: \n");
        printf("1. Use Go-Back-N ARQ Protocol (GBQ)\n");
        printf("2. Use Stop-and-Wait ARQ Protocol (SNW)\n");
        printf("3. Use Commands (e.g., PALINDROME, ARMSTRONG, REVERSE, CLASSIP,CALCULATE etc.)\n");
        printf("4. HAMMING");
        printf("Enter your choice: ");
        fgets(buff, sizeof(buff), stdin);

        if (buff[0] == '1')
        {
            snprintf(buff, sizeof(buff), "GBQ");
            write(sockfd, buff, sizeof(buff));
            goBackN(sockfd);
        }
        else if (buff[0] == '2')
        {
            // Send SNW command to server
            snprintf(buff, sizeof(buff), "SNW");
            write(sockfd, buff, sizeof(buff));
            stopAndWait(sockfd);
        }
        else if (buff[0] == '3')
        {
            while (1)
            {
                bzero(buff, sizeof(buff));
                printf("\nEnter a command (e.g., PALINDROME <string>, ARMSTRONG <number>, REVERSE <string>, CLASSIP <IP Address>, CALCULATE 5 + 3, CHECKSUM <size> <elements>): ");
                fgets(buff, sizeof(buff), stdin);

                if (strncmp(buff, "CHECKSUM", 8) == 0)
                {
                    int n;
                    sscanf(buff + 9, "%d", &n); // Extract the size of the array
                    int arr[n];
                    printf("Enter the elements of the array:\n");
                    for (int i = 0; i < n; i++)
                    {
                        scanf("%d", &arr[i]);
                    }
                    int sch = sender(arr, n);
                    snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), " %d", sch); // Append checksum to the buffer
                    write(sockfd, buff, sizeof(buff));
                }
                else
                {
                    write(sockfd, buff, sizeof(buff));
                }

                // Exit condition
                if (strncmp(buff, "exit", 4) == 0)
                {
                    printf("Client Exit...\n");
                    break;
                }

                // Receive response from server
                bzero(buff, sizeof(buff));
                read(sockfd, buff, sizeof(buff));
                printf("\nResponse from Server: %s\n", buff);
            }
        }
        else if (buff[0] == '4')
        {
            snprintf(buff, sizeof(buff), "HAMMING");
            write(sockfd, buff, sizeof(buff));
            hammingCode(sockfd);
        }

        else
        {
            printf("Invalid choice. Please enter 1 or 2.\n");
        }
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd != -1)
        printf("Socket successfully created..\n");

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) >= 0)
        printf("connected to the server..\n");
    func(sockfd);
    close(sockfd);
    return 0;
}