# melkor
C++/Lua WAD Library

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

    wad.insert(0, "text0", (char*)msg, strlen(msg));
    wad.insert(1, "text1", (char*)msg, strlen(msg));
    wad.erase(1);
    wad.insert(1, "vertexes", (char*)vex, sizeof(vex_st)*4);
    wad.insert(2, "__entry", 0, 0);

    std::cout <<    "WAD HEADER" << std::endl
                <<  "Identification: " << wad.get_identification() << std::endl 
                <<  "Path to file:   " << wad.get_path() << std::endl 
                <<  "Total lumps:    " << wad.get_numlumps() << std::endl;
    return 0;
}

```

### Lua5.1

```Lua

-- main.lua
require("libmelkor")
local wad = Melkor()

wad:open("MAP00.WAD") -- The library automatically creates the file if it does not exist

-- import(index, name, file to include)
wad:import(0, "entry", "main.lua")
wad:import(1, "__mark1", " ") -- use " " to indicate mark
wad:import(2, "__mark2", " ")
wad:import(3, "__mark3", " ") 
wad:import(4, "elded1", "i_dont_exist.data") -- assume that the file does not exist, so it does not include it
wad:import(4, "name2", "main.lua")

-- remove(index to delete)
wad:remove(2) -- this will delete __mark2

-- export(index, file to save)
wad:export(0, "main2.lua")

print("WAD HEADER")
print("Identification: ", wad.identification)
print("Path to file:   ", wad.path)
print("Total lumps:    ", wad.numlumps)

```