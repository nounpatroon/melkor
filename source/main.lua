-- For lua5.1

local WAD = require("libmelkor")
local wad = WAD("MAP00.WAD")

print(wad.identification)
print(wad.numlumps)
