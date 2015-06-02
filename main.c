/* 

Main routine

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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "tlm_reader.h"

int main( int argc, const char* argv[] ){
   
    FILE *fp;
    struct stat st;
    const char * filename;
    size_t ret, log_size;
    void * log_data;
    GENERIC_BLOCK *first_block;

    if(argc != 2){
        error_handler(E_ERROR, "Usage: tlm_reader FILE.TML\n");
        exit(EXIT_FAILURE);
    } 
    filename = argv[1];

    printf("Reading %s\n", filename);

    ret=stat(filename, &st);
    if(ret != EXIT_SUCCESS){
        error_handler(E_ERROR, "Error stat %s\n", filename);
    }
    log_size = st.st_size;

    log_data=malloc(log_size);
    if(log_data==NULL){
        error_handler(E_ERROR, "Error malloc\n");
    }

    fp=fopen(argv[1],"r");
    if(fp==NULL){
        error_handler(E_ERROR, "Error fopen %s\n", filename);
    }
    ret=fread(log_data, 1, log_size, fp);
    if(ret!=log_size){
        error_handler(E_ERROR, "Error wrong number of bytes read, should be %i is %i\n", st.st_size, ret);
    }
    fclose(fp);
    
    first_block = tlm_reader_read(log_data, log_size);

    free(log_data);   

    exit(EXIT_SUCCESS);
    
}
