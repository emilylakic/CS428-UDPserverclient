#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <ctime>


#define PORT	 12000

int main(){
	int sockfd, n;
	socklen_t len;
	char buffer[1024];
	struct sockaddr_in servaddr, cliaddr;

	//Initializing server and client addresses
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	//Server Information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY; // localhost
	servaddr.sin_port = htons(PORT); // Port Number

	//id defined to keep track of each ping received by client
	//Initialized to 1
	int id = 1;

	//For loop to let client receive exactly 10 pings
	for(int i =0;i<10;i++) {
		//Create a UDP socket. SOCK_DGRAM used for UDP packets.
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		//Timeout defined
		struct timeval timeout;
		//Timeout set to 1 second
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		//If timeout fails
		 if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
			printf("setsockopt failed\n");
		 if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
     	printf("setsockopt failed\n");

		//Time that client sends ping to server
		//Placed directly before sendto() to accurately record time ping was sent
		struct timeval now;
      		gettimeofday(&now, NULL);
      		int milli2 = now.tv_usec / 1000;
      		char newerBuffer[80];
      		strftime(newerBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&now.tv_sec));
      		char currentTime2[84] = "";
      		sprintf(currentTime2, "%s:%03d", newerBuffer, milli2);
		//Client sends ping to the server
		sendto(sockfd, (const char *)buffer, strlen(buffer),MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
		//Client recieves ping back from the server
		n = recvfrom(sockfd, (char *)buffer, sizeof(buffer), 0, ( struct sockaddr *) &cliaddr, &len);
		//Prints 'Ping Message: X', X being the corresponding id number depending on the number ping message
		printf("Ping Message %d\n",id);
		//id is incremented to account for next ping message being received from server
		id++;
		//Time the client receives ping back from server
		//Placed directly after recvfrom() to accurately record time ping was received
		struct timeval later;
      		gettimeofday(&later, NULL);
      		int milli = later.tv_usec / 1000;
     		char newBuffer[80];
      		strftime(newBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&later.tv_sec));
      		char currentTime[84] = "";
      		sprintf(currentTime, "%s:%03d", newBuffer, milli);
		//Determines the time difference between when the client sends ping to the time the clients receive a ping back
		//This is how the delay, or the Round Trip Time (RTT), is calculated
		//Microseconds are calculated to account for greater precision necessary for time difference (seconds and milliseconds outputted a difference of '0' in almost all cases)
		int microseconds = (later.tv_sec - now.tv_sec) * 1000000 + ((int)later.tv_usec - (int)now.tv_usec);
		int milliseconds = microseconds/1000;
		int seconds = milliseconds /1000;
		//If n, the number of bytes, is -1, the packet was lost
		if(n==-1) {
		printf("Packet Was Lost\n");
		}
		//If n, the number of bytes, is not -1, the packet was successful and the round trip time is calculated
		//Given in microseconds
		else {
		printf("Round Trip Time Delay (Microseconds): %i\n",microseconds);
		}
		//Prints a divider to allow for better visibility between each ping in the client terminal
		printf("----------\n");
		//If there is a request timeout
		if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		printf("Request timed out!");
		//Close the socket within the for loop
		close(sockfd);
		}
	}
	//Close the socket
	close(sockfd);
	return 0;
}
