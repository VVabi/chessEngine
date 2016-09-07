/*
 * network.h
 *
 *  Created on: Sep 20, 2015
 *      Author: fabian
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <communication/Network/message.h>
#include <DataTypes/fsarray.h>
#include <tr1/memory>

void initialize_network(const char* IP, int port);
void send_msg(std::unique_ptr<message> ptr, uint32_t target);
std::unique_ptr<raw_message> get_message(uint32_t unique_id);
void receive_from_network();
void network_shutdown();

struct header {
	uint32_t length;
	uint32_t targetLocation;
}__attribute__((packed));

//TODO: these functions should be in another file
template <class T> std::unique_ptr<T> VMP_create(std::unique_ptr<raw_message> arr, bool do_delete=true) {

	std::unique_ptr<T> ret = std::unique_ptr<T>(new T(arr->raw_data->data, 4)); //TODO: can we efficiently replace this with a factory?
	return ret;
}

template<class T> std::unique_ptr<T> VMP_receive(){
	std::unique_ptr<raw_message> raw = get_message(T::unique_id);

	if(raw) {
		return VMP_create<T>(std::move(raw));
	} else {
		return std::unique_ptr<T>(nullptr);
	}


}

#endif /* NETWORK_H_ */
