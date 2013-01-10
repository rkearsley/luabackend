#ifndef LUA_GETVALUEFROMTABLE_HH
#define LUA_GETVALUEFROMTABLE_HH

#include "lua.hpp"

#include <string>
#include <stdint.h>

class gVFT {
    public:
        static bool getValueFromTable(lua_State *lua, uint32_t key, std::string& value, int32_t index = -2) { 
            lua_pushnumber(lua, key);
            lua_gettable(lua, index);

            value.clear();
            bool ret = false;

            if (!lua_isnil(lua, -1)) {
                size_t l;
                const char *s = lua_tolstring(lua, -1, &l);
                if (s != NULL)
                    value = std::string(s, l);
                ret = true;
            }

            lua_pop(lua, 1);

            return ret;
        }

        static bool getValueFromTable(lua_State *lua, const std::string& key, time_t& value, int32_t index = -2) {
            lua_pushstring(lua, key.c_str());
            lua_gettable(lua, index);

            value = 0;
            bool ret = false;

            if (!lua_isnil(lua, -1)) {
                value = (time_t)lua_tonumber(lua, -1);
                ret = true;
            }

            lua_pop(lua, 1);

            return ret;
        }

        static bool getValueFromTable(lua_State *lua, const std::string& key, uint32_t& value, int32_t index = -2) {
            lua_pushstring(lua, key.c_str());
            lua_gettable(lua, index);

            value = 0;
            bool ret = false;

            if (!lua_isnil(lua, -1)) {
                value = (uint32_t)lua_tonumber(lua, -1);
                ret = true;
              }

            lua_pop(lua, 1);

            return ret;
        }

        static bool getValueFromTable(lua_State *lua, const std::string& key, uint16_t& value, int32_t index = -2) {
            lua_pushstring(lua, key.c_str());
            lua_gettable(lua, index);

            value = 0;
            bool ret = false;

            if (!lua_isnil(lua, -1)) {
                value = (uint16_t)lua_tonumber(lua, -1);
                ret = true;
            }

            lua_pop(lua, 1);

            return ret;
        }

        static bool getValueFromTable(lua_State *lua, const std::string& key, int& value, int32_t index = -2) {
            lua_pushstring(lua, key.c_str());
            lua_gettable(lua, index);

            value = 0;
            bool ret = false;

            if (!lua_isnil(lua, -1)) {
                value = (int)lua_tonumber(lua, -1);
                ret = true;
            }

            lua_pop(lua, 1);

            return ret;
        }

        static bool getValueFromTable(lua_State *lua, const std::string& key, bool& value, int32_t index = -2) {
            lua_pushstring(lua, key.c_str());
            lua_gettable(lua, index);

            value = false;
            bool ret = false;

            if (!lua_isnil(lua, -1)) {
                value = lua_toboolean(lua, -1);
                ret = true;
            }

            lua_pop(lua, 1);

            return ret;
        }

        static bool getValueFromTable(lua_State *lua, const std::string& key, std::string& value, int32_t index = -2) {
            lua_pushstring(lua, key.c_str());
            lua_gettable(lua, index);

            value.clear();
            bool ret = false;

            if (!lua_isnil(lua, -1)) {
                size_t l;
                const char *s = lua_tolstring(lua, -1, &l);
                if (s != NULL)
                    value = std::string(s, l);
                ret = true;
            }

            lua_pop(lua, 1);

            return ret;
        }
};
#endif
