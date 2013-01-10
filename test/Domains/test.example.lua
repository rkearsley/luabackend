local ttl4 = 400
local ttl6 = 600

domains["test.example"] = { 
    domain_id = 11,
    type = "NATIVE",
    soa = {
        hostmaster = "hostmaster.test.example",
        nameserver = "ns1.test.example",
        serial = 2012121242,
        refresh = 28800,
        retry = 7200,
        expire = 604800,
        default_ttl = 86400,
        ttl = 3600
    },
    r = {
        ["test.example"] = {
            {type = "NS", ttl = 3600, content = "ns1.test.example"},
            {type = "NS", ttl = 3600, content = "ns2.test.example"},
            {type = "MX", ttl = 3600, priority = 10, content = "mx1.test.example"},
            {type = "MX", ttl = 3600, priority = 11, content = "mx2.test.example"}
        },
        ["ns1.test.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.1"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::1"}
        },
        ["ns2.test.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.2"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::2"}
        },
        ["mx1.test.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.3"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::3"}
        },
        ["mx2.test.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.4"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::4"}
        },
        ["www.test.example"] = { 
            {type = "CNAME", ttl = ttl4 + ttl6, content = "host.test.example"} 
        },
        ["host.test.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.5"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::5"}
        }
    }
}
