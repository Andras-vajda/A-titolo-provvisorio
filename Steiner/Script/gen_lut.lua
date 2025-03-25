local lfs = require("lfs")

local function get_csv_files(folder)
    local files = {}
    for file in lfs.dir(folder) do
        if file:match("%.csv$") then
            table.insert(files, file)
        end
    end
    table.sort(files)
    return files
end

local function parse_csv_files(folder)
    local lut = {}
    local terne = {}
    
    for _, file in ipairs(get_csv_files(folder)) do
        local path = folder .. "/" .. file
        local f = io.open(path, "r")
        local header = f:read("*l")
        local fields = {}
        for v in header:gmatch("[^,]+") do table.insert(fields, v) end
        local v, td = tonumber(fields[4]), tonumber(fields[5])
        table.insert(lut, {v, td})
        terne[v] = {}
        
        for line in f:lines() do
            local values = {}
            for num in line:gmatch("%d+") do table.insert(values, tonumber(num)) end
            table.insert(terne[v], values)
        end
        f:close()
    end
    return lut, terne
end

local function generate_c_header(lut, terne, output_file)
    local f = io.open(output_file, "w")
    f:write("#ifndef _STSC_LUT_H_\n#define _STSC_LUT_H_\n\n")
    f:write("#include <stdint.h>\n\n")
    
    f:write("#define MAX_LUT " .. #lut .. "\n")
    f:write("const uint32_t LUT[MAX_LUT][2] = {\n")
    for _, entry in ipairs(lut) do
        f:write(string.format("   {%d, %d},\n", entry[1], entry[2]))
    end
    f:write("};\n\n")

    f:write("typedef struct { uint32_t a, b, c; } Terna_t;\n\n")
    f:write("const Terna_t TERNE_PELTESOHN[MAX_LUT][MAX_DIFF] = {\n")
    for _, entry in ipairs(lut) do
        local v = entry[1]
        f:write(string.format("   // v = %d\n   {", v))
        for _, t in ipairs(terne[v]) do
            f:write(string.format("{%d, %d, %d}, ", t[1], t[2], t[3]))
        end
        f:write("},\n")
    end
    f:write("};\n\n#endif\n")
    f:close()
end

local folder_path = "./csv_input"
local lut, terne = parse_csv_files(folder_path)
generate_c_header(lut, terne, "STSC_LUT.h")
print("STSC_LUT.h generato con successo!")
