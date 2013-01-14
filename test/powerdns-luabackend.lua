--remember, this is just a test case to see that the minimal backend does work...

logging = false
domains = {}

logger(log_info, "powerdns-luabackend.lua starting up!")

local commandline = getarg("commandline")

if commandline ~= nil then 
    logging = string.find(commandline, "log") ~= nil
    if logging then logger(log_info, "commandline:", commandline) end
    prefixes.log(logging)
end

local function loadme(a_file)
    local f = assert(loadfile(a_file))
    f()
    f = nil
end

function rediscover()
    local k,v
    for k,v in pairs(user_files_to_rediscover) do
        if logging then logger(log_info, "(l_rediscover) Loading file:", v) end
        loadme(v)
    end
end

local directory = dirname(getarg("filename"))

function reload()
    local domains = domains
    domains = nil
    collectgarbage()
    domains = {}

    local file = directory .. "/powerdns-Files-To-Load.lua"
    if logging then logger(log_info, "Loading:", file) end
    loadme(file)

    local k,v
    for k,v in pairs(user_files_to_reload) do
        if logging then logger(log_info, "(l_reload) Loading file:", v) end
        loadme(v)
    end

    --Trigger a rediscover. Same as the command: 'pdns_control rediscover'
    rediscover()
end

local backend_to_load = { 
    directory .. "/powerdns-minimal.lua",
}

local k,v
for k,v in pairs(backend_to_load) do
    if logging then logger(log_info, "Loading backend-file:", v) end
    loadme(v)
end

--Trigger a reload. Same as the command: 'pdns_control reload'
reload()
