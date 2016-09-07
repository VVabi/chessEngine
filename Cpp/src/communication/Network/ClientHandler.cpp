/*
 * ClientHandler.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: fabian
 */

#include <communication/Network/ClientHandler.h>
#include <communication/Network/Socket.h>
#include <communication/Network/message.h>
#include <communication/Network/network.h>
#include <iostream>
#include <tr1/memory>
#include <DataTypes/fsarray.h>

ClientHandler::~ClientHandler() {

	// TODO Auto-generated destructor stub
}

void ClientHandler::send_message(std::unique_ptr<message> ptr, uint32_t target){
	uint32_t length = ptr->getSize()+sizeof(uint32_t);
	uint8_t* mem = new uint8_t[length+sizeof(header)];
	header h;
	h.length = length;
	h.targetLocation = target;
	memcpy(mem, &h, sizeof(header));
	ptr->deflate(mem, sizeof(header));

	sock.binary_write(mem, length+sizeof(header));
	delete [] mem;
}

bool ClientHandler::check_pending(){
	return !(sock.get_number_of_pending_bytes() == 0);

}

std::unique_ptr<raw_message> ClientHandler::get_next_message(){
	//TODO: It would be cleaner not to blindly read even if we do not know how much data is actually there.
	//read the incoming size in check_pending?
	if(ClientHandler::check_pending()){
		header h;
		if(!sock.binary_read((uint8_t *) &h, sizeof(header))){
			return std::unique_ptr<raw_message>(nullptr);
		}
		//std::cout << "Got header with size " << h.length << " and target " << h.targetLocation <<std::endl;
		std::unique_ptr<fsarray<uint8_t>> raw = std::unique_ptr<fsarray<uint8_t>>(new fsarray<uint8_t>(h.length));
		if(!sock.binary_read(raw->data, h.length)){
			return std::unique_ptr<raw_message>(nullptr);
		}
		uint32_t unique_id =  *((uint32_t*) raw->data);
		//std::cout << "Id is " << unique_id <<std::endl;
		auto data_arr = std::unique_ptr<raw_message>(new raw_message(unique_id, std::move(raw)));
		return data_arr;
	}
	else{
		return std::unique_ptr<raw_message>(nullptr);
	}

}

void ClientHandler::disconnect(){
	sock.disconnect();

}
