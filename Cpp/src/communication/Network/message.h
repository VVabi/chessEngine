/*
 * message.h
 *
 *  Created on: Sep 11, 2015
 *      Author: fabian
 */
#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdint.h>
#include <DataTypes/fsarray.h>
#include <tr1/memory>
#include <iostream>
class message {
	public:
		virtual uint32_t getUniqueId()=0;
		virtual void deflate(uint8_t* mem, int offset_correction)=0;
		virtual uint32_t getSize()=0;
		virtual void dispose()=0;
		virtual~ message(){};

};

class raw_message {
public:
	uint32_t unique_id;
    std::unique_ptr<fsarray<uint8_t>> raw_data;

    raw_message(uint32_t id, std::unique_ptr<fsarray<uint8_t>> arr){
    	raw_data = std::move(arr);
    	unique_id = id;

    }

    ~raw_message(){
    	raw_data->free_array();
    }
    raw_message( const raw_message& other ) = delete; // non construction-copyable
       raw_message& operator=( const raw_message& ) = delete; // non copyable

};




#endif /* MESSAGE_H_ */
