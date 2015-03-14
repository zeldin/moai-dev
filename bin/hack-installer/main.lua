local myfn = debug.getinfo(1).source:sub(2)
local mydir = MOAIFileSystem.getAbsoluteDirectoryPath(myfn:match("(.*/)"))

package.path = mydir .. "?.lua"

local Uncompiler = require('Uncompiler')
Uncompiler.setJavaCommand(os.getenv("JAVACMD"))
Uncompiler.setJarPath(mydir .. "unluac.jar")

local Installer = require('Installer')
MOAIFileSystem.setWorkingDirectory(os.getenv("SRCDIR"))
Installer.install(os.getenv("DESTDIR"))
