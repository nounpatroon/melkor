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
#include <lua.hpp>

#include "wad.hh"
#include "wad.lua.hh"

int libmelkor_lua_open(lua_State *L);
int libmelkor_lua_debug(lua_State *L);

int libmelkor_lua__constructor(lua_State *L);
int libmelkor_lua__destructor(lua_State *L);

int libmelkor_lua_open(lua_State *L) {
    std::cout << "Hello world from !!open!!" << std::endl;
    libmelkor_lua_debug(L);

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

    // Add variables
    lua_pushstring(L, "PWAD"); lua_setfield(L, -2, "id");
    lua_pushstring(L, "nil"); lua_setfield(L, -2, "path");
    lua_pushinteger(L, 0); lua_setfield(L, -2, "lumps");

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
