local pairs = pairs
local tableinsert = table.insert
local stringsub = string.sub

local domain_soa = {}
local domains = domains
local logging = logging

local logger = logger
local log_info = log_info
local dnspacket = dnspacket

local remote_ip, remote_port, local_ip

local res = {}

local QTypes = QTypes

local function deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

function list(target, domain_id)
    if logging then logger(log_info, "(l_list)", "target: ", target, " domain_id: ", domain_id) end

    res = {}
    counter = 0
    nofr = 0
    remote_ip = nil

    local rec
    if domain_soa["domain_id"] == domain_id then
        rec = domain_soa["r"]
    end

    if rec ~= nil then
        local did = domain_soa["domain_id"]
        local k,v,kk,vv
        for k,v in pairs(rec) do
            for kk,vv in pairs(v) do
                if vv["type"] ~= "PREFIX" then
                    vv["domain_id"] = did
                    vv["name"] = k
                    tableinsert(res, vv)
                end
            end
        end
    end

    if res ~= nil then
        nofr = #res
        q_type = "ANY"
        domainid = domain_id
    else
        q_type = nil
        domainid = nil
    end

    if logging then logger(log_info, "(l_list) END - Number of records found:", nofr) end

    return nofr > 0
end

function lookup(qtype, qname, domain_id)
    if logging then logger(log_info, "(l_lookup) BEGIN qtype:", qtype, " qname:", qname," domain_id:", domain_id) end

    res = {}
    counter = 0
    nofr = 0

    remote_ip, remote_port, local_ip = dnspacket()
    if logging then logger(log_info, "(l_lookup) dnspacket - remote:", remote_ip, " port:", remote_port, " local:", local_ip) end

    if domain_soa["domain_id"] == domain_id then
        local k,v
        if domain_soa["tlmc"] == nil then
            if domain_soa["r"][qname] ~= nil then
                res = deepcopy(domain_soa["r"][qname])
            end
        else
            --This is to handle The Last Mile Cache for a ISP.
            local qname_len = qname:len()
            local myname_size = domain_soa["tlmc"]["myname"]:len()
            local question = stringsub(qname, qname_len - myname_size + 1)

            if logging then logger(log_info, "(l_lookup) TLMC", question) end

            if (qname_len >= myname_size) and (question == domain_soa["tlmc"]["myname"]) then 
                if (qname == domain_soa["tlmc"]["myname"]) then
                    res = deepcopy(domain_soa["r"][qname])
                    if logging then logger(log_info, "(l_lookup) TLMC question for myname") end
                elseif domain_soa["r"][qname] ~= nil then
                    res = deepcopy(domain_soa["r"][qname])
                    if logging then logger(log_info, "(l_lookup) TLMC already found the answer") end
                elseif domain_soa["tlmc"]["mode"] == 1 then
                    res = deepcopy(domain_soa["tlmc"]["online"])
                    if logging then logger(log_info, "(l_lookup) TLMC giveing online answer") end
                else
                    res = deepcopy(domain_soa["tlmc"]["offline"])
                    res[1]["name"] = qname
                    res[1]["content"] = domain_soa["tlmc"]["mylocation"] .. "." .. stringsub(qname, 1, (qname_len - myname_size - 1))
                    if logging then logger(log_info, "(l_lookup) TLMC giveing offline answer") end
                end
            end
         end
    end

    if res ~= nil then
        nofr = #res
        q_type = qtype
        q_name = qname
        domainid = domain_id
    else
        q_type = nil
        q_name = nil
        domainid = nil
    end

    if logging then logger(log_info, "(l_lookup) END - Number of records found:", nofr) end
end

function get()
    if logging then logger(log_info, "(l_get) BEGIN") end

    while counter < nofr do
        counter = counter + 1

        local result = {}
        local madecopy = false
        local no_more_records = false

        while res[counter] ~= nil and res[counter]["type"] == "PREFIX" do
            if logging then logger(log_info, "(l_get) Got a record for Prefix!") end
            local value = res[counter]["prefix"]:find(remote_ip)

            if value ~= nil then
                if logging then logger(log_info, "(l_get) Prefix - Found", #value, "answer...") end

                result = res[counter]

                madecopy = true
                result["prefix"] = nil

                local stop = result["stop"] ~= nil and result["stop"]
                result["stop"] = nil

                if result["qba"] then
                    if logging then logger(log_info, "(l_get) Prefix - question before answer") end
                    result["content"] = q_name .. "." .. value[1]["content"]
                else
                    result["content"] = value[1]["content"]
                end
                result["qba"] = nil

                result["type"] = value[1]["type"]
                if result["ttl"] == nil then result["ttl"] = value[1]["ttl"] end

                if result["priority"] == nil then if value[1]["priority"] ~= nil then result["priority"] = value[1]["priority"] end end

                if #value > 1 then
                    --more than one answer....
                    for k,v in pairs(value) do 
                        --we have already taking care of the first result so we are skipping that...
                        if k ~= 1 then
                            tableinsert(res, counter + 1, v)
                        end
                    end
                    nofr = nofr + (#value - 1)
                end

                --we don't continue with the remaining records if stop is true
                if stop then nofr = nofr - (#value + 1) end

                if logging then logger(log_info, "(l_get) Prefix - Got", #value, "records in the answer for this prefix. Number of records is now:", nofr, "(including this record to be served) stop is:", stop) end

                --now we continue with this record...
                break

            else
                if counter < nofr then
                    if logging then logger(log_info, "(l_get) Prefix - not found. Skipping to next record.") end
                    counter = counter + 1
                else
                    if logging then logger(log_info, "(l_get) Prefix - not found. No more records.") end
                    no_more_records = true
                    break
                end
            end
        end

        if no_more_records then break end

        if not(madecopy) then
            result = res[counter]
        end

        if (q_type == "ANY" or q_type == result["type"]) then
        --or (res[counter]["type"] == "PREFIX" and q_type == result["type"])
            if result["domain_id"] == nil then 
                result["domain_id"] = domainid
            end
            if result["name"] == nil then 
                result["name"] = q_name 
            end
            if logging then 
                local k,v
                for k,v in pairs(result) do
                    logger(log_info, "(l_get)", k, type(v), v)
                end
            end
            return result
        else
            if logging then 
                local t = result["type"] or res[counter]["type"]
                logger(log_info, "(l_get) Skipping record with type:", t, "was looking for type:", q_type) 
            end
        end
    end

    if logging then logger(log_info, "(l_get) END") end
end

function getsoa(name)
    if logging then logger(log_info, "(l_getsoa) BEGIN name:", name) end

    remote_ip, remote_port, local_ip = dnspacket()
    if logging then logger(log_info, "(l_getsoa) dnspacket - remote:", remote_ip, " port:", remote_port, " local:", local_ip) end

    domain_soa = domains[name]

    if domain_soa ~= nil and domain_soa["r"] ~= nil then
        domain_soa["soa"]["domain_id"] = domain_soa["domain_id"]
        if logging then logger(log_info, "(l_getsoa) END with soa") end
        return domain_soa["soa"] 
    end

    if logging then logger(log_info, "(l_getsoa) END with NO soa") end
end
