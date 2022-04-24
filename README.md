

 Godot Lua Module
===============
**Table of contents:**
  * [About](#about)
  * [Features](#features)
  * [TODO](#todo)
  * [Compiling](#compiling)
  * [Examples](#examples)
  * [Contributing And Feature Requests](#contributing-and-feature-requests)

About
-------
[![🐧 Linux](https://github.com/Trey2k/lua/actions/workflows/linux.yml/badge.svg)](https://github.com/Trey2k/lua/actions/workflows/linux.yml) [![🎨 Windows](https://github.com/Trey2k/lua/actions/workflows/windows.yml/badge.svg)](https://github.com/Trey2k/lua/actions/workflows/windows.yml) [![🌐 javascript](https://github.com/Trey2k/lua/actions/workflows/javascript.yml/badge.svg)](https://github.com/Trey2k/lua/actions/workflows/javascript.yml)

***WARNING!!!*** this is a **alpha** version of the module. Please see the branch [v1.1-stable](https://github.com/Trey2k/lua/tree/v1.1-stable) for a more stable build.

This is a Godot engine module that adds lua support via GDScript. Importantly this is **NOT** meant to be a replacement for GDScript. The main purpose of this module is to add runtime execution of code for tasks such as modding or in game scripting. 

While the original purpose of this module was for my own use I understand more may find it useful. If a feature is missing that you would like to see feel free to create a [Feature Request](https://github.com/Trey2k/lua/issues/new?assignees=&labels=feature%20request&template=feature_request.md&title=) or submit a PR 

To use you can either [Compile from source](#compiling) or if you are on windows or linux you can download one of the pre built binarays from the [releas](https://github.com/Trey2k/lua/releases) page.

By default the lua print function is set to print to the GDEditor console. This can be changed by exposing your own print function as it will overwrite the existing one.

Features
--------------------------------
- Run lua directly from a string or a text file.
- Push any Variant as a global.
- Expose GDScript functions to lua with a return value and up to 5 arguments.
- Call lua functions from GDScript.
- Choose which libraries you wont lua to have access to.
- Object passed as userdata see [examples](#examples) below.
- Basic types are passed as userdata (currently: Vector2, Vector3, Color, Rect2, Plane) with a useful metatable. This means you can do things like:  
```lua
local v1 = Vector2(1,2)
local v2 = Vector2(100.52,100.83)
v2 = v2.floor()
print( v2.x ) -- "100"
print( v1+v2 ) -- "(101,102)"
change_my_sprite_color( Color(1,0,0,1) ) -- if "change_my_sprite_color" was exposed, in GDScript it will receive a Color variant.
```

TODO
-----
- Finish v2 documentation
- Add the rest of the userdata functions. Vector2 is the only complete one.
- More up to date todo list on the v2 [project](https://github.com/Trey2k/lua/projects/1) 

Compiling
------------
This build is for godot 4.0.0-alpha.
- Start by cloning the Godot 4.0.0-alpha [source](https://github.com/godotengine/godot) with this command `git clone https://github.com/godotengine/godot`

- Next change directories into the modules folder and clone this repo with this command `git clone https://github.com/Trey2k/lua`

- Now you can follow the Godot build instructions on their [site](https://docs.godotengine.org/en/latest/development/compiling/).

Examples
------------
**Running lua from a string:**
```gdscript
extends Node2D

lua = Lua.new()

func _ready():
	lua = Lua.new()
	lua.add_string("for i=1,10,1 do print('Hello lua!') end")
	lua.execute()
```
<br />

**Running lua from a file:**
```gdscript
extends Node2D

lua = Lua.new()

func _ready():
	lua = Lua.new()
	lua.add_file("user://luaFile.lua")
	lua.execute()
```
<br />

**Pushing a Variant as a global:**
```gdscript
extends Node2D

var lua: Lua
var test = "Hello lua!"

func _ready():
	lua = Lua.new()
	lua.push_variant(test, "str")
	lua.add_string("print(str)")
	lua.execute()
```
<br />

**Exposing a GDScript function to lua:**
```gdscript
extends Node2D

var lua: Lua

func luaAdd(a, b):
	return a + b

func _ready():
	lua = Lua.new()
	lua.expose_function(luaAdd, "add")
	lua.expose_function(func(a, b): return a+b, "addLamda")
	lua.add_string("print(add(2, 4), addLamda(3,3))")
	lua.execute()
```
<br />

**Calling a lua function from GDScript:**
```gdscript
extends Node2D

var lua: Lua

func _ready():
	lua = Lua.new()
	lua.add_file("user://luaFile.lua")
	lua.execute()
	if( lua.lua_function_exists("set_colors") ):
		# call_function will return a Variant if lua returns nothing the value will be null
		var value = lua.call_function( "set_colors", ["red", "blue"])
		if value != null:
			print(value)
		else:
			print("no value returned")	
```
<br />

**Capturing lua errors:**
```gdscript
extends Node2D

var lua: Lua

func luaCallBack(err):
	print(err)

func _ready():
	lua = Lua.new()
	lua.add_stringg("print(This wont work)")
	lua.set_error_handler(luaCallBack)
	lua.execute()
```
<br />

**Bind libraries:**
```gdscript
extends Node2D

var lua: Lua

func _ready():
	lua = Lua.new()
	#all libraries are avalible. Use OS and IO at your own risk.
	lua.bind_libs(["base", "table", "string"])
```
<br />

**Passing objects as userdata:**
```gdscript
extends Node2D
var lua: Lua
class Player:
	var pos = Vector2(0, 0)
	#If lua_funcs is not defined or returns a empty array, all functions will be aval
	func lua_funcs():
		return ["move_forward"]
	#lua_fields behaves the same as lua_funcs but for fields.
	func lua_fields():
		return ["pos"]
	func move_forward():
		pos.x+=1

var player2: Player

func _ready():
	lua = Lua.new()
	player2 = Player.new()
	lua.expose_function(func(): return player2, "getPlayer2")
	lua.expose_constructor(Player, "Player")
	lua.add_string("player = Player() player.move_forward() print(player.pos.x)")
	lua.execute()
	lua.add_string("player2 = getPlayer2() player2.pos = Vector2(50, 1) print(player2.pos)")
	lua.execute()
	var player = lua.pull_variant("player")
	print(player.pos)
	print(player2.pos)
```
Contributing And Feature Requests
---------------
All contributions are welcome, if you would like to contribute submit a PR.
<br />
Additionally if you do not have the time and or the knowledge you can create a [Feature Request](https://github.com/Trey2k/lua/issues/new?assignees=&labels=feature%20request&template=feature_request.md&title=)

[![lua logo](https://www.lua.org/images/powered-by-lua.gif)](https://www.lua.org/)
