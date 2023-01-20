#!/bin/bash -e
ls ../testing
pwd

path=$(dirname ../testing)
./scripts/godot/bin/godot.linuxbsd.editor.x86_64.luaAPI --headless --path path -s run_tests.gd

ERRFILE=testing/log.txt.error
if test -f "$ERRFILE"; then
    exit 1
fi
