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

void wad_open(wad_st *_self, const char *_path) {
    _self->filepath = strdup(_path);
    _self->header = (wadheader_st*)malloc(sizeof(wadheader_st));
    _self->lumps = (char*)malloc(sizeof(char));
    _self->directorys = (waddirectory_st*)malloc(sizeof(waddirectory_st));

    _self->file = fopen(_path, "rb+");
    if(_self->file == NULL) {
        _self->file = fopen(_path, "wb+");

        strcpy(_self->header->identification, "PWAD");
        _self->header->infotableofs = sizeof(wadheader_st);
        _self->header->numlumps = 0;

        fwrite(_self->header, sizeof(wadheader_st), 1, _self->file);
    } else {
        fseek(_self->file, 0, SEEK_SET);
        fread(_self->header, sizeof(wadheader_st), 1, _self->file);

        _self->lumps = _wad_realloc(_self->lumps, sizeof(char), _self->header->infotableofs - sizeof(wadheader_st));
        fread(_self->lumps, _self->header->infotableofs - sizeof(wadheader_st), 1, _self->file);

        _self->directorys = (waddirectory_st*)_wad_realloc((char*)_self->directorys, sizeof(waddirectory_st), sizeof(waddirectory_st) * _self->header->numlumps);
        fread(_self->directorys, sizeof(waddirectory_st), _self->header->numlumps, _self->file);
    }
}

void wad_close(wad_st * const _self) {
    fclose(_self->file);

    free(_self->filepath);
    free(_self->header);
    free(_self->lumps);
    free(_self->directorys);
}

void wad_debug(wad_st * const _self) {
    uint32_t i;
    printf("%-4.4s %-8d %-8d\t\n", _self->header->identification, _self->header->numlumps, _self->header->infotableofs);
    printf("          %-8.8s %-8.8s %-8.8s\r\n", "NAME", "OFFSET", "SIZE");
    for(i = 0;i < _self->header->numlumps;i++) {
        printf("%8d: %-8.8s %-8d %-8d\r\n", i, _self->directorys[i].name, _self->directorys[i].filepos, _self->directorys[i].size);
    }
    printf("\r\n");
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
    memmove(_self->directorys + _index + 1,\
            _self->directorys + _index,\
            sizeof(waddirectory_st) * (_self->header->numlumps - _index));
    _self->directorys[_index] = newdirectory;
    for(i = _index;i < (_self->header->numlumps+1);i++) {
        if(i == 0) {
            _self->directorys[i].filepos = sizeof(wadheader_st);
        } else {
            _self->directorys[i].filepos = _self->directorys[i-1].filepos + _self->directorys[i-1].size;
        }
    }

    _self->lumps = _wad_realloc(_self->lumps, (_self->header->infotableofs - sizeof(wadheader_st)), sizeof(char) * (_self->header->infotableofs - sizeof(wadheader_st) + _size));
    memmove(_self->lumps + _wad_index_lump(_self, _index) + _size,\
            _self->lumps + _wad_index_lump(_self, _index),\
            sizeof(char) * ((_self->header->infotableofs - sizeof(wadheader_st)) - _wad_index_lump(_self, _index)));
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

void wad_erase(wad_st * const _self, const uint32_t _index) {
    uint32_t index_size,i,start,end;
    
    if(_index >= _self->header->numlumps) {
        return;
    }

    index_size = _self->directorys[_index].size;
    
    /*lumps*/
    if(_self->directorys[_index].size != 0) {
        start = _wad_index_lump(_self, _index);
        end = start + _self->directorys[_index].size;
        memmove(_self->lumps + start,\
                _self->lumps + end,\
                (_self->header->infotableofs - sizeof(wadheader_st)) - end);
        if(_index == 0) {
            _self->lumps = _wad_realloc(_self->lumps, (_self->header->infotableofs - sizeof(wadheader_st)), (_self->header->infotableofs - sizeof(wadheader_st)) - start);
        } else {
            _self->lumps = _wad_realloc(_self->lumps, (_self->header->infotableofs - sizeof(wadheader_st)), (_self->header->infotableofs - sizeof(wadheader_st)) - end);
        }
    }

    /*directorys*/
    memmove(\
        &_self->directorys[_index],\
        &_self->directorys[_index + 1],\
        ((_self->header->numlumps) - _index) * sizeof(waddirectory_st)\
    );
    _self->directorys = (waddirectory_st*)realloc(_self->directorys,  (sizeof(waddirectory_st) * _self->header->numlumps) - sizeof(waddirectory_st));
    for(i = _index;i < (_self->header->numlumps-1);i++) {
        if(i == 0) {
            _self->directorys[i].filepos = 12;
        } else {
            _self->directorys[i].filepos = _self->directorys[i-1].filepos + _self->directorys[i-1].size;
        }
    }

    /*write to file*/
    _self->header->numlumps -= 1;
    _self->header->infotableofs -= index_size;

    fseek(_self->file, 0, SEEK_SET);
    fwrite(_self->header, sizeof(wadheader_st), 1, _self->file);
    fwrite(_self->lumps, sizeof(char), (_self->header->infotableofs - sizeof(wadheader_st)), _self->file);
    fwrite(_self->directorys, sizeof(waddirectory_st), _self->header->numlumps, _self->file);
}
