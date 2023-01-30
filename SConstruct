#!/usr/bin/env python
import os
import sys
import fnmatch

env = SConscript("external/SConscript")
env.tools=['mingw']

libraries 		= []
library_paths 	= ''
cppDefines 		= ['LAPI_GODOT_EXTENSION']
cppPath        = [Dir('src').abspath, Dir('external').abspath]
cppFlags 		= []
cxxFlags 		= []

env.Append(LIBS 			= libraries)
env.Append(LIBPATH 		    = library_paths)
env.Append(CPPDEFINES 	    = cppDefines)
env.Append(CPPPATH 	        = cppPath)
env.Append(CPPFLAGS 		= cppFlags)
env.Append(CXXFLAGS 		= cxxFlags)
sources = Glob('src/*.cpp')
sources.append(Glob('src/classes/*.cpp'))

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "project/addons/luaAPI/bin/libluaapi.{}.{}.framework/libluaapi.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "project/addons/luaAPI/bin/libluaapi{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

env.Default(library)


