extends UnitTest
var lua: LuaAPI

class TestObject:
	var a: String

func _init():
	# Since we are using poly here, we need to make sure to call super for _methods
	super._init()
	# id will determine the load order
	id = 9950
	
	lua = LuaAPI.new()
	var err = lua.expose_constructor("TestObj", TestObject)
	if err is LuaError:
		errors.append(err)
		fail()
		
	# testName and testDescription are for any needed context about the test.
	testName = "LuaAPI.expose_constructor"
	testDescription = "
Test LuaAPI.expose_constructor.
Exoposes a object constructor for TestObject which contains one variable. A string
lua calls the constructor and then sets the string to 'Hello from lua!'.
We also test pulling it back to GD and confirm the contents of the string.
"

func fail():
	status = false
	done = true

func _process(delta):
	# Since we are using poly here, we need to make sure to call super for _methods
	super._process(delta)
	
	var err = lua.do_string("testObj = TestObj() testObj.a = 'Hello from lua!'")
	if err is LuaError:
		errors.append(err)
		return fail()
	
	var testObj = lua.pull_variant("testObj")
	if testObj is LuaError:
		errors.append(err)
		return fail()
		
	if not testObj is TestObject:
		errors.append(LuaError.new_error("testObj is not TestObject but is '%d'" % typeof(testObj)))
		return fail()
	
	if not testObj.a == "Hello from lua!":
		errors.append(LuaError.new_error("testObj.a is not 'Hello from lua!' but is '%s'" % testObj.a))
		return fail()
		
	done = true
