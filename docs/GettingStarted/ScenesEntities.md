\page ScenesEntities Scenes and Entities

Scenes are the meat of a hymn, and consist of a collection of entities arranged in a tree structure. Child entities are relative to their parent entities.

The base of a scene is always a root entity that can't be deleted. To add child entities to the root, right click on it and click on the `Add child` button. This entity can in turn have its own children that are relative to it (and so on). An entity can have any number of children.

## Scene instantiation

The real power of scenes comes with scene instantiation. To instantiate a scene, click on `Instantiate scene` and select a scene from the list. An instance of that scene will now exist as a child entity to the entity you instantiated it under. Whenever the scene is changed, all of its instances will change accordingly. This allows you to reuse content you have made.

### Example

Let's say you have a Crab scene that describes a crab enemy. You then instantiate that scene on multiple places in a BeachLevel scene. Now, you want the crab to make a pinching sound whenever the player is close, so you add a SoundSource component to the Crab scene. Each instance of the crab will now change accordingly and make a sound. There is no need to edit each crab individually.

## Entities

To edit an entity, right click on it and select `Edit`. This will open the entity's properties. You can edit the name, position, rotation and scale of the entity. You can enter the values for these directly, but you can also use the gizmo in the main viewport to move the entity around, rotate it and scale it.

### Components
Entities by themselves only contain a name and positional information. All functionality that entities can have come from adding components to them. A camera, for example, is nothing more than an entity containing a Lens component. A renderable 3D object is an entity containing Mesh and Material components. Add a %Physics component to it and it can move around in the game world.

To add a component to an entity, press on `Add %Component` and select a component from the list of available ones. An entity can contain any combination of components but only one of each kind; it can not, for example, contain two Lens components.

To remove a component, click on the component to expand its view and click on `Remove`.
