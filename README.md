==========
Luabackend
==========

Luabackend for PowerDNS.

Replace to current luabackend in PowerDNS under
modules/luabackend with thisone and then recompile.

Has been tested with PowerDNS version 3.2

Please note that you can only use the version of Lua
which PowerDNS itself is using!

See README for more information.


============
Lua Prefixes
============

This also contain the module for Lua called Prefixes.

What this allows you to do is to index network prefixes.
Both IPv4 and IPv6. IPv6 can only be used with a netmask 
of 64 or lower.

If you want this in your application, you only need the files
lua_prefixes.*.

For more information of how to use this in Lua, see the comment 
in the beginning of the file lua_prefixes.cc.

If you are using prefixes then make sure that you don't use of any 
packetcache in PowerDNS at all! Your results will otherwise get
mixed up with each other.


==========================
TLMC - The Last Mile Cache
==========================

This also contains all the code for running TLMC
either for a content provider or an ISP.

Please see test/Domains/tlmc.*.example.lua and 
test/Modes/* for more.

For more information about what TLMC is, please visit
http://tlmc.fredan.se/

