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

#define LUABACKEND_EXTERN_F_HH

#include "luabackend.hh"

#include "pdns/logger.hh"
#include "pdns/arguments.hh"
#include "pdns/dnspacket.hh"

#include <iostream>
#include <sstream>
using namespace std;

extern "C" int luaopen_prefixes(lua_State *lua);

const luaL_Reg lualibs[] = {
    {"", luaopen_base},
    {LUA_LOADLIBNAME, luaopen_package},
    {LUA_TABLIBNAME, luaopen_table},
    {LUA_IOLIBNAME, luaopen_io},
    {LUA_OSLIBNAME, luaopen_os},
    {LUA_STRLIBNAME, luaopen_string},
    {LUA_MATHLIBNAME, luaopen_math},
    {LUA_DBLIBNAME, luaopen_debug},
//    {LUA_COLIBNAME, luaopen_coroutine},
#ifdef USE_LUAJIT
    {"bit",     luaopen_bit},
    {"jit",     luaopen_jit},
#endif
    {"prefixes", luaopen_prefixes},
    {NULL, NULL}
};

int my_lua_panic (lua_State *lua) {
    lua_getfield(lua, LUA_REGISTRYINDEX, "__LUABACKEND"); 
    LUABackend* lb = (LUABackend*) lua_touserdata(lua, -1);

    assert(lua == lb->lua);

    stringstream e;

    e << lb->backend_name << "LUA PANIC! '" << lua_tostring(lua, -1) << "'" << endl;

    throw LUAException (e.str());

    return 0;
}

int l_arg_get (lua_State *lua) {
    int i = lua_gettop(lua);
    if (i < 1)
        return 0;

    lua_getfield(lua, LUA_REGISTRYINDEX, "__LUABACKEND"); 
    LUABackend* lb = (LUABackend*) lua_touserdata(lua, -1);

    string a = lua_tostring(lua, 1);

    std::string arg = lb->my_getArg(a);

    if (arg.empty())
        lua_pushnil(lua);
    else 
        lua_pushstring(lua, arg.c_str());
    return 1;
}

int l_arg_mustdo (lua_State *lua) {
    int i = lua_gettop(lua);
    if (i < 1)
        return 0;

    lua_getfield(lua, LUA_REGISTRYINDEX, "__LUABACKEND"); 
    LUABackend* lb = (LUABackend*) lua_touserdata(lua, -1);

    string a = lua_tostring(lua, 1);

    if (::arg().isEmpty(a))
        lua_pushnil(lua);
    else 
        lua_pushboolean(lua, lb->my_mustDo(a));

    return 1;
}

int l_dnspacket (lua_State *lua) {
    lua_getfield(lua, LUA_REGISTRYINDEX, "__LUABACKEND"); 
    LUABackend* lb = (LUABackend*) lua_touserdata(lua, -1);

    if (lb->dnspacket == NULL) {
        lua_pushnil(lua);

        return 1;
    }

    lua_pushstring(lua, lb->dnspacket->getRemote().c_str());
    lua_pushnumber(lua, lb->dnspacket->getRemotePort());
    lua_pushstring(lua, lb->dnspacket->getLocal().c_str());

    return 3;
}

#include <libgen.h>

int l_dirname (lua_State *lua) {
    if (lua_gettop(lua) < 1)
        return 0;

    //lua will make an copy of the string...
    char *path = (char *) lua_tostring(lua, 1);
    lua_pushstring(lua, dirname(path));

    return 1;
}

int l_basename (lua_State *lua) {
    if (lua_gettop(lua) < 1)
        return 0;

    //lua will make an copy of the string...
    char *path = (char *) lua_tostring(lua, 1);
    lua_pushstring(lua, basename(path));

    return 1;
}

int l_logger (lua_State *lua) {
    lua_getfield(lua, LUA_REGISTRYINDEX, "__LUABACKEND"); 
    LUABackend* lb = (LUABackend*)lua_touserdata(lua, -1);

    int i = lua_gettop(lua);
    if (i < 1)
        return 0;

    int log_level = 0;
    stringstream s;
    int j;
    const char *ss;

    log_level = lua_tointeger(lua, 1);

    string space = "";
    size_t returnedwhat;
    bool ret_bool;
    size_t number;

    i--;
    for(j = 2; j <= i; j++) {
        returnedwhat = lua_type(lua, j);
        if (returnedwhat == LUA_TBOOLEAN) {
            ret_bool = lua_toboolean(lua, j);
            s << space << (ret_bool ? "true" : "false");
        } else if (returnedwhat == LUA_TNUMBER) {
            number = lua_tonumber(lua, j);
            s << space << number;
        } else if (returnedwhat == LUA_TLIGHTUSERDATA) {
            s << space << "lightuserdata";
        } else if (returnedwhat == LUA_TUSERDATA) {
            s << space << "userdata";
        } else if (returnedwhat == LUA_TFUNCTION) {
            s << space << "function";
        } else if (returnedwhat == LUA_TTHREAD) {
            s << space << "thread";
        } else if (returnedwhat == LUA_TTABLE) {
            s << space << "table";
        } else {
            ss = lua_tostring(lua, j);
            if (ss != NULL)
                s << space << ss;
            else
                s << space << "nil";
        }
        space = " ";
    }

    L.log(lb->backend_name + s.str(), (Logger::Urgency) log_level);

    return 0;
}

