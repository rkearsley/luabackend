if prefix == nil then prefix = prefixes.new() else prefix:deleteall() end

prefix:add("127.0.0.0", 9, {content = "This record should not be found", ttl = 1234, type = "TXT"}, {content = "Neither should this be found", ttl = 2345, type = "TXT"})
prefix:add("2001:0db8:1::", 48, {content = "This record should not be found", ttl = 1234, type = "TXT"})

if prefix2 == nil then prefix2 = prefixes.new() else prefix2:deleteall() end

prefix2:add("127.0.0.0", 8, {content = "tlmc.isp.example", ttl = 1234, type = "CNAME"})
prefix2:add("2001:0db8:2::", 48, {content = "tlmc.isp.example", ttl = 1234, type = "CNAME"})
