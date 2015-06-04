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

void _error_handler(ERROR_TYPE err, int line, const char *file, char* format, ...){

    va_list argList;
    
    if(err == E_ERROR){
        fprintf(ERROR_FP, "Error: %s:%i: ", file, line);
    }else if(err == E_WARNING){
        fprintf(ERROR_FP, "Warning: %s:%i: ", file, line);
    }else{
        fprintf(ERROR_FP, "Unknown error code: %s:%i: ", file, line);
    }
    
    va_start(argList, format);
    vfprintf(ERROR_FP, format, argList);
    va_end(argList);

    if(err == E_ERROR){
        exit(EXIT_FAILURE);
    }
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
            work_block->data_size = SIZE_HEADER;

            if(main_header){
                work_block->type = HEADER_MAIN;
                main_header = false;
            }else{
                work_block->type = HEADER_AUX;
                if(work_block->data.common.type[0] != work_block->data.common.type[1]){
                    error_handler(E_WARNING, "Expecting type[0] == type[1]!\n");
                }
                /*Seams that this is the identification of the last aux header block */
                if(work_block->data.common.type[0] == ENA_LAST){
                    main_header = true;
                }
                
            }
            
            tlm_reader_decode_header(work_block);
        }else{
            memcpy(&work_block->data, data, SIZE_DATA);
            work_block->data_size=SIZE_DATA;

            work_block->type=work_block->data.common.type[0];

            tlm_reader_decode_data(work_block);       
        }

        data_ptr=data_ptr + work_block->data_size; 

        work_block->next = malloc(sizeof(GENERIC_BLOCK));
        work_block->next->prev = work_block;
        work_block->next = work_block;
    }

    return first_block;
}

void tlm_reader_decode_header(GENERIC_BLOCK * block){
#ifdef DEBUG
    /*tlm_print_raw(block);*/
#endif

    switch(block->type) {
        case HEADER_MAIN:   
#ifdef DEBUG  
            printf("Header Main: model_number=0x%02X model_type=0x%02X model_name=%s\n", 
                block->data.header_main.model_number, block->data.header_main.model_type, block->data.header_main.model_name);
#endif
            break;
        case HEADER_AUX: 
#ifdef DEBUG  
            printf("Header Aux: ena_info=0x%02X 0x%02X\n", 
                block->data.header_aux.ena_info[0], block->data.header_aux.ena_info[1]);
#endif
            break;
        default: 
            error_handler(E_WARNING, "block->type = 0x%02X unknown!\n", block->type);
            break;
    }

}

void tlm_reader_decode_data(GENERIC_BLOCK * block){
#ifdef DEBUG
    /*tlm_print_raw(block);*/
    printf("Data: timestamp=%06i type=0x%02X:", block->data.common.timestamp, block->type);
#endif

    switch(block->type) {
        case CURRENT:   
#ifdef DEBUG
            printf("CURRENT");
#endif
            block->decoded.current.current = (float)swap_int16(block->data.current.current) * DECODE_CURRENT_FACTOR;

#ifdef DEBUG            
            printf(" current=0x%04X=%.1fA", 0xFFFF & swap_int16(block->data.current.current), block->decoded.current.current);
#endif
            break;
        case POWERBOX:   
#ifdef DEBUG
            printf("POWERBOX");
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
        case RPM_TEMP_VOLT_TM1000:
        case RPM_TEMP_VOLT_TM1100:
#ifdef DEBUG
            printf("RPM_TEMP_VOLT");
#endif

            block->decoded.rpm_volt_temp.rpm = (float)swap_uint16(block->data.rpm_volt_temp.rpm);
            block->decoded.rpm_volt_temp.volt = (float)swap_uint16(block->data.rpm_volt_temp.volt) / 100.0;
            block->decoded.rpm_volt_temp.temp = (float)swap_uint16(block->data.rpm_volt_temp.temp);

            /* Farenheit to Celsius */
            block->decoded.rpm_volt_temp.temp = ( block->decoded.rpm_volt_temp.temp - 32.0 ) *(5.0/9.0);

#ifdef DEBUG
            printf(" rpm=0x%04X=%i", 0xFFFF & swap_uint16(block->data.rpm_volt_temp.rpm), block->decoded.rpm_volt_temp.rpm);
            printf(" volt=0x%04X=%.1fV", 0xFFFF & swap_uint16(block->data.rpm_volt_temp.volt), block->decoded.rpm_volt_temp.volt);
            printf(" temp=0x%04X=%.1fC", 0xFFFF & swap_uint16(block->data.rpm_volt_temp.temp), block->decoded.rpm_volt_temp.temp);
#endif

            break;
        case RX_STAT_TM1000: 
        case RX_STAT_TM1100: 
#ifdef DEBUG
            printf("RX_STAT");
#endif

            block->decoded.rx_stat.a_fade = swap_uint16(block->data.rx_stat.a_fade);
            block->decoded.rx_stat.b_fade = swap_uint16(block->data.rx_stat.b_fade);
            block->decoded.rx_stat.l_fade = swap_uint16(block->data.rx_stat.l_fade);
            block->decoded.rx_stat.r_fade = swap_uint16(block->data.rx_stat.r_fade);

            block->decoded.rx_stat.frame_loss = swap_uint16(block->data.rx_stat.frame_loss);
            block->decoded.rx_stat.hold = swap_uint16(block->data.rx_stat.hold);

            block->decoded.rx_stat.rx_volt = (float)swap_uint16(block->data.rx_stat.rx_volt) / 100.0;

#ifdef DEBUG
            printf(" a_fade=0x%04X=%i", 0xFFFF & swap_uint16(block->data.rx_stat.a_fade), block->decoded.rx_stat.a_fade);
            printf(" b_fade=0x%04X=%i", 0xFFFF & swap_uint16(block->data.rx_stat.b_fade), block->decoded.rx_stat.b_fade);
            printf(" l_fade=0x%04X=%i", 0xFFFF & swap_uint16(block->data.rx_stat.l_fade), block->decoded.rx_stat.l_fade);
            printf(" r_fade=0x%04X=%i", 0xFFFF & swap_uint16(block->data.rx_stat.r_fade), block->decoded.rx_stat.r_fade);

            printf(" frame_loss=0x%04X=%i", 0xFFFF & swap_uint16(block->data.rx_stat.frame_loss), block->decoded.rx_stat.frame_loss);
            printf(" hold=0x%04X=%i", 0xFFFF & swap_uint16(block->data.rx_stat.hold), block->decoded.rx_stat.hold);

            printf(" rx_volt=0x%04X=%.1fV", 0xFFFF & swap_uint16(block->data.rx_stat.rx_volt), block->decoded.rx_stat.rx_volt);
#endif

            break;
        default: 
            error_handler(E_WARNING, "block->type = 0x%02X unknown!\n", block->type);
            break;
    }
    
#ifdef DEBUG
            printf("\n");
#endif

}

void tlm_print_raw(GENERIC_BLOCK * block){
    uint8_t *ptr;
    size_t i;

    ptr=(uint8_t *)&block->data;
    for(i=0 ; i < block->data_size ; i++){
        printf("0x%02X ", ptr[i]);
    }
    printf("\n");
}

/* Byte swap unsigned short */
uint16_t swap_uint16( uint16_t val ) 
{
    return (val << 8) | (val >> 8 );
}

/* Byte swap short */
int16_t swap_int16( int16_t val ) 
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

/* Byte swap unsigned int */
uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}

/* Byte swap int */
int32_t swap_int32( int32_t val )
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

