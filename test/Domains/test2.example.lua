local ttl4 = 404
local ttl6 = 606

domains["test2.example"] = { 
    domain_id = 14,
    type = "NATIVE",
    soa = {
        hostmaster = "hostmaster.test2.example",
        nameserver = "ns3.test2.example",
        serial = 2012121242,
        refresh = 28800,
        retry = 7200,
        expire = 604800,
        default_ttl = 86400,
        ttl = 3600
    },
    r = {
        ["test2.example"] = {
            {type = "NS", ttl = 3600, content = "ns5.north.test3.example"},
            {type = "NS", ttl = 3600, content = "ns2.test.example"},
            {type = "NS", ttl = 3600, content = "ns3.test2.example"},
            {type = "NS", ttl = 3600, content = "ns6.south.test3.example"},
            {type = "NS", ttl = 3600, content = "ns4.test2.example"},
            {type = "MX", ttl = 3600, priority = 12, content = "mx1.test.example"},
            {type = "MX", ttl = 3600, priority = 13, content = "mx2.test.example"}
        },
        ["ns3.test2.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.6"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::6"}
        },
        ["ns4.test2.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.7"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::7"}
        },
        ["*.test2.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.8"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::8"}
        },
        ["se.test2.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.9"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::9"}
        },
        ["nl.test2.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.10"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::10"}
        },
        ["k.se.test2.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.11"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::11"}
        },
    }
}
