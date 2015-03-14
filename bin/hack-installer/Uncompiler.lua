local Uncompiler = {}

Uncompiler.javaCommand = nil
Uncompiler.jarPath = nil

function Uncompiler.setJavaCommand(cmd)
   Uncompiler.javaCommand = cmd
end

function Uncompiler.setJarPath(path)
   Uncompiler.jarPath = path
end

function Uncompiler.createCommandLine(args)
   local cmdLine = ""
   for i,arg in pairs(args) do
      arg = arg:gsub("[^-_0-9A-Za-z./]", "\\%1")
      if #cmdLine > 0 then
	 cmdLine = cmdLine.." "
      end
      cmdLine = cmdLine..arg
   end
   return cmdLine
end

function Uncompiler.uncompile(path)
   local cmdLine = {Uncompiler.javaCommand, "-jar", Uncompiler.jarPath, path}
   local pipe = io.popen(Uncompiler.createCommandLine(cmdLine))
   local source = pipe:read("*a")
   pipe:close()
   return source
end

return Uncompiler
