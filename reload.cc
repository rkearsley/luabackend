/*
    Copyright (C) 2011-2013 Fredrik Danerklint

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published 
    by the Free Software Foundation

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "luabackend.hh"

#include "pdns/logger.hh"
/*
    virtual void reload();
    virtual void rediscover(string* status=0);
*/

void LUABackend::reload() {

    if (f_lua_reload == 0)
        return;

    Lock l(&lock_the_lock);

    if (logging)
        L << Logger::Info << backend_name << "(reload) BEGIN" << endl;

    lua_rawgeti(lua, LUA_REGISTRYINDEX, f_lua_reload);

    if (lua_pcall(lua, 0, 0, f_lua_exec_error) != 0) {
        string e = backend_name + "(reload)" + lua_tostring(lua, -1);
        lua_pop(lua, 1);

        throw runtime_error(e);
        return;
    }

    if (logging)
        L << Logger::Info << backend_name << "(reload) END" << endl;

    return;
}

void LUABackend::rediscover(string* status) {

    if (f_lua_rediscover == 0)
        return;

    Lock l(&lock_the_lock);

    if (logging)
        L << Logger::Info << backend_name << "(rediscover) BEGIN" << endl;

    lua_rawgeti(lua, LUA_REGISTRYINDEX, f_lua_rediscover);

    if (lua_pcall(lua, 0, 1, f_lua_exec_error) != 0) {
        string e = backend_name + "(rediscover)" + lua_tostring(lua, -1);
        lua_pop(lua, 1);

        throw runtime_error(e);
        return;
    }

    size_t returnedwhat = lua_type(lua, -1);
    if (returnedwhat == LUA_TSTRING) {
        size_t l;
        const char *s = lua_tolstring(lua, -1, &l);
        if (s != NULL)
            *status = std::string(s, l);
    }

    lua_pop(lua, 1);

    if (logging)
        L << Logger::Info << backend_name << "(rediscover) END" << endl;

    return;
}
