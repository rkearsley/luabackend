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

/*
    Prefix6 will only hold a prefix with a netmask of 64 or less!
    Prefix4 does not have any restriction regarding the netmask.

    prefix = prefixes.new()
    prefix:add("prefix to add", netmask, {a table of: content = "some kind of content..!", type = "TXT", ttl = 1234 [, priority = 1080]}, ... )
    table_of_values = prefix:find("prefix to search for")

    prefix:del("prefix to delete")
    size4, size6 = prefix:size() -- Gets number of prefixes
    prefix:deleteall() -- Deletes all prefixes without destroying your object in Lua.

    Example: Adding the prefix 2001:0db8::/32

    prefix = prefixes.new()
    prefix:add("2001:0db8::", 32, {content = "some kind of value to this record", type = "TXT", ttl = 1234, priority = 112})
    local table_of_values = prefix:find("2001:0db8:1234::5678")

    local k,v,kk,vv
    for k,v in pairs(table_of_values) do
        for kk,vv in pairs(v) do
            print(k, kk,vv)
        end
    end

    will get you something like this:

        1 type TXT
        1 priority 112
        1 ttl 1234
        1 content some kind of value to this record

    print(prefix:size())
        0  1

    prefix = nil
*/

#include "lua_prefixes.hh"
#include "lua_getvaluefromtable.hh"

bool LUAPrefixes::logging = false;

int LUAPrefixes::lua_prefixes_new (lua_State *lua) {
    if (logging)
        std::cerr << "(lua_prefixes_new)" << std::endl;

    LUAPrefixes::MAPS_OF_PREFIXES *mop;

    mop = (LUAPrefixes::MAPS_OF_PREFIXES *) lua_newuserdata(lua, sizeof(LUAPrefixes::MAPS_OF_PREFIXES));

    mop->p4 = new LUAPrefixes::MAP_Prefix4;
    mop->p6 = new LUAPrefixes::MAP_Prefix6;

    luaL_getmetatable(lua, "PREFIXES");
    lua_setmetatable(lua, -2);

    return 1;
}

int LUAPrefixes::lua_prefixes_gc (lua_State *lua) {
    LUAPrefixes::MAPS_OF_PREFIXES *mop = (LUAPrefixes::MAPS_OF_PREFIXES *)luaL_checkudata(lua, 1, "PREFIXES");

    if (logging)
        std::cerr << "(lua_prefixes_gc)" << std::endl;

    if (mop->p4 != NULL)
        delete mop->p4;

    if (mop->p6 != NULL)
        delete mop->p6;

    return 0;
}

int LUAPrefixes::lua_prefixes_deleteall (lua_State *lua) {
    LUAPrefixes::MAPS_OF_PREFIXES *mop = (LUAPrefixes::MAPS_OF_PREFIXES *)luaL_checkudata(lua, 1, "PREFIXES");

    if (logging)
        std::cerr << "(lua_prefixes_deleteall)" << std::endl;

    if (mop->p4 != NULL)
        delete mop->p4;

    if (mop->p6 != NULL)
        delete mop->p6;

    mop->p4 = new LUAPrefixes::MAP_Prefix4;
    mop->p6 = new LUAPrefixes::MAP_Prefix6;

    return 0;
}

int LUAPrefixes::lua_prefixes_size (lua_State *lua) {
    LUAPrefixes::MAPS_OF_PREFIXES *mop = (LUAPrefixes::MAPS_OF_PREFIXES *)luaL_checkudata(lua, 1, "PREFIXES");

    if (logging)
        std::cerr << "(lua_prefixes_size)" << std::endl;

    lua_pushnumber(lua, mop->p4->size());
    lua_pushnumber(lua, mop->p6->size());

    return 2;
}

