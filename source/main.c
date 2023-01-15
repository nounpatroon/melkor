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

    printf("\r\n%s ---> OK\r\n", "testHello()");
}

void testOpen() {
    int i;
    wad_st *wad;

    for(i = 0;i < 255;i++) {
        wad = wad_open("MAP00.WAD");
        wad_close(wad);
    }

    printf("\r\n%s ---> OK\r\n", "testOpen()");
}

int main(int _argc,char** _argv) {
    argc = _argc;
    argv = _argv;

    testHello();
    testOpen();

    return 0;
}