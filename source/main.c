#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "wad.h"

static int argc;
static char** argv;

void testHello() {
    int i;
    for(i = 0;i < argc;i++) {
        printf("%3d:\"%s\" ", i, argv[i]);
    }

    printf("\r\n%s \t\t\t---> OK\r\n", "testHello()");
}

void testOpen() {
    uint32_t i;
    wad_st *wad;

    for(i = 0;i < 255;i++) {
        wad = wad_open("MAP00.WAD");
        wad_close(wad);
    }

    printf("\r\n%s \t\t\t---> OK\r\n", "testOpen()");
}

void testInsert() {
    uint32_t i;
    typedef struct {
        int16_t x;
        int16_t y;
    } vex_st;

    vex_st vex[4] = {
        {0,0},
        {255,0},
        {0,255},
        {255,255}
    };

    wad_st *wad;

    wad = wad_open("MAP00.WAD");
    wad_insert(wad, 0, "idk", vex, sizeof(vex_st) * 4);
    
    for(i = 0;i < 2;i++) {
        wad_insert(wad, wad->header->numlumps, "vertexes", vex, sizeof(vex_st) * 4);
    }
    wad_close(wad);

    printf("\r\n%s \t\t\t---> OK\r\n", "testInsert()");
}

int main(int _argc,char** _argv) {
    argc = _argc;
    argv = _argv;

    testHello();
    testOpen();
    testInsert();

    printf("\r\nALL TEST SUCCESS\r\n");
    return 0;
}