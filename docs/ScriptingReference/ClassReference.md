\page ClassReference Class Reference

The following classes can be used in scripts.

\section Entity
Refers to an entity in the scene. Contains various components.

## Methods
`void Kill()`  
Kill the entity and its children. A killed entity will be removed from the scene at the end of the frame.

`bool IsKilled() const`  
Get whether the entity has been killed.

`void SetEnabled(bool enabled, bool recursive)`  
Set whether the entity (and its children) should be enabled.

`bool IsEnabled() const`  
Get whether the entity is enabled.

`Entity@ GetParent() const`  
Get the entity's parent entity if it has any. Otherwise returns null.

`Entity@ AddChild(const string& name)`  
Add a new child called `name` to this entity.

`Entity@ InstantiateScene(const string& name)`  
Instantiate a scene as a child to this entity.

`bool IsScene() const`  
Get whether this entity is an instantiated scene.

`Entity@ GetChild(const string& name) const`  
Get child based on its name. Returns null if no child with that name was found.

`Entity@ GetChildFromIndex(int index) const`  
Get child based on its index in child array. Returns null if no child with that name was found.

`uint GetNumChildren() const`  
Get the number of children the entity has.

`void RotateYaw(float angle)`  
Rotate the entity around the Y axis.

`void RotatePitch(float angle)`  
Rotate the entity around the X axis.

`void RotateRoll(float angle)`  
Rotate the entity around the Z axis.

`void RotateAroundWorldAxis(float angle, const vec3& axis)`  
Rotate quaternion style.

`void SetWorldOrientation(quat orientation)`  
Set the orientation in world space.

`void SetLocalOrientation(quat orientation)`  
Set the orientation in local space.

`Component::DirectionalLight@ GetDirectionalLight()`  
Get the entity's directional light component, if it has any. Otherwise returns null.

`Component::Camera@ GetCamera()`  
Get the entity's camera component, if it has any. Otherwise returns null.

`Component::Listener@ GetListener()`  
Get the entity's listener component, if it has any. Otherwise returns null.

`Component::Mesh@ GetMesh()`  
Get the entity's mesh component, if it has any. Otherwise returns null.

`Component::PointLight@ GetPointLight()`  
Get the entity's point light component, if it has any. Otherwise returns null.

`Component::RigidBody@ GetRigidBody()`  
Get the entity's rigid body component, if it has any. Otherwise returns null.

`Component::SoundSource@ GetSoundSource()`  
Get the entity's sound source component, if it has any. Otherwise returns null.

`Component::SpotLight@ GetSpotLight()`  
Get the entity's spot light component, if it has any. Otherwise returns null.

`Component::Sprite@ GetSprite()`  
Get the entity's sprite component, if it has any. Otherwise returns null.

## Properties
`string name`  
The name of the entity.

`vec3 position`  
Position relative to the parent entity.

`vec3 scale`  
Scale.

`quat rotation`  
Rotation.

\section DirectionalLight Component::DirectionalLight
A light that is infinitely far away and thus casts parallel rays. Like the sun.

## Properties
`vec3 color`  
The light color.

`float ambientCoefficient`  
Coefficient of ambient lighting (between 0.0 and 1.0).

\section Camera Component::Camera
A camera through which the world can be rendered.

## Properties
`bool orthographic`  
Whether the camera should have an orthographic projection. Otherwise a perspective projection is used.

`float fieldOfView`  
Field of view, in degrees. Only applies to perspective projection.

`float size`  
The size of the view. Only applies to orthographic projection.

`float zNear`  
Near plane.

`float zFar`  
Far plane.

`vec4 viewport`  
Where on the screen the camera should be rendered.

`int order`  
The order in the camera stack. Cameras with a lower value are rendered first.

`bool overlay`  
Whether the camera should be rendered with a transparent background.

`uint layerMask`  
Which layers the camera should show.

\section Listener Component::Listener
A sound receptor.

\section Mesh Component::Mesh
A 3D mesh.

## Properties

