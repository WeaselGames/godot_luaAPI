Godot Lua API C# Examples
===============

All future examples will be in the [Wiki](https://luaapi.weaselgames.info/latest).

Getting Started Example (In C#)
-------

In this example, we recreate the main GDScript example from the README.md file. I have fully commented this 
example, to explain what each part does. Specific differences and a special note on this example: In C# you 
cannot assign a Method to a `Variant` for use with `LuaAPI.PushVariant`. So, to get around that, and make the 
example work, we will first wrap our called function in a `Callable` variable (wrapper). Please note the line 
`Callable print = new Callable(this, MethodName.LuaPrint);` in the example below.

```csharp
using Godot;

public partial class Node2D : Godot.Node2D {
	private LuaApi lua = new LuaApi();

	public void LuaPrint(string message) {
		GD.Print(message);
	}

	public void _Ready() {
		GD.Print("Starting Node2D.cs.");

		// All builtin libraries are available to bind with. Use OS and IO at your own risk.
		// BindLibraries requires a "Godot Array" so, let's build one.
		Godot.Collections.Array libraries = new Godot.Collections.Array();
		libraries.Add("base");   // Base Lua commands
		libraries.Add("table");  // Table functionality.
		libraries.Add("string"); // String Specific functionality.

		lua.BindLibraries(libraries); // Assign the specified libraries to the LuaAPI object.

		// In C#, .PushVariant does not work with Methods, so we use Callable to wrap our function.
		Callable print = new Callable(this, MethodName.LuaPrint);
		// Assign the Callable, so that the API can call our function.
		// Note, the lua function "cs_print" is now callable within Lua script.
		lua.PushVariant("cs_print", print);
		// Assign a Lua Variable named "message" and give it a value.
		lua.PushVariant("message", "Hello lua!");

		// Use .DoString() to execute our Lua code.
		LuaError error = lua.DoString("cs_print(message)");
		// Check for errors, and if there are any, Print them to the Godot Console.
		if (error != null && error.Message != "") {
			GD.Print("An error occurred calling DoString.");
			GD.Print("ERROR %d: %s", error.Type, error.Message);
		}

		error = lua.DoString(@"
                                  for i=1,10,1 do
                                  	cs_print(message)
                                  end
                                  function get_message()
                                  	return ""This message was sent from 'get_message()'""
                                  end
                                  ");

		// Check for errors, and if there are any, Print them to the Godot Console.
		if (error != null && error.Message != "") {
			GD.Print("An error occurred calling DoString.");
			GD.Print("ERROR %d: %s", error.Type, error.Message);
		}

		var val = lua.PullVariant("get_message");

		// Check to see if it returned an error, or a value.
		if (val.GetType() == typeof(LuaError)) {
			GD.Print("ERROR %d: %s", error.Type, error.Message);
			return;
		}

		// LuaAPI.CallFunction requires a Godot.Collections.Array as the container
		// for the parameters passed in, for the lua function. 
		Godot.Collections.Array Params = new Godot.Collections.Array();

		// We use .CallFunction to actually call the lua function within the Lua State.
		var message = lua.CallFunction("get_message", Params);
		// And, finally, we log the output of the function to Godot Output Console.
		GD.Print(message);
	}
}
```