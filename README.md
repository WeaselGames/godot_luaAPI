

 Godot Lua Module
===============
**Table of contents:**
  * [About](#about)
  * [Compiling](#compiling)
  * [Features](#features)
  * [Examples](#examples)
  * [Contributing And Feature Requests](#contributing-and-feature-requests)

About
-------
This is a Godot engine module that adds lua support via GDScript. Importantly this is **NOT** meant to be a replacment for GDScript. The main purpose of this module is to add runtime execution of code for tasks such as modding or in game scripting. 

While the original purpose of this module was for my own use I understand more may find it useful. If a feature is missing that you would like to see feel free to create a [Feature Request](https://github.com/Trey2k/lua/issues/new?assignees=&labels=feature%20request&template=feature_request.md&title=) or submit a PR 

Features
--------------------------------
- Run lua directly from a string or a text file.
- Push any Variant as a global.
- Expose GDScript functions to lua with a return value and up to 5 arguments.
- By default the lua print function is set to print to the GDEditor console. This can be changed by exposing your own print function as it will overwrite the existing one.

Compiling
------------
  To use this module you will need to compile the Godot engine from source.
- Start by cloning the current stable release of the Godot [source](https://github.com/godotengine/godot/tree/3.2.3-stable).

 - Next clone this repository into the module folder inside the Godot source code.

- Then, go into the lua/luasrc folder with your terminal and Run `make [linux|windows|macos] test`. 
  - Please note as of now the build has only been tested on Linux. If you are on windows you will need to download the [mingw](https://sourceforge.net/projects/mingw/) tools.
- After you have finished building the lua source code you can follow the Godot build instructions on their [site](https://docs.godotengine.org/en/latest/development/compiling/).

Examples
------------
**Running lua from a string:**
```
extends Node2D

onready var lua = Lua.new()

func _ready():
	lua.doString("for i=1,10,1 do print('Hello lua!') end")
```
<br />

**Running lua from a file:**
```
extends Node2D

onready var lua = Lua.new()

func _ready():
	lua.doFile("user://luaFile.lua")
```
<br />

**Pushing a Variant as a global:**
```
extends Node2D

onready var lua = Lua.new()
var test = "Hello lua!"

func _ready():
	lua.pushVariant(test, "str")
	lua.doString("print(str)")
```
<br />

**Exposing a GDScript function to lua:**
```
extends Node2D

onready var lua = Lua.new()

func luaAdd(a, b):
	return a + b

func _ready():
	lua.exposeFunction(self, "luaAdd", "add")
	lua.doString("print(add(2, 4))")
```
<br />

**Capturing lua errors:**
```
extends Node2D

onready var lua = Lua.new()

func _ready():
	var err = lua.doString("print(This wont work)")
	if err != null:
		print(err)
```
Contributing And Feature Requests
---------------
All contributions are welcome, if you would like to contribute submit a PR.
<br />
Additionally if you do not have the time and or the knowledge you can create a [Feature Request](https://github.com/Trey2k/lua/issues/new?assignees=&labels=feature%20request&template=feature_request.md&title=)