`uint layerMask`  
Which layers the mesh should be rendered to.

\section PointLight Component::PointLight
A light emanating from a singular position in space.

## Properties
`vec3 color`  
The light color.

`float attenuation`  
How fast the light loses intensity over distance.

`float intensity`  
Intensity.

\section RigidBody Component::RigidBody
Allows interacting with other physics components.

\section SoundSource Component::SoundSource
The source of a 3D positional sound.

## Properties
`float volume`  
The volume of the sound.

`bool loop`  
Whether the sound should loop.

## Methods
`void Play()`  
Play the sound.

`void Pause()`  
Pause the sound.

`void Stop()`  
Stop the sound.

`bool IsPlaying()`
Whether the sound is currently playing or not.

\section SpotLight Component::SpotLight
Similar to a point light but only lights an angle around its direction.

## Properties
`vec3 color`  
The light color.

`float ambientCoefficient`  
Coefficient of ambient lighting (between 0.0 and 1.0).

`float attenuation`  
How fast the light loses intensity over distance.

`float intensity`  
Intensity.

`float coneAngle`  
Angle of the cone (in degrees).

\section Sprite Component::Sprite
A 2D sprite.

## Properties
`float pixelsPerUnit`  
How many pixels go into one world unit (how large the sprite will be)

`vec2 pivot`  
Where the center of the sprite is. The sprite will be rotated/scaled around its pivot point.

`vec3 tint`  
A color the sprite will be blended with.

`float alpha`  
The opacity of the sprite.

`uint layerMask`  
Which layers the mesh should be rendered to.

\section Hub
Singleton class that holds all subsystems.

## Properties
`DebugDrawingManager@ debugDrawingManager`  
The debug drawing manager.

`PhysicsManager@ physicsManager`  
The physics manager.

\section DebugDrawingManager
Debug drawing facilities.

## Methods
`void AddPoint(const vec3& position, const vec3& color, float size, float duration = 0.0f, bool depthTesting = true)`  
Add a point to the world.

`void AddLine(const vec3& startPosition, const vec3& endPosition, const vec3& color, float width = 1.0f, float duration = 0.0f, bool depthTesting = true)`  
Add a line to the world.

`void AddCuboid(const vec3& dimensions, const mat4& matrix, const vec3& color, float lineWidth = 1.0f, float duration = 0.0f, bool depthTesting = true)`  
Add a cuboid to the world.

`void AddPlane(const vec3& centerPosition, const vec3& normal, const vec2& size, const vec3& color, float lineWidth = 1.0f, float duration = 0.0f, bool depthTesting = true)`  
Add a plane to the world.

`void AddCircle(const vec3& position, const vec3& normal, float radius, const vec3& color, float lineWidth = 1.0, float duration = 0.0, bool depthTesting = true)`  
Add a circle to the world.

`void AddSphere(const vec3& position, float radius, const vec3& color, float lineWidth = 1.0f, float duration = 0.0f, bool depthTesting = true)`  
Add a sphere to the world.

`void AddCylinder(float radius, float length, const mat4& matrix, const vec3& color, float lineWidth = 1.0f, float duration = 0.0f, bool depthTesting = true)`  
Add a cylinder to the world.

`void AddCone(float radius, float height, const mat4& matrix, const vec3& color, float lineWidth = 1.0f, float duration = 0.0f, bool depthTesting = true)`  
Add a cone to the world.

\section PhysicsManager
Debug drawing facilities.

## Methods
`void MakeKinematic(Component::RigidBody@ rigidBody)`  
Turn a rigid body into a kinematic object, putting movement in the control of the programmer.

`void MakeDynamic(Component::RigidBody@ rigidBody)`  
Turn a rigid body into a dynamic object.

`void ForceTransformSync(Component::RigidBody@ rigidBody)`  
Forces a dynamic rigid body to synchronize its transform with that of its owning entity during the next simulation iteration.

`void HaltMovement(Component::RigidBody@ rigidBody)`  
Halts movement of a kinematic rigid body.
