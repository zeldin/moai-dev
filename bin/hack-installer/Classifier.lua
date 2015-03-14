local Classifier = {}

function Classifier.classify(path)
   if path:sub(-4) == ".lua" or path:sub(-9) == ".chunkset" or
      path:sub(-12) == ".spritesheet" or path:sub(-10) == ".tilesheet" or
      path:sub(-7) == ".layout" or path:sub(-7) == ".shader" or
      path:sub(-10) == ".particles" then
      local f = io.open(path, "rb")
      if not f then
	 return nil
      end
      local ascii = f:read(4)
      f:close()
      if not ascii then
	 return "lua_src"
      end
      local buffer = MOAIDataBuffer.new()
      buffer:setString(ascii)
      buffer:hexEncode()
      local hex = buffer:getString()
      if ascii:sub(1,2) == "--" or ascii == "DFGr" or ascii == "loca" or ascii == "retu" then
	 return "lua_src"
      elseif hex == "1b4c7561" then
	 return "lua_bc51"
      elseif hex == "1b4c4a01" then
	 return "lua_bclj"
      elseif hex == "d2fcae3c" then
	 return "lua_encrypted"
      else
	 return nil
      end
   elseif path:sub(-7) == ".dds.gz" or path:sub(-4) == ".png" then
      return "texture"
   elseif path:sub(-4) == ".fnt" then
      return "font"
   elseif path:sub(-4) == ".fsb" or path:sub(-4) == ".fev" then
      return "sound"
   elseif path:sub(-4) == ".fsh" or path:sub(-4) == ".vsh" then
      return "shader"
   elseif path:sub(-9) == ".settings" then
      return "settings"
   else
      return nil
   end
end

return Classifier

