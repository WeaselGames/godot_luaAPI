#!/usr/bin/env python
import os
import sys
import fnmatch

env = SConscript("external/SConscript")
env.tools=['mingw']

env.Append(CPPDEFINES 	    = ['LAPI_GDEXTENSION'])
env.Append(CPPPATH 	        = [Dir('src').abspath, Dir('external').abspath])
sources = Glob('*.cpp')
sources.append(Glob('src/*.cpp'))
sources.append(Glob('src/classes/*.cpp'))

library = env.SharedLibrary(
    "project/addons/luaAPI/bin/libluaapi{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
    source=sources,
)

env.Default(library)

