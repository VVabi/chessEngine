/*
 * Socket.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: fabian
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <sys/ioctl.h>
#include <stdint.h>
#include <list>
#include "Socket.h"

Socket::Socket(const char* IP, int portno){

	struct sockaddr_in serv_addr;
	sockfd = socket(AF_INET,SOCK_STREAM, 0);

	if(sockfd < 0){
		perror("could not open socket");
		exit(0);
	}

	struct hostent *server = gethostbyname(IP);

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);

	if(connect(sockfd, (const sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Error connecting to server");
	}
	else{
		std::cout << "Successfully connected" <<std::endl;
	}


}

bool Socket::binary_read(uint8_t* buffer, uint32_t length){
	int n = 0;
	int offset = 0;
	while(length > 0)
	{
		n = read(sockfd, buffer+offset, length);
		if(n < 0){
			return false;
		}
		length = length-n;
		offset = offset+n;
	}
	return true;
}

bool Socket::binary_write(uint8_t* buffer, uint32_t length){
	int n = 0;
	int offset = 0;
	int counter = 0;

	while(length > 0)
	{
		counter++;
		n = write(sockfd, buffer+offset, length);
		if(n < 0){
			std::cout << n <<std::endl;
			n = 0;
		}
		length = length-n;
		offset = offset+n;
		if(counter > 100){
				std::cout << counter <<std::endl;
				return false;
		}
	}
	return true;
}

uint32_t Socket::get_number_of_pending_bytes(){
	uint32_t count = 0;
	ioctl(sockfd, FIONREAD, &count);
	return count;
}

void Socket::disconnect(){
	shutdown(sockfd, 2);
}

