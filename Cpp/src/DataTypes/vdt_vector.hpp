

#ifndef FS_VDT_VECTOR_H
#define FS_VDT_VECTOR_H
#include <stdint.h>
#include <stddef.h>
#include <iostream>
#include <string.h>

template<typename T> class vdt_vector{
	public:
		uint32_t length;
		uint32_t allocated_length;
		T* data;
		vdt_vector(){
			length = 0;
			data = 0;
			allocated_length = 0;
		}
		vdt_vector(uint32_t n){
			length = 0;
			allocated_length = n;
			if(allocated_length > 0){
				data = new T[allocated_length];
			}
			else{
				data = 0;
			}
		}

		vdt_vector(T* buffer, uint32_t len){
			length = 0;
			allocated_length = len;
			data = buffer;
		}

		T &operator[](uint32_t i){
			if(i > length){
				std::cout << "vector index out ouf bounds" << std::endl;
				while(1){
					//add fatal
				}
			}
			return data[i];

		}

		void add(T* element){

			if(length > allocated_length){
				//add fatal
			}
			if(length >= allocated_length){
				uint32_t new_length = 2*allocated_length;
				if(allocated_length == 0){
					new_length = 1;
				}
				T* buffer = new T[new_length];
				allocated_length = new_length;
				memcpy(buffer, data, length*sizeof(T));
				delete[] data;
				data = buffer;
			}
			data[length] = *element;
			length++;
		}

		T pop(){
			length = (length > 0 ? length-1:length);
			return data[length];
		}

		void free_array(){
			if(data != NULL){
				delete[] data;
				data = NULL;
				length = 0;
				allocated_length = 0;
			}
		}

};
#endif
