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

/* FIRST PART */

bool LUABackend::first = true;
unsigned int LUABackend::backend_count = 0;
unsigned int LUABackend::backend_numbers = 0;
pthread_mutex_t LUABackend::lock_the_lock;
lua_State *LUABackend::lua;
DNSPacket *LUABackend::dnspacket;
int LUABackend::f_lua_exec_error;

//minimal functions....
int LUABackend::f_lua_list;
int LUABackend::f_lua_lookup;
int LUABackend::f_lua_get;
int LUABackend::f_lua_getsoa;

//master functions....
int LUABackend::f_lua_getupdatedmasters;
int LUABackend::f_lua_setnotifed;

//slave functions....
int LUABackend::f_lua_getdomaininfo;
int LUABackend::f_lua_ismaster;
int LUABackend::f_lua_getunfreshslaveinfos;
int LUABackend::f_lua_setfresh;

int LUABackend::f_lua_starttransaction;
int LUABackend::f_lua_committransaction;
int LUABackend::f_lua_aborttransaction;
int LUABackend::f_lua_feedrecord;

//supermaster functions....
int LUABackend::f_lua_supermasterbackend;
int LUABackend::f_lua_createslavedomain;

//reload
int LUABackend::f_lua_reload;
//rediscover
int LUABackend::f_lua_rediscover;

//dnssec
int LUABackend::f_lua_alsonotifies;
int LUABackend::f_lua_getdomainmetadata;
int LUABackend::f_lua_setdomainmetadata;

int LUABackend::f_lua_getdomainkeys;
int LUABackend::f_lua_removedomainkey;
int LUABackend::f_lua_activatedomainkey;
int LUABackend::f_lua_deactivatedomainkey;
int LUABackend::f_lua_updatedomainkey;
int LUABackend::f_lua_gettsigkey;
int LUABackend::f_lua_adddomainkey;

int LUABackend::f_lua_getbeforeandafternamesabsolute;
int LUABackend::f_lua_updatednssecorderandauthabsolute;
int LUABackend::f_lua_updatednssecorderandauth;

bool LUABackend::logging = false;


LUABackend::LUABackend(const string &suffix) {

    setArgPrefix("lua" + suffix);

    try {

    // Make sure only one (the first) backend instance is initializing static things
    Lock l(&lock_the_lock);

    backend_count++;
    backend_numbers++;
    backend_mynumber = backend_numbers;

    backend_name.clear();
    backend_pid = pthread_self();
    backend_name = "[LUABackend " + uitoa(backend_numbers) +"] ";

    if (!first)
        return;

    first = false;

    //see lua_functions.cc
    init(&lua);

    }

    catch(LUAException &e) {
        L << Logger::Error << backend_name << "Error: " << e.what << endl;
        throw AhuException(e.what);
    }
}

LUABackend::~LUABackend() {
    // Make sure only one (the first) backend instance is initializing static things
    Lock l(&lock_the_lock);

    backend_count--;

    if (logging)
        L << Logger::Info << backend_name << "backend number: '" << backend_mynumber << "' - ending!" << endl;

    if (backend_count == 0) {
        lua_close(lua);
    }
}

bool LUABackend::list(const string &target, int domain_id) {
    if ((errno = pthread_mutex_lock(&lock_the_lock))) 
        throw runtime_error("(getsoa) Unable to acquiring lock: " + stringerror());

    if (logging)
        L << Logger::Info << backend_name << "(list) BEGIN target: '" << target << "' domain_id: '" << domain_id << "'" << endl;

    lua_rawgeti(lua, LUA_REGISTRYINDEX, f_lua_list);

    lua_pushlstring(lua, target.c_str(), target.size());
    lua_pushnumber(lua, domain_id);

    if(lua_pcall(lua, 2, 1, f_lua_exec_error) != 0) {
        pthread_mutex_unlock(&lock_the_lock);

        string e = backend_name + "(list) " + lua_tostring(lua, -1);
        lua_pop(lua, 1);

        throw runtime_error(e);
        return false;
    }

    size_t returnedwhat = lua_type(lua, -1);
    bool ok = false;

    if (returnedwhat == LUA_TBOOLEAN)
        ok = lua_toboolean(lua, -1);

    lua_pop(lua, 1);

    if (logging)
        L << Logger::Info << backend_name << "(list) END - " << (ok ? "true" : "false") << endl;

    if (!ok)
        pthread_mutex_unlock(&lock_the_lock);

    return ok;
}

void LUABackend::lookup(const QType &qtype, const string &qname, DNSPacket *p, int domain_id) {
    if ((errno = pthread_mutex_lock(&lock_the_lock))) 
        throw runtime_error("(getsoa) Unable to acquiring lock: " + stringerror());

    if (logging)
        L << Logger::Info << backend_name << "(lookup) BEGIN qtype: '" << qtype.getName().c_str() << "' qname: '" << qname << "' domain_id: '" << domain_id << "'" << endl;

    dnspacket = p;

    lua_rawgeti(lua, LUA_REGISTRYINDEX, f_lua_lookup);

    lua_pushlstring(lua, qtype.getName().c_str(), qtype.getName().size());
    lua_pushlstring(lua, qname.c_str(), qname.size());
    lua_pushnumber(lua, domain_id);

    if(lua_pcall(lua, 3, 0, f_lua_exec_error) != 0) {
        pthread_mutex_unlock(&lock_the_lock);

        string e = backend_name + "(lookup) " + lua_tostring(lua, -1);
        lua_pop(lua, 1);

        dnspacket = NULL;

        throw runtime_error(e);
        return;
    }

    dnspacket = NULL;

    if (logging)
        L << Logger::Info << backend_name << "(lookup) END" << endl;
}

