#!/usr/bin/env python
import os
import sys
import fnmatch

env = SConscript("external/SConscript")
env.tools=['mingw']

libraries 		= []
library_paths 	= ''
cppDefines 		= ['LAPI_GODOT_EXTENSION']
cppFlags 		= ['-Wall']
cxxFlags 		= []
if not env["platform"] == "windows":
    cxxFlags=['-std=c++17']
else:
    cxxFlags=['-std=c++17']

env.Append(LIBS 			= libraries)
env.Append(LIBPATH 		    = library_paths)
env.Append(CPPDEFINES 	    = cppDefines)
env.Append(CPPFLAGS 		= cppFlags)
env.Append(CXXFLAGS 		= cxxFlags)
sources = []

for root, dirnames, filenames in os.walk('src'):
  for filename in fnmatch.filter(filenames, '*.cpp'):
    sources.append(Glob(os.path.join(root, filename)[len(root)+1:]))



if env["platform"] == "macos":
    library = env.SharedLibrary(
        "bin/luaapi/libluaapi.{}.{}.framework/libluaapi.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "bin/luaapi/libluaapi{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

env.Default(library)
