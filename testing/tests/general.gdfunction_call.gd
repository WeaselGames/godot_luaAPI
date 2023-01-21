extends UnitTest
var lua: LuaAPI

func lua_get_frames()->int:
	return frames

func _init():
	# Since we are using poly here, we need to make sure to call super for _methods
	super._init()
	# id will determine the load order
	id = 9850

	lua = LuaAPI.new()
	
	var err = lua.push_variant("getFrames", lua_get_frames)
	if err is LuaError:
		errors.append(err)
		fail()
	
	# testName and testDescription are for any needed context about the test.
	testName = "General.gdfunction_call"
	testDescription = "
Runs the fibonacci sequence of 15 and verifies the result.
Number is passed via push_variant.
"

func fail():
	status = false
	done = true

func _process(delta):
	# Since we are using poly here, we need to make sure to call super for _methods
	super._process(delta)
	
	var err = lua.do_string("result = getFrames()")
	if err is LuaError:
		errors.append(err)
		return fail()
		
	var result = lua.pull_variant("result")
	if result is LuaError:
		errors.append(err)
		return fail()
		
	if not result is float:
		errors.append(LuaError.new_error("Result is not float but is '%d'" % typeof(result), LuaError.ERR_TYPE))
		return fail()
		
	if not result == frames:
		errors.append(LuaError.new_error("Result is not 610 but is '%d'" % result))
		return fail()
		
	if frames == 200:
		done = true
