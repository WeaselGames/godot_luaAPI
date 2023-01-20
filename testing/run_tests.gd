#!/usr/bin/env -S godot -s

# This is a standalone script meant to be run via CI
class_name RunTests
extends SceneTree

var time_elapsed = 0
var testCount = 0

var tests: Array[Test]
var doneTests: Array[Test]

func _init():
	print("LuaAPI Testing framework for v2-alpha")
	print("Starting time: %s\n\n" % str(time_elapsed))
	load_tests()
	for test in tests:
		test._init()
		testCount += 1
	print("Loaded %d tests" % testCount)
		
func _process(delta):
	time_elapsed += delta
	var doneCount = 0
	
	for test in tests:
		test._process(delta)
		if test.done:
			doneTests.append(test)
			tests.erase(test)
			
	if tests.size() == 0:
		finish()
	
func finish():
	print("Finished:")
	print("End time: %s\n\n" % str(time_elapsed))
	print("Report:")
	var failures = 0
	for test in doneTests:
		print("-------------------------------")
		print("Test Name: %s" % test.testName)
		print("Test Description:")
		print(test.testDescription)
		print("Frames: %d" % test.frames)
		print("Secounds: %s" % str(test.time))
		
		if test.status:
			print("Test finished with no errors.")
			print("-------------------------------")
			doneTests.erase(test)
			test.free()
			continue
			
		failures += 1
		
		print("Test finished with %d errors." % test.errors.size())
		for err in test.errors:
			print("\nERROR %d: " % err.type + err.msg)
		print("-------------------------------")
			
		doneTests.erase(test)
		test.free()

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
