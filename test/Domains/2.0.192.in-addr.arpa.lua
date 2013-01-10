domains["2.0.192.in-addr.arpa"] = {
    domain_id = 12,
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
        ["2.0.192.in-addr.arpa"] = { 
            {type = "NS", ttl = 3600, content = "ns1.test.example"},
            {type = "NS", ttl = 3600, content = "ns3.test2.example"},
        },
        ["1.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "ns1.test.example"},
        },
        ["2.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "ns2.test.example"},
        },
        ["3.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "mx1.test.example"},
        },
        ["4.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "mx2.test.example"},
        },
        ["5.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "host.test.example"},
        },
        ["6.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "ns3.test2.example"},
        },
        ["7.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "ns4.test2.example"},
        },
        ["8.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "anything.test2.example"},
        },
        ["9.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "se.test2.example"},
        },
        ["10.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "nl.test2.example"},
        },
        ["11.2.0.192.in-addr.arpa"] = { 
            {type = "PTR", ttl = 86400, content = "k.se.test2.example"},
        }
    }
}
