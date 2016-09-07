/*
 * VMPServer.h
 *
 *  Created on: Sep 11, 2015
 *      Author: fabian
 */

#ifndef VMPSERVER_H_
#define VMPSERVER_H_

#include <list>
#include "message.h"
#include "ClientHandler.h"
#include <tr1/memory>
#include <DataTypes/fsarray.h>
#include <mutex>

class VMPServer {

	private:
		std::list<std::unique_ptr<raw_message>> VMPList;
		ClientHandler client;
		std::mutex communication_mutex;
	public:
		VMPServer(const char* IP, int portno): client(ClientHandler("default", IP, portno)){
			VMPList = std::list<std::unique_ptr<raw_message>>();
		};
		std::unique_ptr<raw_message> get_message(uint32_t unique_id);
		void send_message(std::unique_ptr<message> ptr, uint32_t target);
		void receive_from_network();
		void disconnect();

		VMPServer(const VMPServer& serv) = delete;
		VMPServer(const VMPServer&& serv) = delete;
};

#endif /* VMPSERVER_H_ */
