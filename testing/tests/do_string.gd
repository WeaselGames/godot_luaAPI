#class_name do_string
extends Test
var lua: LuaAPI

func _init():
	super._init()
	lua = LuaAPI.new()
	
	testName = "LuaAPI.do_string()"
	testDescription = "
Runs the fibonacci sequence of 5. 
No return value is captured as pull_variant/push_variant have not been tested yet.
"

func _process(delta):
	super._process(delta)
	
	var err = lua.do_string("
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
		status = false
	lua = null
	done = true
