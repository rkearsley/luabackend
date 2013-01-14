--change this line and issue a 'pdns_control rediscover' to make the change.
--the value can only be 0 for offline or 1 for online mode.

domains["tlmc.isp.example"]["tlmc"]["mode"] = 1

domains["tlmc.isp.example"]["tlmc"]["blacklist"] = nil

domains["tlmc.isp.example"]["tlmc"]["blacklist"] = {
    --if a host exists in the index we are going to 
    --treat it like a offline answer. 
    --the 0 is just to let Lua set the name.
    --we don't use this value for anything.
    ["42static.tlmc.csp.example"] = 0
}


