# Our template test class
extends Node

var done = false
var errors: Array[LuaError]
var status: bool = true
var time: float
var frames: int

var testName = "Test"
var testDescription = "Base test for all other test's to inhirt from for poly"

# Called when the node enters the scene tree for the first time.
func _init():
	pass

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	time += delta
	frames += 1

func _finalize():
	pass
