AM_CPPFLAGS=@THREADFLAGS@
EXTRA_DIST=OBJECTFILES OBJECTLIBS 

INCLUDES=-I/usr/include/lua5.1

lib_LTLIBRARIES = libluabackend.la

libluabackend_la_SOURCES=luabackend.cc luabackend.hh lua_functions.cc minimal.cc reload.cc master.cc private.cc slave.cc supermaster.cc dnssec.cc lua_prefixes.cc lua_prefixes.hh 

libluabackend_la_LDFLAGS=-module -avoid-version -llua5.1