int LUAPrefixes::prefixes_add4 (const char *prefix4, LUAPrefixes::MAP_Prefix4 *p4, uint32_t *j, lua_State *lua) {
    uint32_t prefix4_ntohl = ntohl(inet_addr(prefix4));

    if (prefix4_ntohl == 0) 
        return 0;

    LUAPrefixes::netmask4_value n4v;
    n4v.netmask = lua_tonumber(lua, 3);

    for (uint32_t i = 4; i <= *j; i++) {
        size_t returnedwhat = lua_type(lua, i);
        if (returnedwhat == LUA_TTABLE) {
            struct LUAPrefixes::__Values value;
            gVFT::getValueFromTable(lua, "content", value.content, i);
            gVFT::getValueFromTable(lua, "ttl", value.ttl, i);
            gVFT::getValueFromTable(lua, "type", value.type, i);
            gVFT::getValueFromTable(lua, "priority", value.priority, i);

            if (logging)
                std::cerr << "(lua_prefixes_add4) content: '" << value.content << "' ttl: '" << value.ttl << "' priority: '" << value.priority << "'" << std::endl;

            n4v.values.push_back(value);
        }
    }

    if (n4v.values.size() > 0)
        p4->insert(LUAPrefixes::PAIR_Prefix4(prefix4_ntohl, n4v));

    return 0;
}

int LUAPrefixes::prefixes_add6 (const char *prefix6, LUAPrefixes::MAP_Prefix6 *p6, uint32_t *j, lua_State *lua) {
    LUAPrefixes::netmask6_value n6v;

    n6v.netmask = lua_tonumber(lua, 3);

    //skip if netmask is bigger than 64.
    if (n6v.netmask > 64) 
        return 0;

    struct in6_addr addr6;
    inet_pton(AF_INET6, prefix6, &(addr6.s6_addr));
    for (int i = 0; i < 4; i++) {
        addr6.s6_addr32[i] = ntohl(addr6.s6_addr32[i]);
    }

    uint64_t prefix6_ntohl = (uint64_t) addr6.s6_addr32[0] << 32 | addr6.s6_addr32[1];

    if (prefix6_ntohl == 0)
        return 0;

    for (uint32_t i = 4; i <= *j; i++) {
        size_t returnedwhat = lua_type(lua, i);
        if (returnedwhat == LUA_TTABLE) {
            struct LUAPrefixes::__Values value;
            gVFT::getValueFromTable(lua, "content", value.content, i);
            gVFT::getValueFromTable(lua, "ttl", value.ttl, i);
            gVFT::getValueFromTable(lua, "type", value.type, i);
            gVFT::getValueFromTable(lua, "priority", value.priority, i);

            if (logging)
                std::cerr << "(lua_prefixes_add6) content: '" << value.content << "' ttl: '" << value.ttl << "' priority: '" << value.priority << "'" << std::endl;

            n6v.values.push_back(value);
        }
    }

    if (n6v.values.size() > 0)
        p6->insert(LUAPrefixes::PAIR_Prefix6(prefix6_ntohl, n6v));

    return 0;
}

int LUAPrefixes::lua_prefixes_add (lua_State *lua) {
    LUAPrefixes::MAPS_OF_PREFIXES *mop = (LUAPrefixes::MAPS_OF_PREFIXES *)luaL_checkudata(lua, 1, "PREFIXES");

    uint32_t i = lua_gettop(lua);
    if (i < 3) {
        return 0;
    }

    size_t l;
    const char *pref = lua_tolstring(lua, 2, &l);
    if (pref == NULL)
        return 0;

    std::string prefix = std::string(pref, l);

    if (logging)
        std::cerr << "(lua_prefixes_add) prefix to add: '" << prefix << "'" << std::endl;

    //negativ search; no : is found
    if (prefix.find(':') == std::string::npos)
        return prefixes_add4(prefix.c_str(), mop->p4, &i, lua);

    //negativ search; no . is found
    if (prefix.find('.') == std::string::npos)
        return prefixes_add6(prefix.c_str(), mop->p6, &i, lua);

    return 0;
}