bool LUABackend::get(DNSResourceRecord &rr) {
    if (logging)
        L << Logger::Info << backend_name << "(get) BEGIN" << endl;

    lua_rawgeti(lua, LUA_REGISTRYINDEX, f_lua_get);

    if (lua_pcall(lua, 0, 1, f_lua_exec_error) != 0) {
        pthread_mutex_unlock(&lock_the_lock);

        string e = backend_name + "(get) " + lua_tostring(lua, -1);
        lua_pop(lua, 1);

        throw runtime_error(e);
        return false;
    }

    rr.content.clear();

    size_t returnedwhat = lua_type(lua, -1);
    if (returnedwhat == LUA_TTABLE) {

        string qt;

        if (gVFT::getValueFromTable(lua, "type", qt))
            rr.qtype = qt;

        gVFT::getValueFromTable(lua, "name", rr.qname);
        gVFT::getValueFromTable(lua, "domain_id", rr.domain_id);
        gVFT::getValueFromTable(lua, "last_modified", rr.last_modified);
        gVFT::getValueFromTable(lua, "priority", rr.priority);

        if (!gVFT::getValueFromTable(lua, "auth", rr.auth))
            rr.auth = 1;

        gVFT::getValueFromTable(lua, "ttl", rr.ttl);

        gVFT::getValueFromTable(lua, "content", rr.content);
    }

    lua_pop(lua, 1);

    bool more = !rr.content.empty();

    if (logging)
        L << Logger::Info << backend_name << "(get) END - " << (more ? "more" : "no more") << " record(s) to follow" << endl;

    if (!more)
        pthread_mutex_unlock(&lock_the_lock);

    return more;
}

bool LUABackend::getSOA(const string &name, SOAData &soadata, DNSPacket *p) {
    Lock l(&lock_the_lock);

    if (logging)
        L << Logger::Info << backend_name << "(getsoa) BEGIN name: '" << name << "'" << endl;

    dnspacket = p;

    lua_rawgeti(lua, LUA_REGISTRYINDEX, f_lua_getsoa);

    lua_pushlstring(lua, name.c_str(), name.size());

    if (lua_pcall(lua, 1, 1, f_lua_exec_error) != 0) {
        string e = backend_name + "(getsoa) " + lua_tostring(lua, -1);
        lua_pop(lua, 1);

        dnspacket = NULL;

        throw runtime_error(e);
        return false;
    }

    dnspacket = NULL;

    size_t returnedwhat = lua_type(lua, -1);
    if (returnedwhat != LUA_TTABLE) {
        lua_pop(lua, 1 );
        if (logging)
            L << Logger::Info << backend_name << "(getsoa) END with NO soa" << endl;
        return false;
    }

    soadata.db = this;
    soadata.serial = 0;
    gVFT::getValueFromTable(lua, "serial", soadata.serial);
    if (soadata.serial == 0) {
        lua_pop(lua, 1 );
        L << Logger::Error << backend_name << "(getsoa) ERROR - No serialnumber for the domain: '" << name << "'" << endl;
        return false;
    }

    gVFT::getValueFromTable(lua, "refresh", soadata.refresh);
    gVFT::getValueFromTable(lua, "retry", soadata.retry);
    gVFT::getValueFromTable(lua, "expire", soadata.expire);
    gVFT::getValueFromTable(lua, "default_ttl", soadata.default_ttl);
    gVFT::getValueFromTable(lua, "domain_id", soadata.domain_id);

    gVFT::getValueFromTable(lua, "ttl", soadata.ttl);
    if (soadata.ttl == 0 && soadata.default_ttl > 0) 
        soadata.ttl = soadata.default_ttl;

    if (soadata.ttl == 0) {
        lua_pop(lua, 1);
        L << Logger::Error << backend_name << "(getsoa) ERROR - No ttl for the domain: '" << name << "'" << endl;
        return false;
    }

    if (!gVFT::getValueFromTable(lua, "nameserver", soadata.nameserver)) {
        soadata.nameserver = arg()["default-soa-name"];
        if (soadata.nameserver.empty()) {
            lua_pop(lua, 1);
            L << Logger::Error << backend_name << "(getsoa) ERROR - SOA Record is missing nameserver for the domain: '" << name << "'" << endl; 
            return false;
        }
    }

    if (!gVFT::getValueFromTable(lua, "hostmaster", soadata.hostmaster))
        soadata.hostmaster = "hostmaster." + name;

    if (!gVFT::getValueFromTable(lua, "qname", soadata.qname))
        soadata.qname = name;

    lua_pop(lua, 1);

    if (logging)
        L << Logger::Info << backend_name << "(getsoa) END with soa" << endl;

    return true;
}
