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

int lua_libmelkor_debug(lua_State *L);
int lua_libmelkor_open(lua_State *L);
int lua_libmelkor_close(lua_State *L);

int lua_libmelkor__new(lua_State *L);
int lua_libmelkor__index(lua_State *L);

const luaL_reg lua_libmelkor_register[] = {
    {"open", lua_libmelkor_open},
    {"close", lua_libmelkor_close},
    {"debug", lua_libmelkor_debug},
    {NULL, NULL}
};

int lua_libmelkor__new(lua_State *L) {
    wad_st *wad;
    char *path;

    path = strdup(luaL_checkstring(L, 1));

    /*Cleans stack*/
    lua_settop(L, 0);

    /*Alloc object*/
    wad = (wad_st*)lua_newuserdata(L, sizeof(wad_st*));
    wad_open(wad, path);

    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, lua_libmelkor__index);
    lua_rawset(L, 2);
    lua_setmetatable(L, 1);

    free(path);
    return 1;
}

int lua_libmelkor__index(lua_State *L) {
    wad_st *wad;
    char *k;

    wad = (wad_st*)lua_touserdata(L, 1);
    k = strdup(lua_tostring(L, 2));

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

int lua_libmelkor_open(lua_State *L) {
    printf("Hello from open!\r\n");

    return 0;
}

int lua_libmelkor_close(lua_State *L) {
    printf("Hello from close!\r\n");

    return 0;
}

int lua_libmelkor_debug(lua_State *L) {
    int i,t,top;

    top = lua_gettop(L);
    printf("\r\nBEING STACK\r\n");
    printf("Stack size: %d\r\n", top);

    for(i = top;i >= 1;i--) {
        t = lua_type(L, i);
        switch (t)
        {
        case LUA_TSTRING:
            printf("%i -- (%i) ---- `%s'", i, i - (top + 1), lua_tostring(L, i));
            break;

        case LUA_TBOOLEAN:
            printf("%i -- (%i) ---- %s", i, i - (top + 1), lua_toboolean(L, i) ? "true" : "false");
            break;

        case LUA_TNUMBER:
            printf("%i -- (%i) ---- %g", i, i - (top + 1), lua_tonumber(L, i));
            break;

        default:
            printf("%i -- (%i) ---- %s", i, i - (top + 1), lua_typename(L, t));
            break;
        }
        printf("\r\n");
    }

    printf("\r\nEND STACK\r\n");
    return 0;
}

int luaopen_libmelkor(lua_State *L) {
    lua_pushcfunction(L, lua_libmelkor__new);

    return 1;
}
