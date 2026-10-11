

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 30017
#define SIZE 1024

int main(int argc, char *argv[])
{
    int sockfd;
    char message[SIZE], buffer[SIZE];
    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    if (argc != 3) {
        printf("Usage: %s <server_ip> <roll>\n", argv[0]);
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("Socket error");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1],
                  &server.sin_addr) != 1) {
        printf("Invalid server IP address\n");
        close(sockfd);
        return 1;
    }

    while (1) {
        printf("Enter text (or EXIT): ");

        if (fgets(message, SIZE, stdin) == NULL)
            strcpy(message, "EXIT");
        else
            message[strcspn(message, "\n")] = '\0';

        
        char request[SIZE];
        snprintf(request, SIZE, "%s#%s", argv[2], message);

        // Send request
        sendto(sockfd, request, strlen(request), 0,
               (struct sockaddr *)&server, len);

        // Receive response
        int n = recvfrom(sockfd, buffer, SIZE - 1, 0,
                         (struct sockaddr *)&server, &len);

        if (n < 0) {
            perror("Receive error");
            break;
        }

        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);

        if (strcmp(message, "EXIT") == 0)
            break;
    }

    close(sockfd);
    return 0;
}