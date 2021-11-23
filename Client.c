
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
 
#define SERVER "192.168.56.3" 
#define BUFLEN 512  
 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(int argc, char *argv[])
{
    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other), recv_len, portno;
    int transID, lifeTime;
    time_t t;
    char buf[BUFLEN], message[BUFLEN];
    char ipadd[BUFLEN] = "192.168.56.1";

    system("clear");
    printf("...This is a client of DHCP...\n\n");
 
    if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    portno = atoi(argv[1]); 
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(portno);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
    
    
    printf("\nBegin DHCP 4-Handshake - Discover client arrives\n");
    printf("Ip address: %s\n", ipadd);
    srand((unsigned) time(&t)); 
    transID = rand() % 900 + 100; 
    printf("Transaction ID: %d\n", transID);
    
    if(sendto(s, &ipadd, sizeof(ipadd), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");
    if(sendto(s, &transID, sizeof(transID), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");
    
    
    if((recv_len = recvfrom(s, &ipadd, sizeof(ipadd), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");
    if((recv_len = recvfrom(s, &transID, sizeof(transID), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");
    if((recv_len = recvfrom(s, &lifeTime, sizeof(lifeTime), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");
    printf("\n-From DHCP server-\n");
    printf("Assigned ip address is: %s", ipadd); 
    printf("For Transaction ID: %d\n", transID);
    printf("Assigned Lifetime: %d secs\n\n", lifeTime);

    
    printf("Confirm Ip address is: %s", ipadd);
    printf("For Transaction ID: %d\n", ++transID);
    printf("With Lifetime of: %d\n\n", lifeTime);
    
    if(sendto(s, &ipadd, sizeof(ipadd), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");
    if(sendto(s, &transID, sizeof(transID), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");
    if(sendto(s, &lifeTime, sizeof(lifeTime), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");
    
    if((recv_len = recvfrom(s, &ipadd, sizeof(ipadd), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");
    if((recv_len = recvfrom(s, &transID, sizeof(transID), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");
    if((recv_len = recvfrom(s, &lifeTime, sizeof(lifeTime), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");
    printf("-ACK From DHCP server-\n");
    printf("Ip address is: %s", ipadd); 
    printf("Transaction ID: %d\n", transID);
    printf("Lifetime: %d secs\n\n", lifeTime);
    
    pclose(0);
    return 0;
}
