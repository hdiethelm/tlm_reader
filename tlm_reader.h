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
#define SIZE_HEADER  36
#define SIZE_DATA    20

/* Datatype of Block */
enum _data_type{ 
    HEADER_MAIN     = 0x100,
    HEADER_AUX      = 0x101,
    CURRENT         = 0x03,
    POWERBOX        = 0x0A,
    AIRSPEED        = 0x11,
    ATLITUDE        = 0x12,
    ACCELLERATION   = 0x14,
    JET_CAT         = 0x15,
    GPS1            = 0x16, 
    GPS2            = 0x17,
    VARIO           = 0x40,
    RPM_TEMP_VOLT   = 0x7E,
    RX_STAT         = 0x7F
};
typedef enum _data_type DATA_TYPE;

/**
 * Different data blocks----------------
 */
/* Not really a block, just used to access timestamp and type fields */
struct _block_common {
    uint32_t timestamp;
    uint8_t type[2];
};
typedef struct _block_common BLOCK_COMMON;

/* Model Type */
enum _model_type{ 
    MODEL_FIXED =       0x00,
    MODEL_HELICOPTER =  0x01,
    MODEL_GLIDER =  0x02
};
typedef enum _model_type MODEL_TYPE;

/* Bind Info */
enum _bind_info{ 
    BIND_DSM2_6000 =   0x01,
    BIND_DSM2_8000 =   0x02,
    BIND_DSMX_6000 =   0x03,
    BIND_DSMX_8000 =   0x04,
};
typedef enum _bind_info BIND_INFO;

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

/* Enabled Info */
enum _ena_info{
    ENA_VOLT            = 0x01,
    ENA_TEMP            = 0x02,
    ENA_CURRENT         = 0x03,
    ENA_GPS             = 0x16, 
    ENA_LAST            = 0x17, 
    ENA_ATTITUDE        = 0x18,
    ENA_ACCELLERATION   = 0x20,
    ENA_RPM             = 0x7E,
};
typedef enum _ena_info ENA_INFO;

struct _block_header_aux {
    uint32_t timestamp;
    uint8_t ena_info[2];
    uint8_t unknown1[16];
    uint8_t unknown2[14];
};
typedef struct _block_header_aux BLOCK_HEADER_AUX;

/* Data Blocks */
struct _block_current {
    uint32_t timestamp;
    uint8_t type;    
};
typedef struct _block_current BLOCK_CURRENT;

struct _block_powerbox {
    uint32_t timestamp;
    uint8_t type;    
};
typedef struct _block_powerbox BLOCK_POWERBOX;

struct _block_airspeed {
    uint32_t timestamp;
    uint8_t type;    
};
typedef struct _block_airspeed BLOCK_AIRSPEED;

struct _block_altitude {
    uint32_t timestamp;
    uint8_t type;    
};
typedef struct _block_altitude BLOCK_ALTITUDE;

struct _block_acceleration {
    uint32_t timestamp;
    uint8_t type;    
};
typedef struct _block_acceleration BLOCK_ACCELLERATION;

struct _block_jet_cat {
    uint32_t timestamp;
    uint8_t type;    
};
typedef struct _block_jet_cat BLOCK_JET_CAT;

struct _block_gps1 {
    uint32_t timestamp;
    uint8_t type;    
};
typedef struct _block_gps1 BLOCK_GPS1;

struct _block_gps2 {
    uint32_t timestamp;
    uint8_t type;    
};
typedef struct _block_gps2 BLOCK_GPS2;

struct _block_vario {
    uint32_t timestamp;
    uint8_t type;    
};
typedef struct _block_vario BLOCK_VARIO;

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
    BLOCK_COMMON        common;
    BLOCK_HEADER_MAIN   header_main;
    BLOCK_HEADER_AUX    header_aux;
    BLOCK_CURRENT       current;
    BLOCK_POWERBOX      powerbox;
    BLOCK_AIRSPEED      airspeed;
    BLOCK_ALTITUDE      altitude;
    BLOCK_ACCELLERATION acceleration;
    BLOCK_JET_CAT       jet_cat;
    BLOCK_GPS1          gps1;
    BLOCK_GPS2          gps2;
    BLOCK_VARIO         vario;
    BLOCK_RPM_TEMP_VOLT rpm_volt_temp;
    BLOCK_RX_STAT       rx_stat;
};
typedef union _data_block DATA_BLOCK;

union _decoded_block{/*
    DECODED_HEADER_MAIN   header_main;
    DECODED_HEADER_AUX    header_aux;
    DECODED_CURRENT       current;
    DECODED_POWERBOX      powerbox;
    DECODED_AIRSPEED      airspeed;
    DECODED_ALTITUDE      altitude;
    DECODED_ACCELLERATION acceleration;
    DECODED_JET_CAT       jet_cat;
    DECODED_GPS1          gps1;
    DECODED_GPS2          gps2;
    DECODED_VARIO         vario;
    DECODED_RPM_TEMP_VOLT rpm_volt_temp;
    DECODED_RX_STAT       rx_stat;*/
};
typedef union _decoded_block DECODED_BLOCK;

/**
 * Block structure
 */
struct _generic_block {
    DATA_TYPE       type;
    DATA_BLOCK      data;
    DECODED_BLOCK   decoded;
    
    /* Linked list for blocks */
    struct _generic_block *prev;
    struct _generic_block *next;
};
typedef struct _generic_block GENERIC_BLOCK;

GENERIC_BLOCK *tlm_reader_read(void * log_data, size_t log_size);

void tlm_reader_decode_header(GENERIC_BLOCK * block);
void tlm_reader_decode_data(GENERIC_BLOCK * block);

/* Where to print errors */
#define ERROR_FP stdout
/*#define ERROR_FP stderr*/

/* Type of error */
enum _error_type{ 
    E_ERROR     = 0x00,
    E_WARNING   = 0x01
};
typedef enum _error_type ERROR_TYPE;

#define error_handler(err, ...) _error_handler((err),__LINE__, __FILE__, __VA_ARGS__)
    
void _error_handler(ERROR_TYPE err, int line, const char *file, char* format, ...);

#endif /* TLM_READER_H_ */
