# melkor
C/Lua WAD Library

## Example

### C89

```C

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wad.h"

int main(void) {
    wad_st *wad;

    typedef struct {
        int16_t x;
        int16_t y;
    } vex_st;

    const char *_MSG0 = "Hello world from melkor!";
    vex_st vex[4] = {
        {0,   0},
        {0,   255},
        {255, 0},
        {255, 255}
    };

    wad = (wad_st*)malloc(sizeof(wad_st));
    memset(wad, 0, sizeof(wad_st));
    wad_open(wad, "MAP00.WAD");

    wad_insert(wad, 0, "vertexes", (char*)vex, sizeof(vex_st) * 4);
    wad_insert(wad, 0, "__msg0", _MSG0, strlen(_MSG0)+1);

    wad_close(wad);
    return 0;
}

```

### Lua5.1

```Lua

local WAD = require("libmelkor")
local wad = WAD("MAP00.WAD")

print(wad.identification)
print(wad.numlumps)

```