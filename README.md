 Godot Lua Module
===============
**Table of contents:**
  * [About](#about)
  * [Features](#features)
  * [Nightly Builds](#nightly-builds)
  * [TODO](#todo)
  * [Compiling](#compiling)
  * [Examples](#examples)
  * [Contributing And Feature Requests](#contributing-and-feature-requests)

About
-------
[![🐧 Linux](https://github.com/WeaselGames/lua/actions/workflows/linux.yml/badge.svg)](https://github.com/WeaselGames/lua/actions/workflows/linux.yml) [![🎨 Windows](https://github.com/WeaselGames/lua/actions/workflows/windows.yml/badge.svg)](https://github.com/WeaselGames/lua/actions/workflows/windows.yml) [![🍎 MacOS](https://github.com/WeaselGames/lua/actions/workflows/macos.yml/badge.svg)](https://github.com/WeaselGames/lua/actions/workflows/macos.yml)
![Logo](.github/LuaAPI.png)
Art created by [Alex](https://www.instagram.com/redheadalex1)

***WARNING!!!*** this is a **alpha** version of the module made for Godot v4-alpha. Please see the branch [v1.1-stable](https://github.com/WeaselGames/lua/tree/v1.1-stable) for a more stable build.

This is a Godot engine module that adds lua support via GDScript. Importantly this is **NOT** meant to be a replacement for GDScript. The main purpose of this module is to add runtime execution of code for tasks such as modding or in game scripting. 

To use you can either [Compile from source](#compiling) or you can download one of the [nightly builds](#nightly-builds).

By default the lua print function is set to print to the GDEditor console. This can be changed by exposing your own print function as it will overwrite the existing one.

Features
--------------------------------
- Run lua directly from a string or a text file.
- Push any Variant as a global.
- Call lua functions from GDScript.
- Choose which libraries you want lua to have access to.
- Custom LuaCallable type which allows you to get a lua function as a Callable. See [examples](#examples) below.
- LuaError type which is used to report any errors this module or lua run into.
- LuaThread type which creates a lua thread. This is not a OS thread but a coroutine. 
- Object passed as userdata. See [examples](#examples) below.
- Objects can override most of the lua metamethods. I.E. __index by defining a function with the same name.
- Callables passed as userdata, which allows you to push a Callable as a lua function. see [examples](#examples) below.
- Basic types are passed as userdata (currently: Vector2, Vector3, Color, Rect2, Plane) with a useful metatable. This means you can do things like:  
```lua
local v1 = Vector2(1,2)
local v2 = Vector2(100.52,100.83)
v2 = v2.floor()
print(v2.x) -- "100"
print(v1+v2) -- "(101,102)"
change_my_sprite_color(Color(1,0,0,1)) -- if "change_my_sprite_color" was exposed, in GDScript it will receive a Color variant.
```

If a feature is missing that you would like to see feel free to create a [Feature Request](https://github.com/WeaselGames/lua/issues/new?assignees=&labels=feature%20request&template=feature_request.md&title=) or submit a PR 

Nightly Builds
---------------
- [🐧 Linux Editor](https://nightly.link/WeaselGames/lua/workflows/linux/main/linux-editor.zip)
- [🐧 Linux Template](https://nightly.link/WeaselGames/lua/workflows/linux/main/linux-template-release.zip)
- [🐧 Linux Template Debug](https://nightly.link/WeaselGames/lua/workflows/linux/main/linux-template-debug.zip)
- [🎨 Windows Editor](https://nightly.link/WeaselGames/lua/workflows/windows/main/windows-editor.zip)
- [🎨 Windows Template](https://nightly.link/WeaselGames/lua/workflows/windows/main/windows-template.zip)
- [🎨 Windows Template Debug](https://nightly.link/WeaselGames/lua/workflows/windows/main/windows-template-debug.zip)
- [🍎 MacOS Editor](https://nightly.link/WeaselGames/lua/workflows/macos/main/macos-editor.zip)
- [🍎 MacOS Templates](https://nightly.link/WeaselGames/lua/workflows/macos/main/macos-template.zip)
- [🍎 MacOS Templates Debug](https://nightly.link/WeaselGames/lua/workflows/macos/main/macos-template-debug.zip)

TODO
-----
- Bug fixes
- Finish v2 documentation
- Stability testing
- More up to date todo list on the v2 [project](https://github.com/WeaselGames/lua/projects/1) 

Compiling
------------
This build is for godot 4.0.0-alphaX. X being the latest version. Will not be supporting older alpha builds.
- Start by cloning the Godot 4.0.0-alpha [source](https://github.com/godotengine/godot) with this command `git clone https://github.com/godotengine/godot`

- Next change directories into the modules folder and clone this repo with this command `git clone https://github.com/WeaselGames/lua`

- Now you can follow the Godot build instructions on their [site](https://docs.godotengine.org/en/latest/development/compiling/).

Examples
------------
**Running lua from a string:**
```gdscript
extends Node2D

var lua: LuaAPI

func _ready():
	lua = LuaAPI.new()
	lua.do_string("for i=1,10,1 do print('Hello lua!') end")
```
<br />

**Running lua from a file:**
```gdscript
extends Node2D

var lua: LuaAPI

func _ready():
	lua = LuaAPI.new()
	lua.do_file("user://luaFile.lua")
```
<br />

**Pushing a Variant as a global:**
```gdscript
extends Node2D

var lua: LuaAPI
var test = "Hello lua!"

func _ready():
	lua = LuaAPI.new()
	lua.push_variant(test, "str")
	lua.do_string("print(str)")
```
<br />

**Exposing a GDScript function to lua:**
```gdscript
extends Node2D

var lua: LuaAPI

func luaAdd(a, b):
	return a + b

func _ready():
	lua = LuaAPI.new()
	# Functions are passed the same as any other value to lua.
	lua.push_variant(luaAdd, "add")
	lua.push_variant(func(a, b): return a+b, "addLamda")
	lua.do_string("print(add(2, 4), addLamda(3,3))")
```
<br />

**Calling a lua function from GDScript:**
```gdscript
extends Node2D

var lua: LuaAPI

func _ready():
	lua = LuaAPI.new()
	lua.do_file("user://luaFile.lua")
	if( lua.function_exists("set_colors")):
		# call_function will return a Variant if lua returns nothing the value will be null
		var value = lua.call_function("set_colors", ["red", "blue"])
		if value != null:
			print(value)
		else:
			print("no value returned")	
		
	if( lua.function_exists("set_location")):
		# Assumeing lua defines a function set_location this will return a callable which you can use ot invoke the lua function
		var set_location = lua.pull_variant("set_location")
		var value2 = set_location.call(Vector2(1, 1))
		if value2 != null:
			print(value2)
		else:
			print("no value returned")
```
<br />

**Error handling:**
```gdscript
extends Node2D

var lua: LuaAPI

func test(n: int):
	if n != 5:
		# This will raise a error in the lua state
		return LuaError.new_err("N is not 5 but is %s" % n, LuaError.ERR_RUNTIME)
	return n+5

func _ready():
	lua = LuaAPI.new()
	lua.push_variant(test, "test")
	# Most methods return a LuaError
	# calling test with a type that is not a int would also raise a error.
	var err = lua.do_string("test(6)")
	# the static method is_err will check that the variant type is LuaError and that the errorType is not LuaError.ERR_NONE
	if err is LuaError:
		print("ERROR %d: " % err.type + err.msg)
```
<br />

**Bind libraries:**
```gdscript
extends Node2D

var lua: LuaAPI

func _ready():
	lua = LuaAPI.new()
	#all libraries are avalible. Use OS and IO at your own risk.
	lua.bind_libs(["base", "table", "string"])
```
<br />

**Passing objects as userdata:**
```gdscript
extends Node2D

var lua: LuaAPI

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
	lua = LuaAPI.new()
	player2 = Player.new()
	lua.push_variant(func(): return player2, "getPlayer2")
	lua.expose_constructor(Player, "Player")
	lua.do_string("player = Player() player.move_forward() print(player.pos.x)")
	lua.do_string("player2 = getPlayer2() player2.pos = Vector2(50, 1) print(player2.pos)")
	var player = lua.pull_variant("player")
	print(player.pos)
	print(player2.pos)
```

**Object metamethod overrides:**
```gdscript
extends Node2D

var lua: LuaAPI

class Player:
	var pos = Vector2(1, 0)
	# Most metamethods can be overriden. The function names are the same as the metamethods.
	func __index(index):
		if index=="pos":
			return pos
		else:
			return LuaError.new_err("Invalid index '%s'" % index)
	func move_forward():
		pos.x+=1

func _ready():
	lua = LuaAPI.new()
	lua.expose_constructor(Player, "Player")
	var err = lua.do_string("player = Player() print(player.pos.x)  player.move_forward() -- this will cause our custom error ")
	if err is LuaError:
		print(err.msg)
	var player = lua.pull_variant("player")
	print(player.pos)
```
**Using Coroutines:**
```gdscript
extends Node2D

var lua: LuaAPI
var thread: LuaThread
	
func _ready():
	lua = LuaAPI.new()
	# Despite the name this is not like a OS thread. It is a coroutine
	thread = LuaThread.new_thread(lua)
	thread.load_string("
	while true do
		-- yield is exposed to lua when the thread is bound.
		yield(1)
		print('Hello world!')
	end
	")
	
var yieldTime = 0
var timeSince = 0
func _process(delta):
	timeSince += delta
	if thread.is_done() || timeSince <= yieldTime:
		return
	# thread.resume will either return a LuaError or a Array.
	var ret = thread.resume()
	if ret is LuaError:
		print("ERROR %d: " % ret.type + ret.msg)
		return
	yieldTime = ret[0]
	timeSince = 0
```
Contributing And Feature Requests
---------------
All contributions are welcome, if you would like to contribute submit a PR.
<br />
Additionally if you do not have the time and or the knowledge you can create a [Feature Request](https://github.com/WeaselGames/lua/issues/new?assignees=&labels=feature%20request&template=feature_request.md&title=)

[![lua logo](https://www.lua.org/images/powered-by-lua.gif)](https://www.lua.org/)