void LUAPrefixes::prefixes_find_answer(std::vector<LUAPrefixes::__Values> *v, lua_State *lua) {
    int j = 0;

    lua_newtable(lua);
    int top1 = lua_gettop(lua);

    for (std::vector<LUAPrefixes::__Values>::iterator it_v = (*v).begin() ; it_v != (*v).end(); ++it_v) {
        j++;
        lua_pushnumber(lua, j);

            lua_newtable(lua);
            int top2 = lua_gettop(lua);
            lua_pushlstring(lua, "content", 7);
            lua_pushlstring(lua, (*it_v).content.c_str(), (*it_v).content.size());
            lua_settable(lua, top2);

            lua_pushlstring(lua, "ttl", 3);
            lua_pushnumber(lua, (*it_v).ttl);
            lua_settable(lua, top2);

            lua_pushlstring(lua, "priority", 8);
            lua_pushnumber(lua, (*it_v).priority);
            lua_settable(lua, top2);

            lua_pushlstring(lua, "type", 4);
            lua_pushlstring(lua, (*it_v).type.c_str(), (*it_v).type.size());
            lua_settable(lua, top2);

        lua_settable(lua, top1);
    }

    if (j == 0) {
        //no table as return value since we didn't have anything in the first place...
        lua_pop(lua, top1);
        lua_pushnil(lua);
        std::cerr << "(lua_prefixes_find_answer) This should not happend!" << std::endl;
    }
}

int LUAPrefixes::prefixes_find4(const char *ip4_address, LUAPrefixes::MAP_Prefix4 *p4, lua_State *lua) {
    if (p4->size() > 0) {
        LUAPrefixes::MAP_Prefix4::iterator it4;

        uint32_t ip4 = ntohl(inet_addr(ip4_address));

        it4 = p4->upper_bound(ip4);
        if (it4 != p4->begin())
            it4--;

        uint32_t net4 = (*it4).first;
        uint32_t nm4 = (*it4).second.netmask;

        if (logging)
            std::cerr << "(lua_prefix_find4) ip4_address: '" << ip4_address << "' ip4: '" << ip4 << "' net4: '" << net4 << "' nm4: '" << nm4 << "'" << std::endl;

        if (net4 > 0 && nm4 > 7 && nm4 < 33) {
            uint32_t netmask4 = (~0u) << (32 - nm4);

            if ((ip4 & netmask4) == (net4 & netmask4)) {
                if (logging)
                    std::cerr << "(lua_prefix_find4) found the prefix!" << std::endl;

                prefixes_find_answer(&(*it4).second.values, lua);

                return 1;
            }
        }
    }

    lua_pushnil(lua);
    return 1;
}

int LUAPrefixes::prefixes_find6(const char *ip6_address, LUAPrefixes::MAP_Prefix6 *p6, lua_State *lua) {
    if (p6->size() > 0) {
        LUAPrefixes::MAP_Prefix6::iterator it6;

        struct in6_addr addr6;

        inet_pton(AF_INET6, ip6_address, &(addr6.s6_addr));
        for (int i = 0; i < 4; i++) {
            addr6.s6_addr32[i] = ntohl(addr6.s6_addr32[i]);
        }

        uint64_t ip6 = (uint64_t) addr6.s6_addr32[0] << 32 | addr6.s6_addr32[1];

        it6 = p6->upper_bound(ip6);
        if (it6 != p6->begin())
            it6--;

        uint64_t net6 = (*it6).first;
        uint64_t nm6 = (*it6).second.netmask;

        if (logging)
            std::cerr << "(lua_prefix_find6) ip6_address: '" << ip6_address << "' ip6: '" << ip6 << "' net6: '" << net6 << "' nm6: '" << nm6 << "'" << std::endl;

        if (net6 > 0 && nm6 > 15 && nm6 < 65) {
            uint64_t netmask6 = (~0ull) << (64 - nm6);

            if ((ip6 & netmask6) == (net6 & netmask6)) {
                if (logging)
                    std::cerr << "(lua_prefix_find6) found the prefix!" << std::endl;

                prefixes_find_answer(&(*it6).second.values, lua);

                return 1;
            }
        }
    }

    lua_pushnil(lua);
    return 1;
}

