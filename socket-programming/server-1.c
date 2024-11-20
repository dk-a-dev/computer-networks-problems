
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#define MAX 80
#define PORT 43454
#define SA struct sockaddr

void receiver(int arr[], int n, int sch, char *response)
{
    int checksum, sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];
    sum += sch;
    checksum = ~sum;
    if (checksum == 0)
        snprintf(response, MAX, "Checksum verification successful.");
    else
        snprintf(response, MAX, "Checksum verification failed.");
}

void goBackN(int sockfd)
{
    char buff[MAX];
    int total_frames, window_size;
    printf("Enter the total number of frames: ");
    scanf("%d", &total_frames);
    printf("Enter the window size: ");
    scanf("%d", &window_size);

    write(sockfd, &total_frames, sizeof(total_frames));
    write(sockfd, &window_size, sizeof(window_size));

    int i = 0;
    while (i < total_frames)
    {
        printf("Transmitting frames: ");
        for (int j = i; j < i + window_size && j < total_frames; j++)
            printf("%d ", j + 1);

        printf("\n");

        snprintf(buff, sizeof(buff), "Sending frames: ");
        for (int j = i; j < i + window_size && j < total_frames; j++)
            snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), "%d ", j + 1);

        write(sockfd, buff, strlen(buff) + 1);

        int ack;
        read(sockfd, &ack, sizeof(ack));
        if (ack >= i + window_size)
            i += window_size;
        else
        {
            printf("Resending from frame %d\n", ack + 1);
            i = ack;
        }
    }
}

void stopAndWait(int sockfd)
{
    char buff[MAX];
    int total_frames;
    printf("Enter the total number of frames: ");
    scanf("%d", &total_frames);
    write(sockfd, &total_frames, sizeof(total_frames));

    for (int i = 0; i < total_frames; i++)
    {
        printf("Transmitting frame %d\n", i + 1);
        snprintf(buff, sizeof(buff), "Frame %d", i + 1);
        write(sockfd, buff, strlen(buff) + 1);
        char ack;
        read(sockfd, &ack, sizeof(ack));
        if (ack == 'Y' || ack == 'y')
        {
            printf("Frame %d acknowledged, moving to next frame.\n", i + 1);
        }
        else
        {
            printf("Frame %d not acknowledged, resending.\n", i + 1);
            i--;
        }
    }
}

int input[32];
int code[32];
int ham_calc(int position, int c_l)
{
    int count = 0, i, j;
    i = position - 1;
    while (i < c_l)
    {
        for (j = i; j < i + position && j < c_l; j++)
        {
            if (code[j] == 1)
                count++;
        }
        i = i + 2 * position;
    }
    return count % 2;
}

void hammingCode(int sockfd)
{
    char buff[MAX];
    int n, i, p_n = 0, c_l, j, k;

    // Read the length of the Data Word
    read(sockfd, &n, sizeof(n));
    // Read the Data Word
    read(sockfd, input, sizeof(int) * n);

    // Calculate the number of parity bits needed
    i = 0;
    while (n > (int)pow(2, i) - (i + 1))
    {
        p_n++;
        i++;
    }

    c_l = p_n + n;

    // Initialize the code array with data and parity bits
    j = k = 0;
    for (i = 0; i < c_l; i++)
    {
        if (i == ((int)pow(2, k) - 1))
        {
            code[i] = 0;
            k++;
        }
        else
        {
            code[i] = input[j];
            j++;
        }
    }

    // Calculate parity bits
    for (i = 0; i < p_n; i++)
    {
        int position = (int)pow(2, i);
        int value = ham_calc(position, c_l);
        code[position - 1] = value;
    }

    // Send the calculated Code Word to the client
    write(sockfd, code, sizeof(int) * c_l);

    // Read the received Code Word from the client
    read(sockfd, code, sizeof(int) * c_l);

    // Check for errors
    int error_pos = 0;
    for (i = 0; i < p_n; i++)
    {
        int position = (int)pow(2, i);
        int value = ham_calc(position, c_l);
        if (value != 0)
            error_pos += position;
    }

    // Send the error position to the client
    write(sockfd, &error_pos, sizeof(error_pos));
}

