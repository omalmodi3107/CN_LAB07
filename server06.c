

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 30017
#define SIZE 1024

int main()
{
    int sockfd;
    char buffer[SIZE], reply[SIZE];
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("Socket error");
        return 1;
    }

   
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(sockfd, (struct sockaddr *)&server,
             sizeof(server)) < 0) {
        perror("Bind error");
        close(sockfd);
        return 1;
    }

    printf("Server running on port %d\n", PORT);

    while (1) {
        int n = recvfrom(sockfd, buffer, SIZE - 1, 0,
                         (struct sockaddr *)&client, &len);

        if (n < 0)
            continue;

        buffer[n] = '\0';

        printf("\nClient IP: %s, Port: %d\n",
               inet_ntoa(client.sin_addr),
               ntohs(client.sin_port));

        
        if (strncmp(buffer, "24105142017#", 12) != 0) {
            strcpy(reply, "ERROR: Unauthorized client");
        } else {
            char *msg = buffer + 12;

            printf("Received: %s\n", msg);

          
            if (strcmp(msg, "EXIT") == 0) {
                strcpy(reply, "Server shutting down");
                sendto(sockfd, reply, strlen(reply), 0,
                       (struct sockaddr *)&client, len);
                printf("Sent: %s\n", reply);
                break;
            }

            
            int vowels = 0, consonants = 0, digits = 0;
            int length = strlen(msg);

            for (int i = 0; msg[i] != '\0'; i++) {
                char ch = tolower((unsigned char)msg[i]);

                if (ch >= 'a' && ch <= 'z') {
                    if (ch == 'a' || ch == 'e' ||
                        ch == 'i' || ch == 'o' ||
                        ch == 'u')
                        vowels++;
                    else
                        consonants++;
                } else if (ch >= '0' && ch <= '9') {
                    digits++;
                }
            }

            sprintf(reply,
                    "LEN=%d;VOWELS=%d;CONSONANTS=%d;DIGITS=%d",
                    length, vowels, consonants, digits);
        }

       
        sendto(sockfd, reply, strlen(reply), 0,
               (struct sockaddr *)&client, len);

        printf("Sent response: %s\n", reply);
    }

    close(sockfd);
    printf("Server closed.\n");

    return 0;
}