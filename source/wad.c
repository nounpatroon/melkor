

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "wad.h"

int wad_open(wad_st **self, const char *filepath) {
    (*self) = (wad_st*)malloc(sizeof(wad_st));

    (*self)->filepath = _wad_strdup(filepath);
    (*self)->file = fopen(filepath, "rb+");

    (*self)->header.numlumps = 0;
    (*self)->header.identification[0] = 'P';
    (*self)->header.identification[1] = 'W';
    (*self)->header.identification[2] = 'A';
    (*self)->header.identification[3] = 'D';
    (*self)->header.infotableofs = 12;

    /*Read*/
    if((*self)->file) {
        fread(&(*self)->header, 12, 1, (*self)->file);
    }
    /*Write*/
    else {
        (*self)->file = fopen(filepath, "wb+");
        fwrite(&(*self)->header, 12, 1, (*self)->file);
    }

    return true;
}

int wad_close(wad_st **self) {
    fclose((*self)->file);
    free((*self)->filepath);
    free((*self));
    
    (*self) = NULL;

    return true;
}

char* _wad_strdup(const char *s) {
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p == NULL) {
        return NULL;
    }
    memcpy(p, s, len + 1);
    return p;
}
