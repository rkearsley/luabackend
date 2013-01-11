local ttl4 = 4040
local ttl6 = 6060

domains["tlmc.content.example"] = { 
    domain_id = 15,
    type = "NATIVE",
    soa = {
        hostmaster = "hostmaster.tlmc.content.example",
        nameserver = "ns7.tlmc.content.example",
        serial = 2012121242,
        refresh = 28800,
        retry = 7200,
        expire = 604800,
        default_ttl = 86400,
        ttl = 3600
    },
    r = {
        ["tlmc.content.example"] = {
            {type = "NS", ttl = 3600, content = "ns9.west.test3.example"},
            {type = "NS", ttl = 3600, content = "ns1.test.example"},
            {type = "NS", ttl = 3600, content = "ns7.tlmc.content.example"},
            {type = "NS", ttl = 3600, content = "ns10.east.test4.example"},
            {type = "NS", ttl = 3600, content = "ns3.test2.example"},
            {type = "NS", ttl = 3600, content = "ns8.tlmc.content.example"},
        },
        ["ns7.tlmc.content.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.16"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::16"}
        },
        ["ns8.tlmc.content.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.17"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::17"}
        },
        ["static.tlmc.content.example"] = {
            --Magic happens here!
            --If a record has PREFIX as type, it will try to find a prefix from the 
            --prefixes as specified in the index of "prefix".
            --In this case, prefix and prefix2 is defined in the file Prefixes/Prefixes.lua.
            --Remember that you must run pdns_server without any packetcache at all, since
            --you do not want these answers from the prefix to be cached.
            {type = "PREFIX", qba = false, prefix = prefix},
            {type = "PREFIX", qba = true, prefix = prefix2, stop = true},
            --If stop is true and we found a prefix then we don't continue to go thru
            --the remaining records. This is so you can use remaining records as a default
            --values if you don't find a prefix.
            {type = "A", ttl = ttl4, content = "192.0.2.18"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::18"}
        },
        ["*.static.tlmc.content.example"] = {
            {type = "TXT", ttl = ttl4 + ttl6, content = "test with TXT record for a wildcard query"},
            {type = "A", ttl = ttl4, content = "192.0.2.19"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::19"}
        },
        ["*.k.se.static.tlmc.content.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.20"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::20"}
        },
        ["*.se.static.tlmc.content.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.21"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::21"}
        },
    }
}
