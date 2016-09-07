/*
 * Socket.h
 *
 *  Created on: Sep 11, 2015
 *      Author: fabian
 */

#ifndef SOCKET_H_
#define SOCKET_H_
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


class Socket{
	private:
		int sockfd;

	public:
		Socket(const char* IP, int portno);
		bool binary_read(uint8_t* buffer, uint32_t length);
		bool binary_write(uint8_t* buffer, uint32_t length);
		uint32_t get_number_of_pending_bytes();
		void disconnect();

};



#endif /* SOCKET_H_ */
