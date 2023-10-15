#!/usr/bin/env python
import os
import sys
import fnmatch

env = SConscript("external/SConscript")
env.tools=['mingw']

env.Append(CPPDEFINES = ['LAPI_GDEXTENSION'])
env.Append(CPPPATH 	  = [Dir('src').abspath, Dir('external').abspath])

env['sources'] = []
Export('env')
SConscript("lua_libraries/SConscript")

sources = Glob('*.cpp')
sources.append(Glob('src/*.cpp'))
sources.append(Glob('src/classes/*.cpp'))
sources.append(env['sources'])

if env["luaapi_luaver"] == 'jit':
    env.Append(CPPDEFINES=['LAPI_LUAJIT'])
elif env["luaapi_luaver"] == '5.1':
    env.Append(CPPDEFINES=['LAPI_51'])

library = env.SharedLibrary(
    "project/addons/luaAPI/bin/libluaapi{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
    source=sources,
)

env.Default(library)

