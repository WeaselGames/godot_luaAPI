#!/usr/bin/env -S godot -s

# This is a standalone script meant to be run via CI
class_name RunTests
extends SceneTree

var time_elapsed = 0
var testCount = 0

var tests: Array[Test]
var doneTests: Array[Test]
var currentTest: Test

func sort_tests(a, b):
	if a.id < b.id:
		return true
	return false

func _init():
	print("LuaAPI Testing framework for v2-alpha")
	print("Starting time: %s\n" % str(time_elapsed))
	load_tests()
	for test in tests:
		test._init()
		testCount += 1
	tests.sort_custom(sort_tests)
	print("Loaded %d tests\n" % testCount)
		
func _process(delta):
	time_elapsed += delta
	var doneCount = 0
	if currentTest == null:
		currentTest = tests.pop_back()
	
	currentTest._process(delta)
	
	if currentTest.done:
		print("Test #%d: " % currentTest.id + currentTest.testName + " has finished.")
		doneTests.append(currentTest)
		currentTest = null
	
	if tests.size() == 0:
		finish()

func finish():
	print("\nFinished!")
	print("End time: %s\n" % str(time_elapsed))
	print("Report:\n")
	var failures = 0
	for test in doneTests:
		print("Test Name: %s" % test.testName)
		print("-------------------------------")
		print("Test id(start order): %d" % test.id)
		print("Test Description:")
		print(test.testDescription)
		print("Frames: %d" % test.frames)
		print("Time: %s" % str(test.time))
		
		if test.status:
			print("Test finished with no errors.")
			print("-------------------------------\n")
			test._finalize()
			test.free()
			continue
			
		failures += 1
		
		print("Test finished with %d errors." % test.errors.size())
		for err in test.errors:
			print("\nERROR %d: " % err.type + err.msg)
		print("-------------------------------\n")
		test._finalize()
		test.free()
	
	doneTests.clear()

	quit(failures)


func load_tests():
	var dir = DirAccess.open("res://tests")
	dir.list_dir_begin()

	while true:
		var file = dir.get_next()
		if file == "":
			break
		elif not file.begins_with(".") and  file.ends_with(".gd"):
			var test = load("res://tests/%s" % file).new()
			tests.append(test)
	
	dir.list_dir_end()
