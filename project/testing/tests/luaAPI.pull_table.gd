extends UnitTest
var lua: LuaAPI

func _ready():
	# Since we are using poly here, we need to make sure to call super for _methods
	super._ready()
	# id will determine the load order
	id = 9975

	lua = LuaAPI.new()

	# testName and testDescription are for any needed context about the test.
	testName = "LuaAPI.pull_table"
	testDescription = "
Pulls a table made in Lua and verifies the result is a valid dictionary
"

func fail():
	status = false
	done = true

func _process(delta):
	# Since we are using poly here, we need to make sure to call super for _methods
	super._process(delta)

	var err = lua.do_string("""
	result = {
		a = "test",
		b = 42,
		c = false,
		d = { }
	}
	""")

	if err is LuaError:
		errors.append(err)
		return fail()

	var result = lua.pull_variant("result")
	if result is LuaError:
		errors.append(err)
		return fail()

	if not result is Dictionary:
		errors.append(LuaError.new_error("Result is not a Dictionary but is '%d'" % typeof(result), LuaError.ERR_TYPE))
		return fail()
	
	if !result.has("a"):
		errors.append(LuaError.new_error("Result doesn't contain 'a'", LuaError.ERR_TYPE))
		return fail()
	if result["a"] != "test":
		errors.append(LuaError.new_error("result['a'] is not test but is %s" % result["a"], LuaError.ERR_TYPE))
		return fail()
		
	if !result.has("b"):
		errors.append(LuaError.new_error("Result doesn't contain 'b'", LuaError.ERR_TYPE))
		return fail()
	if result["b"] != 42:
		errors.append(LuaError.new_error("result['a'] is not 42 but is %s" % result["b"], LuaError.ERR_TYPE))
		return fail()
		
	if !result.has("c"):
		errors.append(LuaError.new_error("Result doesn't contain 'c'", LuaError.ERR_TYPE))
		return fail()
	if result["c"] != false:
		errors.append(LuaError.new_error("result['c'] is not false but is %s" % result["b"], LuaError.ERR_TYPE))
		return fail()
		
	if !result.has("c"):
		errors.append(LuaError.new_error("Result doesn't contain 'c'", LuaError.ERR_TYPE))
		return fail()
	if result["c"] != false:
		errors.append(LuaError.new_error("result['c'] is not false but is %s" % result["b"], LuaError.ERR_TYPE))
		return fail()
		
	if !result.has("d"):
		errors.append(LuaError.new_error("Result doesn't contain 'd'", LuaError.ERR_TYPE))
		return fail()
	if not result["d"] is Dictionary:
		errors.append(LuaError.new_error("result['d'] is not a dictionary but is %s" % typeof(result["d"]), LuaError.ERR_TYPE))
		return fail()

	# Once done is set to true, the test's _process function will no longer be called.
	done = true
