============
 Luabackend
============

Luabackend for PowerDNS.

Replace to current luabackend in PowerDNS under
modules/luabackend with thisone and then recompile.

Has been tested with PowerDNS version 3.2

Please note that you can only use the version of Lua
which PowerDNS itself is using!

See README for more information.


==============
 Lua Prefixes
==============

This also contain the module for Lua called Prefixes.

What this allows you to do is to index network prefixes.
Both IPv4 and IPv6. IPv6 can only be used with a netmask 
of 64 or lower.

If you want this in your application, you only need the files
lua_prefixes.cc, lua_prefixes.hh and lua_getvaluefromtable.hh.

For more information of how to use this in Lua, see the comment 
in the beginning of the file lua_prefixes.cc.

If you are using prefixes then make sure that you don't use of any 
packetcache in PowerDNS at all! Your results will otherwise get
mixed up with each other.

Howto compile this as a standalone module:

g++ -shared -o prefixes.so lua_prefixes.cc -fPIC `pkg-config lua5.1 --libs --cflags`


=================================================
 TOECDN - The Open Edge Content Delivery Network
=================================================

This also contains all the code for running TOECDN
either for a Content Service Provider (csp) or as an 
Internet Service Provider (isp).

Please see test/Domains/toecdn.*.example.lua and 
test/Rediscover/* for more.

For more information about what TOECDN is, please visit
http://www.toecdn.org/

