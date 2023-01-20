#!/bin/bash -e
./godot/bin/godot.linuxbsd.editor.x86_64.luaAPI --headless --path ../testing -s run_tests.gd

ERRFILE=../testing/log.txt.error
if test -f "$ERRFILE"; then
    exit 1
fi
