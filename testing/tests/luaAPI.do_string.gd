#class_name do_string
extends Test
var lua: LuaAPI

func _init():
	# Since we are using poly here, we need to make sure to call super for _methods
	super._init()
	lua = LuaAPI.new()
	
	# testName and testDescription are for any needed context about the test.
	testName = "LuaAPI.do_string()"
	testDescription = "
Runs the fibonacci sequence of 5. 
No return value is captured as pull_variant/push_variant have not been tested yet.
"

func _process(delta):
	# Since we are using poly here, we need to make sure to call super for _methods
	super._process(delta)
	
	var err = lua.do_string("TEST_FAIL()
	function Fib(n)
	  local function inner(m)
		if m < 2 then
		  return m
		end
		return inner(m-1) + inner(m-2)
	  end
	  return inner(n)
	end
	
	result = Fib(5)
	")
	
	if err is LuaError:
		errors.append(err)
		# Status is true by default, once the test determines a failure state it will set status to false.
		status = false
	lua = null

	# Once done is set to true, the test's _process function will no longer be called.
	done = true