int LUAPrefixes::lua_prefixes_find (lua_State *lua) {
    LUAPrefixes::MAPS_OF_PREFIXES *mop = (LUAPrefixes::MAPS_OF_PREFIXES *)luaL_checkudata(lua, 1, "PREFIXES");

    int i = lua_gettop(lua);
    if (i < 2) {
        lua_pushnil(lua);
        return 1;
    }

    size_t l;
    const char *ip = lua_tolstring(lua, 2, &l);
    if (ip == NULL) {
        lua_pushnil(lua);
        return 1;
    }

    std::string ip_address = std::string(ip, l);

    //negativ search; no : is found
    if (ip_address.find(':') == std::string::npos)
        return prefixes_find4(ip_address.c_str(), mop->p4, lua);

    //negativ search; no . is found
    if (ip_address.find('.') == std::string::npos)
        return prefixes_find6(ip_address.c_str(), mop->p6, lua);

    lua_pushnil(lua);
    return 1;
}

int LUAPrefixes::prefixes_del4 (const char *prefix4, LUAPrefixes::MAP_Prefix4 *p4) {
    uint32_t prefix4_ntohl = ntohl(inet_addr(prefix4));

    if (prefix4_ntohl > 0) 
        p4->erase(prefix4_ntohl);

    return 0;
}

int LUAPrefixes::prefixes_del6 (const char *prefix6, LUAPrefixes::MAP_Prefix6 *p6) {
    struct in6_addr addr6;

    inet_pton(AF_INET6, prefix6, &(addr6.s6_addr));
    for (int i = 0; i < 4; i++) {
        addr6.s6_addr32[i] = ntohl(addr6.s6_addr32[i]);
    }

    uint64_t prefix6_ntohl = (uint64_t) addr6.s6_addr32[0] << 32 | addr6.s6_addr32[1];

    if (prefix6_ntohl > 0) 
        p6->erase(prefix6_ntohl);

    return 0;
}

int LUAPrefixes::lua_prefixes_del (lua_State *lua) {
    LUAPrefixes::MAPS_OF_PREFIXES *mop = (LUAPrefixes::MAPS_OF_PREFIXES *)luaL_checkudata(lua, 1, "PREFIXES");

    size_t l;
    const char *pref = lua_tolstring(lua, 2, &l);
    if (pref == NULL)
        return 0;

    std::string prefix = std::string(pref, l);

    if (logging)
        std::cerr << "(lua_prefixes_del) prefix to del: '" << prefix << "'" << std::endl;

    //negativ search; no : is found
    if (prefix.find(':') == std::string::npos)
        return prefixes_del4(prefix.c_str(), mop->p4);

    //negativ search; no . is found
    if (prefix.find('.') == std::string::npos)
        return prefixes_del6(prefix.c_str(), mop->p6);

    return 0;
}

int LUAPrefixes::lua_prefixes_log (lua_State *lua) {
    if (lua_gettop(lua) > 0)
        LUAPrefixes::logging = lua_toboolean(lua, 1);

    return 0;
}

extern "C" int luaopen_prefixes(lua_State *lua) {
    static const struct luaL_Reg prefixes_f [] = {
        {"new", LUAPrefixes::lua_prefixes_new },
        {"log", LUAPrefixes::lua_prefixes_log },
        {NULL, NULL}
    };

    static const struct luaL_Reg prefixes_m [] = {
        {"add", LUAPrefixes::lua_prefixes_add },
        {"del", LUAPrefixes::lua_prefixes_del },
        {"find", LUAPrefixes::lua_prefixes_find },
        {"size", LUAPrefixes::lua_prefixes_size },
        {"__gc", LUAPrefixes::lua_prefixes_gc },
        {"deleteall", LUAPrefixes::lua_prefixes_deleteall },
        {NULL, NULL}
    };

    luaL_newmetatable(lua, "PREFIXES");
    lua_pushvalue (lua, -1);
    lua_setfield (lua, -2, "__index");

    luaL_register (lua, NULL, prefixes_m);
    luaL_register (lua, "prefixes", prefixes_f);

    return 1;
}
