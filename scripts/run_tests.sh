#!/bin/bash -e
# we expect the working directory to be root.
scripts/godot/bin/godot.linuxbsd.editor.x86_64.luaAPI --headless --path testing/ -s run_tests.gd
mv testing/log.txt testing/log-lua.txt
scripts/godot/bin/godot.linuxbsd.editor.x86_64.luaAPI.luaJIT --headless --path testing/ -s run_tests.gd
mv testing/log.txt testing/log-luaJIT.txt