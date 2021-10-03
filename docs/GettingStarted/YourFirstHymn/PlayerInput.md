\page YourFirstHymn_PlayerInput Player %Input

You now have a camera entity that moves to the right through script. But to actually let the player control it, you need to handle player input.

\section MousePosition Mouse position
The global function `vec2 %GetCursorXY()` returns the mouse's current position. By comparing this to the previous frame's position we can implement mouse looking.

Add a member variable to store the position from the previous frame.

    vec2 mousePosition;

Then in the `Update` method we can get the difference compared to the previous frame, and then update the previous position for next frame.

    // Get mouse position compared to previous frame's.
    vec2 newPosition = GetCursorXY();
    vec2 mouseDiff = newPosition - mousePosition;
    mousePosition = newPosition;

Using the mouse movement, let's change the rotation of the camera. First we need to store the rotation of the camera. We will only consider yaw (horizontal movement) and pitch (vertical movement) and ignore roll.

    float yaw = 0.0f;
    float pitch = 0.0f;

Now in the `Update` method we can update these based on the `mouseDiff`.

    yaw -= mouseDiff.x;
    pitch -= mouseDiff.y;

Adding a full radian for every pixel the mouse moves is a lot. This will make the camera uncontrollable, so let's consider mouse sensitivity. Add another member variable:

    float mouseSensitivity = 0.01f;

Then scale the yaw and pitch changes based on the sensitivity.

    yaw -= mouseDiff.x * mouseSensitivity;
    pitch -= mouseDiff.y * mouseSensitivity;

You now have the desired yaw and pitch. The only thing left to do is to set the entity's rotation based on these values. However, Hymn to Beauty uses quaternions, not Euler angles. To set the rotation, we first need to convert these Euler angles to quaternions.

    // Calculate quaternion from Euler angles.
    vec3 yawAxis = vec3(0.0f, 1.0f, 0.0f);
    yawAxis *= sin(yaw * 0.5f);
    quat yawRotation = quat(cos(yaw * 0.5f), yawAxis.x, yawAxis.y, yawAxis.z);
    
    vec3 pitchAxis = vec3(1.0f, 0.0f, 0.0f);
    pitchAxis *= sin(pitch * 0.5f);
    quat pitchRotation = quat(cos(pitch * 0.5f), pitchAxis.x, pitchAxis.y, pitchAxis.z);
    
    self.rotation = yawRotation * pitchRotation;

The final code looks like this.

    class FirstPersonController {
        Entity @self;
        vec2 mousePosition;
        float mouseSensitivity = 0.01f;
        float yaw = 0.0f;
        float pitch = 0.0f;

        FirstPersonController(Entity @entity){
            @self = @entity;
            
            mousePosition = GetCursorXY();

            // Remove this if updates are not desired.
            RegisterUpdate();
        }

        // Called by the engine for each frame.
        void Update(float deltaTime) {
            // Get mouse position compared to previous frame's.
            vec2 newPosition = GetCursorXY();
            vec2 mouseDiff = newPosition - mousePosition;
            mousePosition = newPosition;
            
            // Mouse look.
            yaw -= mouseDiff.x * mouseSensitivity;
            pitch -= mouseDiff.y * mouseSensitivity;
            
            // Calculate quaternion from Euler angles.
            vec3 yawAxis = vec3(0.0f, 1.0f, 0.0f);
            yawAxis *= sin(yaw * 0.5f);
            quat yawRotation = quat(cos(yaw * 0.5f), yawAxis.x, yawAxis.y, yawAxis.z);
            
            vec3 pitchAxis = vec3(1.0f, 0.0f, 0.0f);
            pitchAxis *= sin(pitch * 0.5f);
            quat pitchRotation = quat(cos(pitch * 0.5f), pitchAxis.x, pitchAxis.y, pitchAxis.z);
            
            self.rotation = yawRotation * pitchRotation;
        }
    }

Try it out by playing the hymn and looking around by moving your mouse.

\section ButtonInput Button input

Next you'll implement standard WASD movement. But rather than checking if the W key has been pressed in order to move forward, you'll define a "forward" button, and assign that to W. The advantage of this approach is that the keys can be reassigned, for instance if you're using an AZERTY keyboard you can assign the forward button to Z instead, without having to change any of your scripts.

## Defining an input
Press on `Hymn` and select `%Input`.

\image html YourFirstHymn/HymnInput.png

This brings up the %Input window.

\image html YourFirstHymn/InputWindow.png

This is the window you use to define all the inputs for your hymn. To add a new button, click on `Add Button`.

You will need to define three things:

1. The name of the action. This name has to be unique and is what you will use in scripts.
2. The key assigned to the action. See the list of values [here](https://www.glfw.org/docs/3.3/group__keys.html).
3. The state the key should be in to trigger the action. 0 means the key is released. 1 means the key is pressed. 2 means the key is repeated.

\image html YourFirstHymn/WASD.png

## Checking button input in script

We can check for button inputs using the `bool %Input(input)` function.

Begin by defining a member variable for the movement speed.

    float movementSpeed = 2.0f;

Next, at the end of the `Update` method, figure out what directions we should move in when WASD are pressed.

    // Calculate forward and right vectors.
    float forwardAngle = yaw + radians(90);
    vec3 forwardDir = vec3(cos(forwardAngle), 0.0, sin(-forwardAngle));
    float rightAngle = yaw;
    vec3 rightDir = vec3(cos(rightAngle), 0.0, sin(-rightAngle));

Query the input:

    // Get WASD movement.
    float forwardMovement = (Input(forward) ? 1.0f : 0.0f) - (Input(backward) ? 1.0f : 0.0f);
    float rightMovement = (Input(right) ? 1.0f : 0.0f) - (Input(left) ? 1.0f : 0.0f);

Angelscript does not allow casting between `bool` and `float` so use inline `if`s to do the casting.

Finally, perform the movement.

    // Move.
    self.position += forwardDir * forwardMovement * movementSpeed * deltaTime;
    self.position += rightDir * rightMovement * movementSpeed * deltaTime;

You have now implemented standard first person movement.