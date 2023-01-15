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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#include "wad.h"

/*Because realloc sucks when you use memmove*/
char* _wad_realloc(char * const _ptr, const size_t _size, const size_t _newsize) {
    char *new_ptr;
    
    new_ptr = (char*)malloc(_newsize);
    if(new_ptr == NULL) {
        return NULL;
    }

    memmove(new_ptr, _ptr, _size);

    free(_ptr);

    return new_ptr;
}

/*Get lump index position*/
size_t _wad_index_lump(wad_st * const _self, const uint32_t _index) {
    return _self->directorys[_index].filepos - sizeof(wadheader_st);
}

wad_st* wad_open(const char *_path) {
    wad_st *self;

    self = (wad_st*)malloc(sizeof(wad_st));
    memset(self, 0, sizeof(wad_st));

    self->filepath = strdup(_path);
    self->header = (wadheader_st*)malloc(sizeof(wadheader_st));
    self->lumps = (char*)malloc(sizeof(char));
    self->directorys = (waddirectory_st*)malloc(sizeof(waddirectory_st));

    self->file = fopen(_path, "rb+");
    if(self->file == NULL) {
        self->file = fopen(_path, "wb+");

        strcpy(self->header->identification, "PWAD");
        self->header->infotableofs = sizeof(wadheader_st);
        self->header->numlumps = 0;

        fwrite(self->header, sizeof(wadheader_st), 1, self->file);
    } else {
        fseek(self->file, 0, SEEK_SET);
        fread(self->header, sizeof(wadheader_st), 1, self->file);

        self->lumps = _wad_realloc(self->lumps, sizeof(char), self->header->infotableofs - sizeof(wadheader_st));
        fread(self->lumps, self->header->infotableofs - sizeof(wadheader_st), 1, self->file);

        self->directorys = (waddirectory_st*)_wad_realloc((char*)self->directorys, sizeof(waddirectory_st), sizeof(waddirectory_st) * self->header->numlumps);
        fread(self->directorys, sizeof(waddirectory_st), self->header->numlumps, self->file);
    }

    return self;
}

void wad_close(wad_st * const _self) {
    fclose(_self->file);
    
    free(_self->filepath);
    free(_self->header);
    free(_self->lumps);
    free(_self->directorys);
    free(_self);
}

void wad_debug(wad_st * const _self) {
    uint32_t i;
    printf("\r\n--WAD DEBUG--\r\n");
    printf("%-8.8s %-8.8s %-8.8s\r\n", "NAME", "OFFSET", "SIZE");
    for(i = 0;i < _self->header->numlumps;i++) {
        printf("%-8.8s %-8d %-8d\r\n", _self->directorys[i].name, _self->directorys[i].filepos, _self->directorys[i].size);
    }

    printf("\r\n--WAD DEBUG--\r\n");
}

void wad_insert(wad_st * const _self, const uint32_t _index, const char *_name, const char *_data, const size_t _size) {
    uint32_t i;
    waddirectory_st newdirectory;
    
    if(_index > _self->header->numlumps) {
        return;
    }

    sprintf(newdirectory.name, "%-8.8s" /*IGNORE THE WARNING*/ , _name);
    for(i = 0;i < 8;i++) {newdirectory.name[i] = toupper(newdirectory.name[i]);}
    newdirectory.size = _size;
    newdirectory.filepos = _self->directorys[_index].filepos;

    /*memmove(array + index + 1, array + index, sizeof(array_type) * (total - index));*/
    _self->directorys = (waddirectory_st*)_wad_realloc((char*)_self->directorys, sizeof(waddirectory_st) * _self->header->numlumps, sizeof(waddirectory_st) * (_self->header->numlumps+1));
    memmove(_self->directorys + _index + 1, _self->directorys + _index, sizeof(waddirectory_st) * (_self->header->numlumps - _index));
    _self->directorys[_index] = newdirectory;
    for(i = _index;i < (_self->header->numlumps+1);i++) {
        if(i == 0) {
            _self->directorys[i].filepos = sizeof(wadheader_st);
        } else {
            _self->directorys[i].filepos = _self->directorys[i-1].filepos + _self->directorys[i-1].size;
        }
    }

    _self->lumps = _wad_realloc(_self->lumps, (_self->header->infotableofs - sizeof(wadheader_st)), sizeof(char) * (_self->header->infotableofs - sizeof(wadheader_st) + _size));
    memmove(_self->lumps + _wad_index_lump(_self, _index) + _size, _self->lumps + _wad_index_lump(_self, _index), sizeof(char) * ((_self->header->infotableofs - sizeof(wadheader_st)) - _wad_index_lump(_self, _index)));
    for(i = 0;i < _size;i++) {
        _self->lumps[_wad_index_lump(_self, _index) + i] = _data[i];
    }

    _self->header->numlumps += 1;
    _self->header->infotableofs += _size;

    fseek(_self->file, 0, SEEK_SET);
    fwrite(_self->header, sizeof(wadheader_st), 1, _self->file);
    fwrite(_self->lumps, sizeof(char), (_self->header->infotableofs - sizeof(wadheader_st)), _self->file);
    fwrite(_self->directorys, sizeof(waddirectory_st), _self->header->numlumps, _self->file);
}
