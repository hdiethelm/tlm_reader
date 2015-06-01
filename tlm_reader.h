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

#ifndef TLM_READER_H_
#define TLM_READER_H_

#include <stdint.h>

#define TIMESTAMP_HEADER 0xFFFFFFFF
#define SIZE_HEADER 36
#define SIZE_DATA	20

#define HEADER_AUX_TYPE_LAST 0x17

/* Datatype of Block */
enum _datatype{ 
	HEADER_MAIN		= 0x100,
	HEADER_AUX 		= 0x101,
	CURRENT			= 0x03,
	GPS 			= 0x16, 
	SPEED 			= 0x17,
	ATTITUDE		= 0x18,
	ACCELLERATION 	= 0x20,
	RPM_TEMP_VOLT 	= 0x7E,
	RX_STAT 		= 0x7F
};
typedef enum _datatype DATATYPE;

/**
 * Different data blocks----------------
 */
/* Not really a block, just used to access timestamp */
struct _block_common {
	uint32_t timestamp;
	uint8_t type[2];
};
typedef struct _block_common BLOCK_COMMON;

struct _block_header_main {
	uint32_t timestamp;
	uint8_t model_number;
	uint8_t model_type;
	uint8_t bind_info;
	uint8_t unknown1[5];
	uint8_t model_name[16];
	uint8_t unknown2[4];
};
typedef struct _block_header_main BLOCK_HEADER_MAIN;

struct _block_header_aux {
	uint32_t timestamp;
	uint8_t sensor_type[2];
	uint8_t unknown1[16];
	uint8_t unknown2[14];
};
typedef struct _block_header_aux BLOCK_HEADER_AUX;

struct _block_current {
	uint32_t timestamp;
	uint8_t type;	
};
typedef struct _block_current BLOCK_CURRENT;

struct _block_gps {
	uint32_t timestamp;
	uint8_t type;	
};
typedef struct _block_gps BLOCK_GPS;

struct _block_speed {
	uint32_t timestamp;
	uint8_t type;	
};
typedef struct _block_speed BLOCK_SPEED;

struct _block_attitude {
	uint32_t timestamp;
	uint8_t type;	
};
typedef struct _block_attitude BLOCK_ATTITUDE;

struct _block_accelleration {
	uint32_t timestamp;
	uint8_t type;	
};
typedef struct _block_accelleration BLOCK_ACCELLERATION;

struct _block_rpm_volt_temp {
	uint32_t timestamp;
	uint8_t type;
};
typedef struct _block_rpm_volt_temp BLOCK_RPM_TEMP_VOLT;

struct _block_rx_stat {
	uint32_t timestamp;
	uint8_t type;
};
typedef struct _block_rx_stat BLOCK_RX_STAT;

/**
 * Union for all data block types--------
 */
union _data_block{
	BLOCK_COMMON		common;
	BLOCK_HEADER_MAIN	header_main;
	BLOCK_HEADER_AUX	header_aux;
	BLOCK_CURRENT 		current;
	BLOCK_GPS 			gps;
	BLOCK_SPEED 		speed;
	BLOCK_ATTITUDE 		attiture;
	BLOCK_ACCELLERATION accelleration;
	BLOCK_RPM_TEMP_VOLT rpm_volt_temp;
	BLOCK_RX_STAT		rx_stat;
};
typedef union _data_block DATA_BLOCK;

/**
 * Block structure
 */
struct _generic_block {
    DATATYPE type;
	DATA_BLOCK *data;
	
	/* Linked list for blocks */
	struct _generic_block *prev;
	struct _generic_block *next;
};
typedef struct _generic_block GENERIC_BLOCK;

GENERIC_BLOCK *tlm_reader_read(void * log_data, size_t log_size);

#define error_handler(...) _error_handler(__LINE__, __FILE__, __VA_ARGS__)
    
void _error_handler(int line, const char *file, char* format, ...);

#endif /* TLM_READER_H_ */
