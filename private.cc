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
#include "pdns/arguments.hh"

string LUABackend::my_getArg(string a) {
    return getArg(a);
}

bool LUABackend::my_mustDo(string a) {
    return mustDo(a);
}

bool LUABackend::domaininfo_from_table(DomainInfo *di) {

    di->backend = NULL;

    if (!gVFT::getValueFromTable(lua, "id", di->id))
        return false;

    if (!gVFT::getValueFromTable(lua, "zone", di->zone))
        return false;

    if (!gVFT::getValueFromTable(lua, "serial", di->serial))
        return false;

    gVFT::getValueFromTable(lua, "notified_serial", di->notified_serial);
    gVFT::getValueFromTable(lua, "last_check", di->last_check);

    di->kind = DomainInfo::Native;

    string kind;
    if (gVFT::getValueFromTable(lua, "kind", kind)) {
        if (kind == "MASTER")
            di->kind = DomainInfo::Master;
        else if (kind == "SLAVE")
            di->kind = DomainInfo::Slave;
    }

    lua_pushlstring(lua, "masters", 7);
    lua_gettable(lua, -2);

    if(!lua_isnil(lua, -1)) {
        lua_pushnil(lua);

        while (lua_next(lua, -2) != 0) {
            size_t l;
            const char *value = lua_tolstring(lua, -1, &l);

            lua_pop(lua,1);
            int key __attribute__((unused)) = lua_tonumber(lua, -1); 

            if (value != NULL)
                di->masters.push_back(std::string(value, l));
        }
    }

    lua_pop(lua, 1);

    di->backend = this;

    return true;
}

void LUABackend::domains_from_table(vector<DomainInfo>* domains) {
    lua_pushnil(lua);

    
    size_t returnedwhat;

    while (lua_next(lua, -2) != 0) {
        returnedwhat = lua_type(lua, -1);
        if (returnedwhat == LUA_TTABLE) {
            DomainInfo di;

            if (domaininfo_from_table(&di)) 
                domains->push_back(di);
        }

        lua_pop(lua,1);
        int key __attribute__((unused)) = lua_tonumber(lua, -1);
    }
}


void LUABackend::dnsrr_to_table(lua_State *lua, const DNSResourceRecord *rr) {

    lua_newtable(lua);

    lua_pushlstring(lua, "qtype", 5);
    lua_pushlstring(lua, rr->qtype.getName().c_str(), rr->qtype.getName().size());
    lua_settable(lua, -3);

    lua_pushlstring(lua, "qclass", 6);
    lua_pushnumber(lua, rr->qclass);
    lua_settable(lua, -3);

    lua_pushlstring(lua, "priority", 8);
    lua_pushnumber(lua, rr->priority);
    lua_settable(lua, -3);

    lua_pushlstring(lua, "ttl", 3);
    lua_pushnumber(lua, rr->ttl);
    lua_settable(lua, -3);

    lua_pushlstring(lua, "auth", 4);
    lua_pushboolean(lua, rr->auth);
    lua_settable(lua, -3);

    lua_pushlstring(lua, "content", 7);
    lua_pushlstring(lua, rr->content.c_str(), rr->content.size());
    lua_settable(lua, -3);
}
