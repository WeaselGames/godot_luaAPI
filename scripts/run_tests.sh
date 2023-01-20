#!/bin/bash -e
./scripts/godot/bin/godot.linuxbsd.editor.x86_64.luaAPI --headless --path ../testing -s run_tests.gd 2>&1 | tee ../report.txt