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

wad_st* wad_open(const char *_path);
void wad_close(wad_st *_self);

#ifdef __cplusplus
}
#endif

#endif /*__wad_h__*/