void register_lua_functions(lua_State *lua) {

#if LUA_VERSION_NUM == 501
    lua_gc(lua, LUA_GCSTOP, 0);  // stop collector during initialization 

    const luaL_Reg *lib = lualibs;
    for (; lib->func; lib++) {
        lua_pushcfunction(lua, lib->func);
        lua_pushstring(lua, lib->name);
        lua_call(lua, 1, 0);
    }

    lua_gc(lua, LUA_GCRESTART, 0);

#else
    const luaL_Reg *lib = lualibs;
    for (; lib->func; lib++) {
        luaL_requiref(lua, lib->name, lib->func, 1);
        lua_settop(lua, 0);
    }

#endif


    lua_pushinteger(lua, Logger::All);
    lua_setglobal(lua, "log_all");

    lua_pushinteger(lua, Logger::NTLog);
    lua_setglobal(lua, "log_ntlog");

    lua_pushinteger(lua, Logger::Alert);
    lua_setglobal(lua, "log_alert");

    lua_pushinteger(lua, Logger::Critical);
    lua_setglobal(lua, "log_critical");

    lua_pushinteger(lua, Logger::Error);
    lua_setglobal(lua, "log_error");

    lua_pushinteger(lua, Logger::Warning);
    lua_setglobal(lua, "log_warning");

    lua_pushinteger(lua, Logger::Notice);
    lua_setglobal(lua, "log_notice");

    lua_pushinteger(lua, Logger::Info);
    lua_setglobal(lua, "log_info");

    lua_pushinteger(lua, Logger::Debug);
    lua_setglobal(lua, "log_debug");

    lua_pushinteger(lua, Logger::None);
    lua_setglobal(lua, "log_none");

    lua_pushcfunction(lua, l_dnspacket);
    lua_setglobal(lua, "dnspacket");

    lua_pushcfunction(lua, l_logger);
    lua_setglobal(lua, "logger");

    lua_pushcfunction(lua, l_arg_get);
    lua_setglobal(lua, "getarg");

    lua_pushcfunction(lua, l_arg_mustdo);
    lua_setglobal(lua, "mustdo");

    lua_pushcfunction(lua, l_dirname);
    lua_setglobal(lua, "dirname");

    lua_pushcfunction(lua, l_basename);
    lua_setglobal(lua, "basename");

    lua_newtable(lua);
    //first we make key = numerical, value = string
    for (vector<QType::namenum>::const_iterator iter = QType::names.begin(); iter != QType::names.end(); ++iter) {
        lua_pushnumber(lua, iter->second);
        lua_pushlstring(lua, iter->first.c_str(), iter->first.size());
        lua_settable(lua, -3);
    }

    lua_pushnumber(lua, 3);
    lua_pushlstring(lua, "NXDOMAIN", 8);
    lua_settable(lua, -3);

    //second we make key = string, value = numeric
    for (vector<QType::namenum>::const_iterator iter = QType::names.begin(); iter != QType::names.end(); ++iter) {
        lua_pushlstring(lua, iter->first.c_str(), iter->first.size());
        lua_pushnumber(lua, iter->second);
        lua_settable(lua, -3);
    }

    lua_pushlstring(lua, "NXDOMAIN", 8);
    lua_pushnumber(lua, 3);
    lua_settable(lua, -3);

    lua_setglobal(lua, "QTypes");
}

void LUABackend::get_lua_function2(lua_State *lua, const char *name, int *function) {
    *function = 0;

    string f = "f_";
    f.append(name);

    string arg = "";
    if (!::arg().isEmpty(f))
        arg = getArg(f);

    lua_getglobal(lua, arg == "" ? name : arg.c_str());
    if (!lua_isnil(lua, -1)) {
        size_t returnedwhat = lua_type(lua, -1);
        if (returnedwhat == LUA_TFUNCTION) {
            lua_pushvalue(lua, -1);
            *function = luaL_ref(lua, LUA_REGISTRYINDEX);
        }
    }
}

void LUABackend::get_lua_function(lua_State *lua, const char *name, int *function) {
    *function = 0;

    lua_getglobal(lua, name );
    if (!lua_isnil(lua, -1)) {
        size_t returnedwhat = lua_type(lua, -1);
        if (returnedwhat == LUA_TFUNCTION) {
            lua_pushvalue(lua, -1);
            *function = luaL_ref(lua, LUA_REGISTRYINDEX);
        }
    }
}


