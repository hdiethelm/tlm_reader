/* 

TLM reader library

Copyright (C) 2015 Hannes Diethelm

This file is part of tlm_reader

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "tlm_reader.h"

#define DEBUG

void _error_handler(int line, const char *file, char* format, ...){

    va_list argList;
    
    printf("%s:%i: ", file, line);
    
    va_start(argList, format);
    vprintf(format, argList);
    va_end(argList);

    exit(EXIT_FAILURE);
    
}

GENERIC_BLOCK *tlm_reader_read(void * log_data, size_t log_size){
	GENERIC_BLOCK *first_block, *work_block;
	void *data_ptr;
	bool main_header;
	
	first_block = malloc(sizeof(GENERIC_BLOCK));
	first_block->prev=NULL;
	first_block->next=NULL;

	data_ptr = log_data;
	work_block = first_block;
	main_header = true;

	while(data_ptr - log_data < log_size){
		work_block->data = data_ptr;

		if(work_block->data->common.timestamp == TIMESTAMP_HEADER){
			if(main_header){
				work_block->type = HEADER_MAIN;
				main_header = false;
				printf("Header Main nr=0x%02x type=0x%02x name=%s\n", work_block->data->header_main.model_number, work_block->data->header_main.model_type, work_block->data->header_main.model_name);
			}else{
				work_block->type = HEADER_AUX;
				if(work_block->data->header_aux.sensor_type[0] != work_block->data->header_aux.sensor_type[1]){
					printf("Expecting sensor_type[0] == sensor_type[1]!\n");
				}
				/*Seams that this is the identification of the last aux header block */
				if(work_block->data->header_aux.sensor_type[0] == HEADER_AUX_TYPE_LAST){
					main_header = true;
				}
				printf("Header Aux sensor=0x%02x 0x%02x\n", work_block->data->header_aux.sensor_type[0], work_block->data->header_aux.sensor_type[1]);
			}

			data_ptr=data_ptr + SIZE_HEADER;
		}else{
			work_block->type=work_block->data->common.type[0];
			printf("Data timestamp=%06i type=0x%02x\n", work_block->data->common.timestamp, work_block->type);

			data_ptr=data_ptr + SIZE_DATA;			
		}

		work_block->next = malloc(sizeof(GENERIC_BLOCK));
		work_block->next->prev = work_block;
		work_block->next = work_block;
	}

	return first_block;
}
