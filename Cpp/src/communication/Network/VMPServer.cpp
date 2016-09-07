/*
 * VMPServer.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: fabian
 */

#include "VMPServer.h"
#include <iostream>
#include <tr1/memory>
#include <DataTypes/fsarray.h>
#include <tr1/memory>


std::unique_ptr<raw_message> VMPServer::get_message(uint32_t unique_id){
	communication_mutex.lock();
	std::unique_ptr<raw_message> msg(nullptr);
	for (auto iterator = VMPList.begin(); iterator !=  VMPList.end(); ++iterator) {
			if((*iterator)->unique_id == unique_id){
				msg = std::move((*iterator));
				VMPList.erase(iterator);
				break;
			}
	}
	communication_mutex.unlock();
	return msg;
}

void VMPServer::send_message(std::unique_ptr<message> ptr, uint32_t target){
	communication_mutex.lock();
	client.send_message(std::move(ptr), target);
	communication_mutex.unlock();
}

void VMPServer::receive_from_network(){
	communication_mutex.lock();
	std::unique_ptr<raw_message> ptr;
	while( (ptr = client.get_next_message())){
		VMPList.push_back(std::move(ptr));
	}
	communication_mutex.unlock();
}

void VMPServer::disconnect(){
	client.disconnect();
}
