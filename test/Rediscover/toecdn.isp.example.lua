--change this line and issue a 'pdns_control rediscover' to make the change.
--the value can only be 0 for offline or 1 for online mode.

domains["toecdn.isp.example"]["toecdn"]["mode"] = 1

domains["toecdn.isp.example"]["toecdn"]["blacklist"] = nil

domains["toecdn.isp.example"]["toecdn"]["blacklist"] = {
    --if a host exists in the index we are going to 
    --treat it like a offline answer. 
    --the 0 is just to let Lua set the name.
    --we don't use the value for anything.
    ["42static.toecdn.csp.example"] = 0
}


