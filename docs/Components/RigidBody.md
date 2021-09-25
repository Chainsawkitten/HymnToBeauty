\page ComponentRigidBody Rigid body

Allows interacting with other physics components. Also needs a %Shape component to function.

| Property | Description |
|----------|-------------|
| Mass | The mass of the object. |
| Friction | Friction value. |
| Rolling friction | Rolling friction value. |
| Spinning friction | Spinning friction value. |
| CoR | Coefficient of restitution.  |
| Lin. damping | Linear damping. |
| Ang. damping | Angular damping. |
| Kinematic | Whether the rigid body is kinematic or dynamic. In the former case the transform is determined by that of its entity. In the latter case, Bullet calculates it.  |
| Ghost | Whether the entity should disregard all collisions and only act as a collider for trigger purposes. |
