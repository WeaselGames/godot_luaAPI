#!/bin/bash -e
./scripts/godot/bin/godot.linuxbsd.editor.x86_64.luaAPI --headless --path $(dirname ../testing) -s run_tests.gd

ERRFILE=testing/log.txt.error
if test -f "$ERRFILE"; then
    exit 1
fi
