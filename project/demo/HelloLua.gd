extends Node

var lua: LuaAPI = LuaAPI.new()

func _lua_print(message: String):
	print(message)
	return LuaError.new_error("test", LuaError.ERR_RUNTIME)

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
	print(err)
	if err is LuaError:
		print("ERROR %d: %s" % [err.type, err.message])
		return
	
	
	
