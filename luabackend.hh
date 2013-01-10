#ifndef LUABACKEND_HH
#define LUABACKEND_HH

#include "lua.hpp"

#include "pdns/dnsbackend.hh"
#include "pdns/lock.hh"
#include "pdns/qtype.hh"

#undef VERSION
#include <string>

#include "lua_getvaluefromtable.hh"

using std::string;

class LUAException {
public:
  LUAException(const string &ex) : what(ex){}
  string what;
};

class LUABackend : public DNSBackend {

public:
//  MINIMAL BACKEND

    LUABackend(const string &suffix="");
    ~LUABackend();
    bool list(const string &target, int domain_id);
    void lookup(const QType &qtype, const string &qname, DNSPacket *p, int domain_id);
    bool get(DNSResourceRecord &rr);
    //! fills the soadata struct with the SOA details. Returns false if there is no SOA.
    bool getSOA(const string &name, SOAData &soadata, DNSPacket *p=0);


//  MASTER BACKEND

    void getUpdatedMasters(vector<DomainInfo>* domains);
    void setNotifed(int id, u_int32_t serial);


//  SLAVE BACKEND

    bool getDomainInfo(const string &domain, DomainInfo &di);
    bool isMaster(const string &name, const string &ip);
    void getUnfreshSlaveInfos(vector<DomainInfo>* domains);
    void setFresh(int id);

    bool startTransaction(const string &qname, int id);
    bool commitTransaction();
    bool abortTransaction();
    bool feedRecord(const DNSResourceRecord &rr);


//  SUPERMASTER BACKEND

    bool superMasterBackend(const string &ip, const string &domain, const vector<DNSResourceRecord>&nsset, string *account, DNSBackend **db);
    bool createSlaveDomain(const string &ip, const string &domain, const string &account);


//  DNSSEC BACKEND

    //! get a list of IP addresses that should also be notified for a domain
    void alsoNotifies(const string &domain, set<string> *ips);
    bool getDomainMetadata(const string& name, const std::string& kind, std::vector<std::string>& meta);
    bool setDomainMetadata(const string& name, const std::string& kind, std::vector<std::string>& meta);

    bool getDomainKeys(const string& name, unsigned int kind, std::vector<KeyData>& keys);
    bool removeDomainKey(const string& name, unsigned int id);
    bool activateDomainKey(const string& name, unsigned int id);
    bool deactivateDomainKey(const string& name, unsigned int id);
    bool getTSIGKey(const string& name, string* algorithm, string* content);
    int addDomainKey(const string& name, const KeyData& key);

    bool getBeforeAndAfterNamesAbsolute(uint32_t id, const std::string& qname, std::string& unhashed, std::string& before, std::string& after);
    bool updateDNSSECOrderAndAuthAbsolute(uint32_t domain_id, const std::string& qname, const std::string& ordername, bool auth);
    bool updateDNSSECOrderAndAuth(uint32_t domain_id, const std::string& zonename, const std::string& qname, bool auth);


//  OTHER

    void reload();
    void rediscover(string* status=0);


    //private.cc
    string my_getArg(string a);
    bool my_mustDo(string a);


    string backend_name;
    static lua_State *lua;
    static DNSPacket *dnspacket;

    static bool logging;


private:
    static bool first;
    static pthread_mutex_t lock_the_lock;
    pthread_t backend_pid;
    static unsigned int backend_count;
    static unsigned int backend_numbers;
    unsigned int backend_mynumber;

    static int f_lua_exec_error;

    //minimal functions....
    static int f_lua_list;
    static int f_lua_lookup;
    static int f_lua_get;
    static int f_lua_getsoa;

    //master functions....
    static int f_lua_getupdatedmasters;
    static int f_lua_setnotifed;

    //slave functions....
    static int f_lua_getdomaininfo;
    static int f_lua_ismaster;
    static int f_lua_getunfreshslaveinfos;
    static int f_lua_setfresh;

    static int f_lua_starttransaction;
    static int f_lua_committransaction;
    static int f_lua_aborttransaction;
    static int f_lua_feedrecord;

    //supermaster functions....
    static int f_lua_supermasterbackend;
    static int f_lua_createslavedomain;

    //reload
    static int f_lua_reload;
    //rediscover
    static int f_lua_rediscover;

    //dnssec
    static int f_lua_alsonotifies;
    static int f_lua_getdomainmetadata;
    static int f_lua_setdomainmetadata;

    static int f_lua_getdomainkeys;
    static int f_lua_removedomainkey;
    static int f_lua_activatedomainkey;
    static int f_lua_deactivatedomainkey;
    static int f_lua_updatedomainkey;
    static int f_lua_gettsigkey;
    static int f_lua_adddomainkey;

    static int f_lua_getbeforeandafternamesabsolute;
    static int f_lua_updatednssecorderandauthabsolute;
    static int f_lua_updatednssecorderandauth;


//  FUNCTIONS TO THIS BACKEND

    //private.cc
    bool domaininfo_from_table(DomainInfo *di);
    void domains_from_table(vector<DomainInfo>* domains);
    void dnsrr_to_table(lua_State *lua, const DNSResourceRecord *rr);

    //lua_functions.cc
    void init(lua_State **lua);
    void get_lua_function(lua_State *lua, const char *name, int *function);
    void get_lua_function2(lua_State *lua, const char *name, int *function);

    //dnssec.cc
    bool updateDomainKey(const string& name, unsigned int &id, bool toowhat);
};

#endif
