extends Node2D

var lua: LuaAPI

func _ready():
	lua = LuaAPI.new()
	lua.do_string("print('Hello Lua!')")
