#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "wad.h"

/*Because realloc sucks when you use memmove*/
char* _wad_realloc(char *_ptr, size_t _size, size_t _newsize) {
    char *new_ptr;
    
    new_ptr = (char*)malloc(_newsize);
    if(new_ptr == NULL) {
        return NULL;
    }

    memmove(new_ptr, _ptr, _size);

    free(_ptr);

    return new_ptr;
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

        self->directorys = _wad_realloc(self->directorys, sizeof(waddirectory_st), sizeof(waddirectory_st) * self->header->numlumps);
        fread(self->directorys, sizeof(waddirectory_st), self->header->numlumps, self->file);
    }

    return self;
}

void wad_close(wad_st *_self) {
    fclose(_self->file);
    
    free(_self->filepath);
    free(_self->header);
    free(_self->lumps);
    free(_self->directorys);
    free(_self);
}
