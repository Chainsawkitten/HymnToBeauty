\page YourFirstHymn_Components Components

Entities by themselves only contain a name and positional information. All functionality that entities can have come from adding components to them. A camera, for example, is nothing more than an entity containing a Lens component. A renderable 3D object is an entity containing Mesh and Material components. Add a %Physics component to it and it can move around in the game world.

\section AddComponent Adding a component

To add a component to an entity, press on `Add %Component` and select a component from the list of available ones.

\image html YourFirstHymn/AddComponent.png

An entity can contain any combination of components but only one of each kind; it can not, for example, contain two Lens components.

\section RemoveComponent Removing a component

To remove a component, click on the component to expand its view and click on `Remove`.
