#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h> 

#define BUFLEN 512  //Max length of buffer
 
void die(char *s)
{
    perror(s);
    exit(1);
}

 
int main(int argc, char *argv[])
{
    struct sockaddr_in si_me, si_other;
    struct timeval timeout={0,0};
    fd_set readfds;
    int select_ret;
    int s, i, j = 0, slen = sizeof(si_other), recv_len, portno;
    int ctransID = 0, lifeTime = 3600;
    char buf[BUFLEN], message[1024];
    char cipadd[BUFLEN] = "";
    char fileGetIPAddr[50];
    char stringLine[50], rmLine1[50], ipCount[5];
    FILE *fileStream, *fileTemp;
    time_t start, end;
    double elapsed = 0;
     
    //create a UDP socket
    if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
    portno = atoi(argv[1]); //The port on which to listen for incoming data
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(portno);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if(bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
        die("bind");
    
    system("clear");
    printf("...This a is server of DHCP...\n\n");

    //keep listening for data
    while(1)
    {
        do
        {
            FD_ZERO(&readfds); //Zero out socket set
            FD_SET(s, &readfds); //Add socket to listen to
            select_ret = select(s+1, &readfds, NULL, NULL, &timeout);

            fflush(stdout);

            //Begin sequence for DHCP discover - Receive broadcast message from client
            printf("\nBegin DHCP 4-Handshake - Discover client arrives\n");
            if((recv_len = recvfrom(s, &cipadd, sizeof(cipadd), 0, (struct sockaddr *) &si_other, &slen)) == -1)
                die("recvfrom()");
            if((recv_len = recvfrom(s, &ctransID, sizeof(ctransID), 0, (struct sockaddr *) &si_other, &slen)) == -1)
                die("recvfrom()");
            printf("Broadcast from client from %s, port number:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
            printf("Client's Ip address: %s\n", cipadd);
            printf("Transaction ID: %d\n", ctransID);

            //Send to the server, print error if it doesn't work
            if(sendto(s, &cipadd, sizeof(cipadd), 0, (struct sockaddr *) &si_other, slen) == -1)
                die("sendto()");
            if(sendto(s, &ctransID, sizeof(ctransID), 0, (struct sockaddr *) &si_other, slen) == -1)
                die("sendto()");
            if(sendto(s, &lifeTime, sizeof(lifeTime), 0, (struct sockaddr *) &si_other, slen) == -1)
                die("sendto()");

            //Begin sequence for DHCP request - Receive request/confirmation message from client
            if((recv_len = recvfrom(s, &cipadd, sizeof(cipadd), 0, (struct sockaddr *) &si_other, &slen)) == -1)
                die("recvfrom()");
            if((recv_len = recvfrom(s, &ctransID, sizeof(ctransID), 0, (struct sockaddr *) &si_other, &slen)) == -1)
                die("recvfrom()");
            if((recv_len = recvfrom(s, &lifeTime, sizeof(lifeTime), 0, (struct sockaddr *) &si_other, &slen)) == -1)
                die("recvfrom()");
            printf("Request from client from %s, port number:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
            printf("Confirmed Ip address: %s", cipadd);
            printf("Transaction ID: %d\n", ctransID);
            printf("Lifetime: %d\n\n", lifeTime);

            //Begin sequence for DHCP acknowledge - Send acknowledgment message to client
            printf("Sending ACK message to client:%s, on port number:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
            printf("Ip address: %s", cipadd);
            printf("Transaction ID: %d\n", ctransID);
            printf("Lifetime: %d\n\n", lifeTime);
            //Send ACK message to client
            if(sendto(s, &cipadd, sizeof(cipadd), 0, (struct sockaddr *) &si_other, slen) == -1)
                die("sendto()");
            if(sendto(s, &ctransID, sizeof(ctransID), 0, (struct sockaddr *) &si_other, slen) == -1)
                die("sendto()");
            if(sendto(s, &lifeTime, sizeof(lifeTime), 0, (struct sockaddr *) &si_other, slen) == -1)
                die("sendto()");

            //Start the Lifetime timer here
            time(&start);

            //Update the IPaddress file
            //fileStream = fopen("IPadress.txt", "r"); //Original file
            fileTemp = fopen("File_copy.txt", "w");  //Temporary file

            i = 0;
            while(i < 9)
            {
                fscanf(fileStream, "%s", stringLine); //Scan the line of the file at line i
                i++;
                if(i == 0)
                    strcpy(rmLine1, stringLine); //Get the first line in the IPaddress file for file removal
                else //Not the first IP Address in IPaddress.txt
                    fprintf(fileTemp, "%s\n", stringLine); //Copy the next line to the temporary file File_copy.txt
            }
            strcpy(rmLine1, "192.168.56."); 
            //itoa(j, ipCount, 10);
            sprintf(ipCount, "%d", j);
            j++; //Update the counter for next IP Address in list to use
            strcat(rmLine1, ipCount); //Attach updated IP Address and store in variable
            fprintf(fileTemp, "%s\n", rmLine1); //Update the last used IP address back to the file to cycle through
            fclose(fileStream);
            fclose(fileTemp);

            system("rm IPaddress.txt"); //Remove old IPaddress.txt
            system("mv File_copy.txt IPaddress.txt"); //Rename File_copy.txt to IPaddress.txt

            //Grab the end time then show elapsed for Lifetime counter if it were a requirement
            //it would be implemented with something like this...
            sleep(5);
            time(&end);
            elapsed = difftime(end, start);
            printf("Elapsed time for IPaddress: %s is - %.2lf secs\n\n", rmLine1, elapsed);
		
           }while(select_ret > 0);
    }
 
    pclose(0);
    return 0;
}