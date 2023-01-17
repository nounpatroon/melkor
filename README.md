# melkor
C/Lua WAD Library

## Example

### CPP

```C

/*main.cc*/
#include <iostream>
#include <cstring>
#include <cstdlib>

#include "wad.hh"

int main(void) {
    wad_c wad;

    typedef struct {
        int16_t x;
        int16_t y;
    } vex_st;

    const char *msg = "Hello world from melkor!";
    vex_st vex[4] = {
        {0,   0},
        {0,   255},
        {255, 0},
        {255, 255}
    };

    wad.open("MAP00.WAD");

    wad.insert(0, "_msg0", (void*)msg, strlen(msg)+1);
    wad.insert(0, "_msg0", (void*)msg, strlen(msg)+1);
    wad.erase(0);
    wad.insert(0, "vertexes", (void*)vex, sizeof(vex_st)*4);
    return 0;
}

```

### Lua5.1

```Lua

-- main.lua
require("libmelkor")
local wad = Melkor()

wad:open("MAP00.WAD") -- The library automatically creates the file if it does not exist

--  insert(index, name, file to include)
wad:insert(0, "entry", "file.txt")
wad:insert(0, "__hello", "empty") -- use empty to indicate mark
wad:insert(0, "__hello2", "empty")
wad:insert(0, "__hello3", "empty") 
wad:insert(0, "file1", "fileadsfasdf") -- assume that the file does not exist, so it does not include it
wad:insert(0, "newfile", "file.txt")

--  erase(index to delete)
wad:erase(2)

print("WAD HEADER")
print("Identification: ", wad.identification)
print("Path to file:   ", wad.path)
print("Total lumps:    ", wad.numlumps)

```