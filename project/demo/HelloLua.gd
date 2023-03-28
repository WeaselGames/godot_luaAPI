extends Node

var lua: LuaAPI = LuaAPI.new()

func _lua_print(message: String) -> void:
	print(message)

func _ready():
	lua.push_variant("print", _lua_print)
	lua.push_variant("message", "Hello lua!")
	
	# All builtin libraries are available to bind with. Use OS and IO at your own risk.
	lua.bind_libraries(["base", "table", "string"])
	
	# Most methods return a LuaError in case of an error
	var err: LuaError = lua.do_string("""
	for i=1,10,1 do
		print(message)
	end
	function get_message()
		return "Hello gdScript!"
	end
	""")
	if err is LuaError:
		print("ERROR %d: %s" % [err.type, err.message])
		return
	
	var val = lua.pull_variant("get_message")
	if val is LuaError:
		print("ERROR %d: %s" % [err.type, err.message])
		return
	
	var message = val.call()
	print(message)