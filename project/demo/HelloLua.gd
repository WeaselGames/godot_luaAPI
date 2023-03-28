extends Node2D

var lua: LuaAPI

class Test:
	var a: String

func _ready():
	lua = LuaAPI.new()
	lua.do_string('print("Hello lua!")')