void LUABackend::init(lua_State **lua) {
    logging = ::arg().mustDo("query-logging") || mustDo("query-logging");

    if (logging) {
        L.toConsole(Logger::Info);
        L << Logger::Info << backend_name << "(init) BEGIN" << endl;
    }

#if LUA_VERSION_NUM == 501
    *lua = lua_open();
#else
    *lua = luaL_newstate();
#endif

    if (*lua != NULL) {
        lua_atpanic(*lua, my_lua_panic);

        string filename = getArg("filename");

        if (logging)
            L << Logger::Info << backend_name << "(init) Loading filename: '" << filename << "'" << endl;

        if (luaL_loadfile(*lua, filename.c_str()) != 0) {
            stringstream e;
            e << backend_name << "Error loading the file '" << filename << "' : " << lua_tostring(*lua, -1) << endl;

            lua_pop(*lua, 1);
            throw LUAException (e.str());

        } else {
            lua_pushlightuserdata(*lua, (void*)this);
            lua_setfield(*lua, LUA_REGISTRYINDEX, "__LUABACKEND");

            register_lua_functions(*lua);

            if(lua_pcall(*lua, 0, 0, 0)) { 
                stringstream e;
                e << backend_name << "Error running the file '" << filename << "' : " << lua_tostring(*lua, -1) << endl;

                lua_pop(*lua, 1);
                throw LUAException (e.str());

            } else {
                get_lua_function2(*lua,"exec_error", &f_lua_exec_error);

                //minimal functions....
                get_lua_function(*lua,"list", &f_lua_list);
                get_lua_function(*lua,"lookup", &f_lua_lookup);
                get_lua_function(*lua,"get", &f_lua_get);
                get_lua_function(*lua,"getsoa", &f_lua_getsoa);

                if (f_lua_list == 0 || f_lua_lookup == 0 || f_lua_get == 0 || f_lua_getsoa == 0) {
                        throw LUAException (backend_name + "MINIMAL BACKEND: Missing required function(s)!");
                }

                //master functions....
                get_lua_function(*lua,"getupdatedmasters", &f_lua_getupdatedmasters);
                get_lua_function(*lua,"setnotifed", &f_lua_setnotifed);

                //slave functions....
                get_lua_function(*lua,"getdomaininfo", &f_lua_getdomaininfo);
                get_lua_function(*lua,"ismaster", &f_lua_ismaster);
                get_lua_function(*lua,"getunfreshslaveinfos", &f_lua_getunfreshslaveinfos);
                get_lua_function(*lua,"setfresh", &f_lua_setfresh);
                get_lua_function(*lua,"starttransaction", &f_lua_starttransaction);
                get_lua_function(*lua,"committransaction", &f_lua_committransaction);
                get_lua_function(*lua,"aborttransaction", &f_lua_aborttransaction);
                get_lua_function(*lua,"feedrecord", &f_lua_feedrecord);

                //supermaster functions....
                get_lua_function(*lua,"supermasterbackend", &f_lua_supermasterbackend);
                get_lua_function(*lua,"createslavedomain", &f_lua_createslavedomain);

                //reload
                get_lua_function(*lua,"reload", &f_lua_reload);
                //rediscover
                get_lua_function(*lua,"rediscover", &f_lua_rediscover);

                 //dnssec
                get_lua_function(*lua,"alsonotifies", &f_lua_alsonotifies);
                get_lua_function(*lua,"getdomainmetadata", &f_lua_getdomainmetadata);
                get_lua_function(*lua,"setdomainmetadata", &f_lua_setdomainmetadata);

                get_lua_function(*lua,"getdomainkeys", &f_lua_getdomainkeys);
                get_lua_function(*lua,"removedomainkey", &f_lua_removedomainkey);
                get_lua_function(*lua,"activatedomainkey", &f_lua_activatedomainkey);
                get_lua_function(*lua,"deactivatedomainkey", &f_lua_deactivatedomainkey);
                get_lua_function(*lua,"updatedomainkey", &f_lua_updatedomainkey);
                get_lua_function(*lua,"adddomainkey", &f_lua_adddomainkey);

                get_lua_function(*lua,"gettsigkey", &f_lua_gettsigkey);

                get_lua_function(*lua,"getbeforeandafternamesabsolute", &f_lua_getbeforeandafternamesabsolute);
                get_lua_function(*lua,"updatednssecorderandauthabsolute", &f_lua_updatednssecorderandauthabsolute);
                get_lua_function(*lua,"updatednssecorderandauth", &f_lua_updatednssecorderandauth); // not needed...

            }
        }
    } else {
        //a big kaboom here!
        throw LUAException (backend_name + "LUA OPEN FAILED!");
    }

    if (logging)
        L << Logger::Info << backend_name << "(init) END" << endl;
}
