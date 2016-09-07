/*
 * ClientHandler.h
 *
 *  Created on: Sep 11, 2015
 *      Author: fabian
 */

#ifndef CLIENTHANDLER_H_
#define CLIENTHANDLER_H_

#include<list>
#include <string>
#include "Socket.h"
#include "message.h"
#include <tr1/memory>
#include <DataTypes/fsarray.h>

class ClientHandler {
    private:
		Socket sock;
		std::string name;
	public:
		ClientHandler(std::string nm, const char* IP, int portno):sock(Socket(IP, portno)), name(nm){};
		~ClientHandler();
		void send_message(std::unique_ptr<message> ptr, uint32_t target);
		bool check_pending();
		std::unique_ptr<raw_message> get_next_message();
		void disconnect();

};

#endif /* CLIENTHANDLER_H_ */
