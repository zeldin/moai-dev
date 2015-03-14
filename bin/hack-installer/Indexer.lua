local Classifier = require("Classifier")

local Indexer = {}

function Indexer.addFile(index, path)
   local class = Classifier.classify(path)
   if not class then
      print("Fatal: Failed to classify "..path)
      os.exit(1)
   end
   if not index[class] then
      index[class] = {}
   end
   table.insert(index[class], path)
end

function Indexer.index(paths)
   local result = {}
   local path = table.remove(paths)
   local cnt = 0
   while path do
      for i,dir in pairs(MOAIFileSystem.listDirectories(path)) do
	 if fn ~= "." and fn ~= ".." then
	    table.insert(paths, path.."/"..dir)
	 end
      end
      for i,fn in pairs(MOAIFileSystem.listFiles(path)) do
	 if fn ~= "." and fn ~= ".." then
	    cnt = cnt + 1
	    io.output():write("\rIndexing files: "..cnt)
	    Indexer.addFile(result, path.."/"..fn)
	 end
      end
      path = table.remove(paths)
   end
   io.output():write(" Done\r\n")
   return result
end

return Indexer
