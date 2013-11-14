local ttl4 = 4044
local ttl6 = 6064

local tfad = 86400

domains["toecdn.isp.example"] = { 
    domain_id = 16,
    type = "NATIVE",
    soa = {
        hostmaster = "hostmaster.toecdn.isp.example",
        nameserver = "ns11.toecdn.isp.example",
        serial = 2012121242,
        refresh = 28800,
        retry = 7200,
        expire = 604800,
        default_ttl = 86400,
        ttl = 3600
    },
    --The Open Edge Content Delivery Network.
    toecdn = {
        --If mode is 1 then we are in online mode. 0 is offline mode.
        --See Redicover/toecdn.isp.example.lua for more.
        ["mode"] = 0,
        --mylocation is UN Locode as described in the specification of The Open Edge Content Delivery Network.
        --More information about this can be found http://toecdn.fredan.se/
        ["mylocation"] = "kaa.k.se",
        ["myname"] = "toecdn.isp.example",
        --In offline mode we answer with mylocation + question without myname.
        --Example; answer to the question 'static.toecdn.content.example.toecdn.isp.example':
        --static.toecdn.content.example.toecdn.isp.example 180 IN CNAME kaa.k.se.static.toecdn.content.example
        ["offline"] = {
            {type = "CNAME", ttl = 180}
        },
        --In online mode we answer with these records for question to '*.toecdn.isp.example'
        --Should be an anycast address to the cache server at the ISP.
        ["online"] = {
            {type = "A", ttl = 90, content = "192.0.2.187"},
            {type = "AAAA", ttl = 90, content = "2001:0db8::718c:718c"}
        },
    },
    r = {
        ["toecdn.isp.example"] = {
            {type = "NS", ttl = tfad, content = "ns11.toecdn.isp.example"},
            {type = "NS", ttl = tfad, content = "ns12.toecdn.isp.example"},

            {type = "A", ttl = ttl4, content = "192.0.2.34"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::34"},

            --even tough you might not want to have A or AAAA records for this record, you MUST have the following TXT record:
            --you specify all of your unicast ip addresses of your cache servers with netmasks as one prefix per TXT record.
            --this addresses is where your cache server(s) originates their requests from.

            --this TXT record is to be used by the upstream provider(s) of yours to build access rules so that 
            --only those listed can connect to their servers.
            --(only cache server(s) should be allowed to connect to a upstream provider, not everyone).
            {type = "TXT", ttl = tfad, content = "192.0.2.64/32"},
            {type = "TXT", ttl = tfad, content = "2001:0db8::718c:0264/128"},
        },
        ["ns11.toecdn.isp.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.32"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::32"}
        },
        ["ns12.toecdn.isp.example"] = {
            {type = "A", ttl = ttl4, content = "192.0.2.33"},
            {type = "AAAA", ttl = ttl6, content = "2001:0db8::33"}
        },
    }
}
