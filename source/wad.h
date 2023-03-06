
#ifndef __MELKOR_WAD_H__
#define __MELKOR_WAD_H__

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef struct {
    char identification[4];
    uint32_t numlumps;
    uint32_t infotableofs;
} wad_header_st;

typedef struct {
    uint32_t filepos;
    uint32_t size; 
    char name[8];
} wad_directory_st;

typedef struct {
    FILE *file;
    char *filepath;
    wad_header_st header;
} wad_st;

int wad_open(wad_st **self, const char *filepath);
int wad_close(wad_st **self);

char* _wad_strdup(const char *s);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__MELKOR_WAD_H__*/
