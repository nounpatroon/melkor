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
    WAD in LUA API C
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wad.h"
#include "wad.lua.h"

int libmelkor_lua__debug(lua_State *L);
int libmelkor_lua__new(lua_State *L);
int libmelkor_lua__delete(lua_State *L);

int libmelkor_lua_erase(lua_State *L);

int libmelkor_lua_erase(lua_State *L) {
    printf("\r\nHello from ERASE!!!\r\n");
    libmelkor_lua__debug(L);

    return 0;
}

int libmelkor_lua__delete(lua_State *L) {
    wad_st* wad = (wad_st*) lua_touserdata(L, 1);

    wad_close(wad);

    return 0;
}

int libmelkor_lua__new(lua_State *L) {
    wad_st *wad;
    int type;

    type = lua_type(L, 1);
    luaL_argcheck(L, type == LUA_TSTRING, 1, "`string' expected");

    const char* path = luaL_checkstring(L, 1);

    /*Clean the stack*/
    lua_settop(L, 0);

    wad = (wad_st*)lua_newuserdata(L, sizeof(wad_st));
    wad_open(wad, path);

    /*Set the __index metatable*/
    /*Add new metatable*/
    luaL_newmetatable(L, LIBMELKOR_NAME);
    /*Add garbage collector*/
    lua_pushcfunction(L, libmelkor_lua__delete); lua_setfield(L, -2, "__gc");
    /*Add __index*/
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    /*Add functions*/
    lua_pushcfunction(L, libmelkor_lua_erase); lua_setfield(L, -2, "erase");
    /*Add variable*/
    lua_pushstring(L, wad->header->identification); lua_setfield(L, -2, "id");
    lua_pushstring(L, wad->filepath); lua_setfield(L, -2, "path");
    lua_pushinteger(L, wad->header->numlumps); lua_setfield(L, -2, "lumps");

    /*Add metatable to userdata*/
    lua_setmetatable(L, 1);

    return 1;
}

int libmelkor_lua__debug(lua_State *L) {
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

int luaopen_libmelkor(lua_State *L) {
    lua_register(L, LIBMELKOR_NAME, libmelkor_lua__new);

    return 1;
}
