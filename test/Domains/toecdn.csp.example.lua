local ttl4 = 4040
local ttl6 = 6060

domains["toecdn.csp.example"] = { 
    domain_id = 15,
    type = "NATIVE",
    soa = {
        hostmaster = "hostmaster.toecdn.csp.example",
        nameserver = "ns7.toecdn.csp.example",
        serial = 2012121242,
        refresh = 28800,
        retry = 7200,
        expire = 604800,
        default_ttl = 86400,
        ttl = 3600
    },
    r = {
        ["toecdn.csp.example"] = {
            {type = "NS", ttl = 3600, content = "ns9.west.test3.example"},
            {type = "NS", ttl = 3600, content = "ns1.test.example"},
            {type = "NS", ttl = 3600, content = "ns7.toecdn.csp.example"},
            {type = "NS", ttl = 3600, content = "ns10.east.test4.example"},
            {type = "NS", ttl = 3600, content = "ns3.test2.example"},
            {type = "NS", ttl = 3600, content = "ns8.toecdn.csp.example"},
        },
        ["ns7.toecdn.csp.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.16"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::16"}
        },
        ["ns8.toecdn.csp.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.17"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::17"}
        },
        ["static.toecdn.csp.example"] = {
            --Magic happens here!
            --If a record has PREFIX as type, it will try to find a prefix from the 
            --prefixes as specified in the index of "prefix".
            --In this case, prefix and toecdn_prefix is defined in the file Prefixes/Prefixes.lua.
            --Remember that you must run pdns_server without any packetcache at all, since
            --you do not want these answers from the prefix to be cached.
            {type = "PREFIX", qba = false, prefix = prefix},
            {type = "PREFIX", qba = true, prefix = toecdn_prefix, stop = true},
            --If stop is true and we found a prefix then we don't continue to go through
            --the remaining records. This is so you can use the remaining records as a default
            --values if you don't find a prefix.
            {type = "A", ttl = ttl4, content = "192.0.2.18"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::18"}
        },
        ["*.static.toecdn.csp.example"] = {
            {type = "TXT", ttl = ttl4 + ttl6, content = "test with TXT record for a wildcard query"},
            {type = "A", ttl = ttl4, content = "192.0.2.19"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::19"}
        },
        ["*.k.se.static.toecdn.csp.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.20"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::20"}
        },
        ["*.se.static.toecdn.csp.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.21"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::21"}
        },
    }
}
