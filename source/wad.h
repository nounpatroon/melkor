/*
Copyright (C) 2023  Juan Carlos Peña Gonzalez
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
DESCRIPTION:
    WAD in C89
*/

#ifndef __wad_h__
#define __wad_h__

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef struct {
    char identification[4];
    uint32_t numlumps;
    uint32_t infotableofs;
} wadheader_st;

typedef struct {
    uint32_t filepos;
    uint32_t size;
    char name[8];
} waddirectory_st;

typedef struct {
    FILE *file;
    char *filepath;
    wadheader_st *header;
    char *lumps;
    waddirectory_st *directorys;
} wad_st;

/*Read/Write WAD file*/
void wad_open(wad_st * _self, const char *_path);

/*Clode WAD file*/
void wad_close(wad_st * const _self);

/*Print directorys of WAD file*/
void wad_debug(wad_st * const _self);

/*Insert directory and lump at certain index to WAD file*/
void wad_insert(wad_st * const _self, const uint32_t _index, const char *_name, const char *_data, const size_t _size);

/*Delete directory and lump at certain index to WAD file*/
void wad_erase(wad_st * const _self, const uint32_t _index);

#ifdef __cplusplus
}
#endif

#endif /*__wad_h__*/
