/*
 * network.cpp
 *
 *  Created on: Sep 20, 2015
 *      Author: fabian
 */



#include <iostream>
#include <communication/Network/Socket.h>
#include <communication/Network/ClientHandler.h>
#include <communication/Network/VMPServer.h>
#include <DataTypes/fsarray.h>
#include <communication/gen/VMPmessages.h>
#include <communication/Network/message.h>
#include <tr1/memory>
#include <string>


static VMPServer* server = NULL;


void initialize_network(const char* IP, int port){

	if(server != NULL) {
		server->disconnect();
	}
	server = new VMPServer(IP, port);
}

void send_msg(std::unique_ptr<message> ptr, uint32_t target){
	server->send_message(std::move(ptr), target);
}

std::unique_ptr<raw_message> get_message(uint32_t unique_id){
	std::unique_ptr<raw_message> ret = server->get_message(unique_id);
	return ret;
}


void network_shutdown(){
	server->disconnect();

}

void receive_from_network(){

	server->receive_from_network();

}
