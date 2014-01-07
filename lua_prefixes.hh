#ifndef LUA_PREFIXES_HH
#define LUA_PREFIXES_HH

#include "lua.hpp"

#include <string>
#include <vector>
#include <map>

#include <iostream>

#include <stdint.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

class LUAPrefixes {
public:

    static int lua_prefixes_log(lua_State*);
    static int lua_prefixes_add(lua_State*);
    static int lua_prefixes_del(lua_State*);
    static int lua_prefixes_new(lua_State*);
    static int lua_prefixes_gc(lua_State*);
    static int lua_prefixes_size(lua_State*);
    static int lua_prefixes_find(lua_State*);
    static int lua_prefixes_deleteall(lua_State*);
    static bool logging;

private:
    struct __Values {
        uint32_t ttl;
        uint32_t priority;
        std::string type;
        std::string content;
    };
    struct netmask4_value {
        uint32_t netmask;
        std::vector<__Values> values;
    };
    struct netmask6_value {
        uint64_t netmask;
        std::vector<__Values> values;
    };

    typedef std::map<uint32_t, netmask4_value> MAP_Prefix4;
    typedef std::map<uint64_t, netmask6_value> MAP_Prefix6;

    struct MAPS_OF_PREFIXES {
        MAP_Prefix4 *p4;
        MAP_Prefix6 *p6;
    };

    typedef std::pair<uint32_t, netmask4_value> PAIR_Prefix4;
    typedef std::pair<uint64_t, netmask6_value> PAIR_Prefix6;

    static int prefixes_add4(const char*, MAP_Prefix4*, uint32_t*, lua_State*);
    static int prefixes_add6(const char*, MAP_Prefix6*, uint32_t*, lua_State*);

    static int prefixes_del4(const char*, MAP_Prefix4*);
    static int prefixes_del6(const char*, MAP_Prefix6*);

    static void prefixes_find_answer(std::vector<__Values>*, lua_State*);
    static int prefixes_find4(const char*, MAP_Prefix4*, lua_State*);
    static int prefixes_find6(const char*, MAP_Prefix6*, lua_State*);
};

#endif
