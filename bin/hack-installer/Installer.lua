local Uncompiler = require("Uncompiler")
local Indexer = require("Indexer")

local Installer = {}

function Installer.uncompile(paths, destdir)
   for i,path in pairs(paths) do
      io.output():write("\rUncompiling files: "..i.."/"..#paths);
      io.output():flush()
      local status, source = pcall(Uncompiler.uncompile, path)
      if status then
	 local stream = MOAIFileStream.new ()
	 stream:open(destdir.."/"..path, MOAIFileStream.READ_WRITE_NEW)
	 stream:write(source)
	 stream:close()
      else
	 io.output():write("\rFailed to uncompile "..path.."\n");
	 MOAIFileSystem.copy(path, destdir.."/"..path)
      end
   end
   io.output():write(" Done\r\n")
end

function Installer.copyfiles(index, destdir)
   local n = 0
   local tot = 0
   for i,paths in pairs(index) do
      if paths then
	 tot = tot + #paths
      end
   end
   for i,paths in pairs(index) do
      if paths then
	 for j,path in pairs(paths) do
	    n = n + 1
	    io.output():write("\rCopying files: "..n.."/"..tot);
	    io.output():flush()
	    MOAIFileSystem.copy(path, destdir.."/"..path)
	 end
      end
   end
   io.output():write(" Done\r\n")
end

function Installer.install(destdir, do_uncompile)
   local index = Indexer.index({"Data", "Linux/Audio", "Linux/Munged"})
   if do_uncompile then
      Installer.uncompile(index.lua_bc51, destdir)
      index.lua_bc51 = nil
   end
   Installer.copyfiles(index, destdir)
end

return Installer