void func(int sockfd)
{
    char buff[MAX];
    char response[MAX];
    for (;;)
    {
        bzero(buff, MAX);
        read(sockfd, buff, sizeof(buff));
        printf("\nMessage received From client: %s\n", buff);

        if (strncmp("exit", buff, 4) == 0)
        {
            printf("Server Exit...\n");
            break;
        }

        bzero(response, MAX);

        if (strncmp("GBQ", buff, 3) == 0)
        {
            goBackN(sockfd);
            snprintf(response, MAX, "Go-Back-N ARQ protocol completed.");
        }
        else if (strncmp("SNW", buff, 3) == 0)
        {
            stopAndWait(sockfd);
            snprintf(response, MAX, "Stop-and-Wait ARQ protocol completed.");
        }
        else if (strncmp("HAMMING", buff, 7) == 0)
        {
            hammingCode(sockfd);
            snprintf(response, MAX, "Hamming code processed.");
        }

        else if (strncmp("CHECKSUM", buff, 8) == 0)
        {
            int n;
            sscanf(buff + 9, "%d", &n); // Extract the size of the array
            int arr[n];
            for (int i = 0; i < n; i++)
            {
                sscanf(buff + 9 + (i + 1) * sizeof(int), "%d", &arr[i]); // Extract array elements
            }
            int sch;
            sscanf(buff + 9 + (n + 1) * sizeof(int), "%d", &sch); // Extract checksum from client
            receiver(arr, n, sch, response);
        }
        else if (strncmp("PALINDROME", buff, 10) == 0)
        {

            char str[MAX];
            sscanf(buff + 11, "%s", str);
            int len = strlen(str);
            int is_palindrome = 1;
            for (int i = 0; i < len / 2; i++)
            {
                if (str[i] != str[len - i - 1])
                {
                    is_palindrome = 0;
                    break;
                }
            }
            if (is_palindrome)
                snprintf(response, MAX, "The string '%s' is a palindrome.", str);
            else
                snprintf(response, MAX, "The string '%s' is NOT a palindrome.", str);
        }

        else if (strncmp("ARMSTRONG", buff, 9) == 0)
        {

            int num, original_num, remainder, result = 0;
            sscanf(buff + 10, "%d", &num);
            original_num = num;
            while (original_num != 0)
            {
                remainder = original_num % 10;
                result += remainder * remainder * remainder;
                original_num /= 10;
            }
            if (result == num)
                snprintf(response, MAX, "%d is an Armstrong number.", num);
            else
                snprintf(response, MAX, "%d is NOT an Armstrong number.", num);
        }

        else if (strncmp("REVERSE", buff, 7) == 0)
        {

            char str[MAX];
            sscanf(buff + 8, "%s", str);
            int len = strlen(str);
            for (int i = 0; i < len; i++)
                response[i] = str[len - i - 1];
            response[len] = '\0';
        }

        else if (strncmp("CLASSIP", buff, 7) == 0)
        {

            char ip[20];
            sscanf(buff + 8, "%s", ip);
            unsigned int first_octet;

            if (sscanf(ip, "%u", &first_octet) != 1 || first_octet > 255)
            {
                snprintf(response, MAX, "Invalid IP address format: %s", ip);
            }
            else
            {
                if (first_octet >= 1 && first_octet <= 126)
                    snprintf(response, MAX, "Class A IP");
                else if (first_octet >= 128 && first_octet <= 191)
                    snprintf(response, MAX, "Class B IP");
                else if (first_octet >= 192 && first_octet <= 223)
                    snprintf(response, MAX, "Class C IP");
                else if (first_octet >= 224 && first_octet <= 239)
                    snprintf(response, MAX, "Class D IP");
                else if (first_octet >= 240 && first_octet <= 255)
                    snprintf(response, MAX, "Class E IP");
                else
                    snprintf(response, MAX, "Invalid IP class");
            }
        }
        else if (strncmp("CALCULATE", buff, 9) == 0)
        {
            char operation;
            int num_1, num_2, result;
            sscanf(buff + 10, "%d %c %d", &num_1, &operation, &num_2);
            switch (operation)
            {
            case '+':
                result = num_1 + num_2;
                break;
            case '-':
                result = num_1 - num_2;
                break;
            case '*':
                result = num_1 * num_2;
                break;
            case '/':
                if (num_2 != 0)
                    result = num_1 / num_2;
                else
                {
                    snprintf(response, MAX, "Error: Division by zero");
                    write(sockfd, response, sizeof(response));
                    continue;
                }
                break;
            default:
                snprintf(response, MAX, "Error: Invalid operation");
                write(sockfd, response, sizeof(response));
                continue;
            }
            snprintf(response, MAX, "Result: %.2lf", (float)result);
        }
        else
        {
            snprintf(response, MAX, "Unknown command: %s", buff);
        }
        write(sockfd, response, sizeof(response));
    }
}

int main()
{
    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd != -1)
        printf("Socket successfully created..\n");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) >= 0)
        printf("Socket bind successful..\n");

    if (listen(sockfd, 5) >= 0)
        printf("Server Listening..\n");

    len = sizeof(cli);
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd >= 0)
        printf("Server accept the client..\n");

    func(connfd);
    close(sockfd);
}