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
    WAD in Lua
*/

#include <iostream>
#include <cstring>
#include <lua.hpp>

#include "wad.hh"
#include "wad.lua.hh"

/*open(path)*/
int libmelkor_lua_open(lua_State *L);
/*import(index, name, filepath)*/
int libmelkor_lua_import(lua_State *L);
/*remove(index)*/
int libmelkor_lua_remove(lua_State *L);
/*export(index, filepath)*/
int libmelkor_lua_export(lua_State *L);

int libmelkor_lua_debug(lua_State *L);

int libmelkor_lua__constructor(lua_State *L);
int libmelkor_lua__destructor(lua_State *L);

int libmelkor_lua_open(lua_State *L) {
    int type;

    /*Handling errors*/
    if(lua_gettop(L) != 1+1) {
        return luaL_error(L, "too fee/many arguments to open(path)");
    }

    type = lua_type(L, 2);
    luaL_argcheck(L, type == LUA_TSTRING, 2, "`string' expected");

    const char* path = luaL_checkstring(L, 2);

    (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->open(path);

    /*Remove the last arguments*/
    lua_settop(L, 1);

    // Update header
    luaL_getmetatable(L, LIBMELKOR_NAME);
    lua_pushstring(L, (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->get_id()); lua_setfield(L, -2, "identification");
    lua_pushstring(L, path); lua_setfield(L, -2, "path");
    lua_pushinteger(L, (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->get_total()); lua_setfield(L, -2, "numlumps");

    /*Add metatable to userdata*/
    lua_setmetatable(L, 1);

    return 0;    
}

int libmelkor_lua_import(lua_State *L) {
    uint32_t index;
    char *name;
    char *filepath;

    int type;

    /*Handling errors*/
    if(lua_gettop(L) != 3+1) {
        return luaL_error(L, "too fee/many arguments to import(index, name, filepath)");
    }

    type = lua_type(L, 2);
    luaL_argcheck(L, type == LUA_TNUMBER, 2, "`number' expected");
    type = lua_type(L, 3);
    luaL_argcheck(L, type == LUA_TSTRING, 3, "`string' expected");
    type = lua_type(L, 4);
    luaL_argcheck(L, type == LUA_TSTRING, 4, "`string' expected");

    /*Save arguments*/
    index = (uint32_t) luaL_checkint(L, 2);
    name = (char*) luaL_checkstring(L, 3);
    filepath = (char*) luaL_checkstring(L, 4);

    if(!strcmp(filepath, " ")) {
        (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->insert(index, name, 0, 0);
    }
    else {
        FILE *file = fopen(filepath, "rb+");
        char *data;
        size_t filelenght;

        if(file != NULL) {
            fseek(file, 0, SEEK_END);
            filelenght = ftell(file);
            fseek(file, 0, SEEK_SET);

            data = (char*)malloc(filelenght+1);
            data[filelenght] = '\0';
            for(size_t i = 0;i < filelenght;i++) {
                data[i] = fgetc(file);
            }

            (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->insert(index, name, data, strlen(data));
        }
        else {
            
            return 0;
        }
    }

    /*Remove the last arguments*/
    lua_settop(L, 1);

    // Update userdata
    luaL_getmetatable(L, LIBMELKOR_NAME);

    lua_pushinteger(L, (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->get_total()); lua_setfield(L, -2, "numlumps");

    /*Add metatable to userdata*/
    lua_setmetatable(L, 1);
    
    return 0;
}

int libmelkor_lua_remove(lua_State *L) {
    uint32_t index;
    
    int type;

    /*Handling errors*/
    if(lua_gettop(L) != 1+1) {
        return luaL_error(L, "too fee/many arguments to remove(index)");
    }

    type = lua_type(L, 2);
    luaL_argcheck(L, type == LUA_TNUMBER, 2, "`number' expected");

    /*Save arguments*/
    index = (uint32_t) luaL_checkint(L, 2);

    /*Remove the last arguments*/
    lua_settop(L, 1);

    // Update numlumps
    luaL_getmetatable(L, LIBMELKOR_NAME);

    (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->erase(index);
    lua_pushinteger(L, (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->get_total()); lua_setfield(L, -2, "numlumps");
    
    /*Add metatable to userdata*/
    lua_setmetatable(L, 1);

    return 0;
}

int libmelkor_lua_export(lua_State *L) {
    uint32_t index;
    char *filepath;

    FILE *file;
    char *data;
    size_t filelenght;
    size_t i;

    int type;

    /*Handling errors*/
    if(lua_gettop(L) != 2+1) {
        return luaL_error(L, "too fee/many arguments to export(index, filepath)");
    }

    type = lua_type(L, 2);
    luaL_argcheck(L, type == LUA_TNUMBER, 2, "`number' expected");
    type = lua_type(L, 3);
    luaL_argcheck(L, type == LUA_TSTRING, 3, "`string' expected");

    /*Save arguments*/
    index = (uint32_t) luaL_checkint(L, 2);
    filepath = (char*) luaL_checkstring(L, 3);

    /*Remove the last arguments*/
    lua_settop(L, 1);

    // Get table
    luaL_getmetatable(L, LIBMELKOR_NAME);

    /*Write data*/
    file = fopen(filepath, "wb+");

    data = (char*)malloc( (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->get_size(index) );
    data = (char*)(*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->get_data(index);

    fwrite(data, (*reinterpret_cast<wad_c**>(luaL_checkudata(L, 1, LIBMELKOR_NAME)))->get_size(index), 1, file);

    /*Add metatable to userdata*/
    lua_setmetatable(L, 1);

    fclose(file);
    free(data);
    return 0;
}

int libmelkor_lua_debug(lua_State *L) {
    int i,t,top;

    top = lua_gettop(L);
    printf("\r\nBEING STACK\r\n");
    printf("Stack size: %d\r\n", top);

    for(i = top;i >= 1;i--) {
        t = lua_type(L, i);
        switch (t)
        {
        case LUA_TSTRING:
            printf("%i -- (%i) ---- string `%s'", i, i - (top + 1), lua_tostring(L, i));
            break;

        case LUA_TBOOLEAN:
            printf("%i -- (%i) ---- boolean %s", i, i - (top + 1), lua_toboolean(L, i) ? "true" : "false");
            break;

        case LUA_TNUMBER:
            printf("%i -- (%i) ---- number %g", i, i - (top + 1), lua_tonumber(L, i));
            break;

        default:
            printf("%i -- (%i) ----        '%s'", i, i - (top + 1), lua_typename(L, t));
            break;
        }
        printf("\r\n");
    }

    printf("\r\nEND STACK\r\n");
    return 0;
}

int libmelkor_lua__constructor(lua_State *L) {
    /*Cleans the stack*/
    lua_settop(L, 0);

    *reinterpret_cast<wad_c**>(lua_newuserdata(L, sizeof(wad_c*))) = new wad_c();
    luaL_newmetatable(L, LIBMELKOR_NAME);

    // Add gabarge collector
    lua_pushcfunction(L, libmelkor_lua__destructor); lua_setfield(L, -2, "__gc");

    // Add index
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    // Add functions
    lua_pushcfunction(L, libmelkor_lua_open); lua_setfield(L, -2, "open");
    lua_pushcfunction(L, libmelkor_lua_remove); lua_setfield(L, -2, "remove");
    lua_pushcfunction(L, libmelkor_lua_import); lua_setfield(L, -2, "import");
    lua_pushcfunction(L, libmelkor_lua_export); lua_setfield(L, -2, "export");

    // Add variables
    lua_pushstring(L, "PWAD"); lua_setfield(L, -2, "identification");
    lua_pushstring(L, "nil"); lua_setfield(L, -2, "path");
    lua_pushinteger(L, 0); lua_setfield(L, -2, "numlumps");

    /*Add metatable to userdata*/
    lua_setmetatable(L, 1);

    return 1;
}

int libmelkor_lua__destructor(lua_State *L) {
    delete *reinterpret_cast<wad_c**>(lua_touserdata(L, 1));

    return 0;
}

int luaopen_libmelkor(lua_State *L) {
    lua_register(L, LIBMELKOR_NAME, libmelkor_lua__constructor);
    
    return 0;
}
