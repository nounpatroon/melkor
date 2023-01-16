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

#ifndef __wad_lua_h__
#define __wad_lua_h__

#include <lua.h>
#include <lauxlib.h>

#include "wad.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

int luaopen_wad(lua_State *L);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__wad_lua_h__*/
