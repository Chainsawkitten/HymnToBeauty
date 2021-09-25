\page FunctionReference Function Reference

The scripting language contains the following global functions:

`void print(const string& in)`  
Display `in` in the console.

`vec2 GetCursorXY()`  
Get the mouse cursor's coordinates in an (x,y) vector.

`bool Input(input button)`  
Gets whether the input `button` has been activated. Inputs are defined in the hymn.

`void RegisterUpdate()`  
Register the current entity to receive update events. (The `void Update(float deltaTime)` method will be called each frame.)

`void SendMessage(Entity@ recipient, int type)`  
Send a message to `recipient`. (The `void ReceiveMessage(int type)` method will be called.)

`Hub@ Managers()`  
Get a hub containing all the subsystems.

`void RegisterTriggerEnter(Component::Physics@ trigger, Component::Physics@ listener, string method)`  
Set up a listener method with name `method` to call when `listener` begins intersecting the volume defined by `trigger`. The listener method takes the trigger as first parameter and the listener object as second.

`void RegisterTriggerRetain(Component::Physics@ trigger, Component::Physics@ listener, string method)`  
Set up a listener method with name `method` to call while `listener` is intersecting the volume defined by `trigger`. The listener method takes the trigger as first parameter and the listener object as second.

`void RegisterTriggerLeave(Component::Physics@ trigger, Component::Physics@ listener, string method)`  
Set up a listener method with name `method` to call when `listener` stops intersecting the volume defined by `trigger`. The listener method takes the trigger as first parameter and the listener object as second.

`void RestartScene()`  
Restarts the scene from the most recently saved state. 
