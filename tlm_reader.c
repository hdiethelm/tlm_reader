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
#include <string.h>

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
    DATA_BLOCK *data;
    void *data_ptr;
    bool main_header;
    
    first_block = malloc(sizeof(GENERIC_BLOCK));
    first_block->prev=NULL;
    first_block->next=NULL;

    data_ptr = log_data;
    work_block = first_block;
    main_header = true;

    while(data_ptr - log_data < log_size){
        data = data_ptr;

        if(data->common.timestamp == TIMESTAMP_HEADER){
            memcpy(&work_block->data, data, SIZE_HEADER);

            if(main_header){
                work_block->type = HEADER_MAIN;
                main_header = false;
            }else{
                work_block->type = HEADER_AUX;
                if(work_block->data.common.type[0] != work_block->data.common.type[1]){
                    error_handler("Expecting type[0] == type[1]!\n");
                }
                /*Seams that this is the identification of the last aux header block */
                if(work_block->data.common.type[0] == ENA_LAST){
                    main_header = true;
                }
                
            }
            
            tlm_reader_decode_header(work_block);

            data_ptr=data_ptr + SIZE_HEADER;
        }else{
            memcpy(&work_block->data, data, SIZE_DATA);

            work_block->type=work_block->data.common.type[0];

            tlm_reader_decode_data(work_block);

            data_ptr=data_ptr + SIZE_DATA;            
        }

        work_block->next = malloc(sizeof(GENERIC_BLOCK));
        work_block->next->prev = work_block;
        work_block->next = work_block;
    }

    return first_block;
}

void tlm_reader_decode_header(GENERIC_BLOCK * block){
    switch(block->type) {
        case HEADER_MAIN:   
#ifdef DEBUG  
            printf("Header Main: model_number=0x%02x model_type=0x%02x model_name=%s\n", 
                block->data.header_main.model_number, block->data.header_main.model_type, block->data.header_main.model_name);
#endif
            break;
        case HEADER_AUX: 
#ifdef DEBUG  
            printf("Header Aux: ena_info=0x%02x 0x%02x\n", 
                block->data.header_aux.ena_info[0], block->data.header_aux.ena_info[1]);
#endif
            break;
        default: 
            error_handler("block->type = 0x%02x unknown!\n", block->type);
            break;
    }

}

void tlm_reader_decode_data(GENERIC_BLOCK * block){
#ifdef DEBUG
    printf("Data: timestamp=%06i type=0x%02x:", block->data.common.timestamp, block->type);
#endif

    switch(block->type) {
        case CURRENT:   
#ifdef DEBUG
            printf("CURRENT");
#endif
           
            break;
        case AIRSPEED: 
#ifdef DEBUG
            printf("AIRSPEED");
#endif

            break;
        case ATLITUDE: 
#ifdef DEBUG
            printf("ATLITUDE");
#endif

            break;
        case ACCELLERATION: 
#ifdef DEBUG
            printf("ACCELLERATION");
#endif

            break;
        case JET_CAT: 
#ifdef DEBUG
            printf("JET_CAT");
#endif

            break;
        case GPS1: 
#ifdef DEBUG
            printf("GPS1");
#endif

            break;
        case GPS2: 
#ifdef DEBUG
            printf("GPS2");
#endif

            break;
        case VARIO: 
#ifdef DEBUG
            printf("VARIO");
#endif

            break;
        case RPM_TEMP_VOLT: 
#ifdef DEBUG
            printf("RPM_TEMP_VOLT");
#endif

            break;
        case RX_STAT: 
#ifdef DEBUG
            printf("RX_STAT");
#endif

            break;
        default: 
            error_handler("block->type = 0x%02x unknown!\n", block->type);
            break;
    }
    
#ifdef DEBUG
            printf("\n");
#endif

}

