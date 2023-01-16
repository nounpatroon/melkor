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

int libmelkor_lua_debug(lua_State *L);
int libmelkor_lua_open(lua_State *L);
int libmelkor_lua_close(lua_State *L);

int libmelkor_lua__index(lua_State *L);

const luaL_reg libmelkor_lua_register[] = {
    {"open", libmelkor_lua_open},
    {"close", libmelkor_lua_close},
    {NULL, NULL}
};

int libmelkor_lua__index(lua_State *L) {
    wad_st *wad; 
    char *k; 

    wad = (wad_st*)lua_touserdata(L, -2);
    k = strdup(lua_tostring(L, -1));

    if(!strcmp(k, "identification")) {
        lua_pushstring(L, wad->header->identification);
    }
    else if(!strcmp(k, "numlumps")) {
        lua_pushinteger(L, (lua_Integer) wad->header->numlumps);
    }
    else {
        lua_pushnil(L);
    }

    free(k);
    return 1;
}

int libmelkor_lua_open(lua_State *L) {
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
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, libmelkor_lua__index);
    lua_rawset(L, -3);
    lua_setmetatable(L, -2);       

    return 1;
}

int libmelkor_lua_close(lua_State *L) {
    wad_st *wad;
    int type;
    char *stype;

    type = lua_type(L, -1);
    stype = strdup(lua_typename(L, type));
    luaL_argcheck(L, !strcmp(stype, "userdata"), 1, "`userdata' expected");

    wad = (wad_st*) lua_touserdata(L, -1);

    wad_close(wad);

    free(stype);
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

int luaopen_libmelkor(lua_State *L) {
    luaL_register(L, "melkor", libmelkor_lua_register);

    return 1;
}
