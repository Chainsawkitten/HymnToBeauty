\page YourFirstHymn_SceneInstantiation Scene Instantiation

The real power of scenes comes with scene instantiation. This means adding a copy of a scene to another scene. Whenever the scene is changed, all of its instances will change accordingly. This allows you to reuse content you have made.

### Example

Let's say you have a Crab scene that describes a crab enemy. You then instantiate that scene on multiple places in a BeachLevel scene. Now, you want the crab to make a pinching sound whenever the player is close, so you add a SoundSource component to the Crab scene. Each instance of the crab will now change accordingly and make a sound. There is no need to edit each crab individually.

\section CreateSceneForInstantiation Creating the scene to instantiate

Before you can instantiate a scene, you need a scene to instantiate. Let's create one. In the resource list, right click on the `Resources` folder and select `Add scene`.

\image html YourFirstHymn/AddScene.png

A new scene will be created. To switch to the new scene, click on it in the resource list. You can rename it to something more descriptive.

Add two entities to the new scene and add PointLight components to them. Move one to the left and one to the right.

\image html YourFirstHymn/InstantiateScene1.png

Save the scene by clicking `File` and selecting `Save Hymn`.

\section InstantiateScene Instantiating the scene

Switch to the default scene by clicking on it in the resource list. Right click on the root entity and select `Instantiate scene`.

\image html YourFirstHymn/InstantiateScene2.png

A list will pop up with all available scenes.

\image html YourFirstHymn/SceneList.png

Select the scene you just created. It will now be instantiated within the default scene. Notice how the light sources you added are now present in the scene. If you move the scene entity around, the light sources will move around with it.

\image html YourFirstHymn/InstantiateScene3.png

\section MakingChanges Making changes to the instantiated scene

Let's try making a change to the instantiated scene. Save the default scene and switch back to the instantiated scene. Now remove the PointLight components from the entities and add SoundSource components instead. Save and switch back to the default scene. Note how the lights have been replaced with sound sources. All changes made to a scene will apply to all instances of it.

\image html YourFirstHymn/InstantiateScene4.png