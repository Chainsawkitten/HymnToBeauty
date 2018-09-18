## Engine ##
set(SRCS
        ../../externals/AngelScript/sdk/add_on/scriptbuilder/scriptbuilder.cpp
        ../../externals/AngelScript/sdk/add_on/scriptarray/scriptarray.cpp
        ../../externals/AngelScript/sdk/add_on/scriptdictionary/scriptdictionary.cpp
        ../../externals/AngelScript/sdk/add_on/scriptmath/scriptmath.cpp
        ../../externals/AngelScript/sdk/add_on/scriptstdstring/scriptstdstring.cpp
        Hymn.cpp
        MainWindow.cpp
        Animation/Animation.cpp
        Animation/AnimationAction.cpp
        Animation/AnimationClip.cpp
        Animation/AnimationController.cpp
        Animation/AnimationTransition.cpp
        Animation/Bone.cpp
        Animation/Skeleton.cpp
        Animation/SkeletonBone.cpp
        Audio/SoundBuffer.cpp
        Audio/SoundFile.cpp
        Audio/VorbisFile.cpp
        Component/AnimationController.cpp
        Component/DirectionalLight.cpp
        Component/Lens.cpp
        Component/Listener.cpp
        Component/Material.cpp
        Component/Mesh.cpp
        Component/PointLight.cpp
        Component/RigidBody.cpp
        Component/Script.cpp
        Component/Shape.cpp
        Component/SoundSource.cpp
        Component/SpotLight.cpp
        Component/SuperComponent.cpp
        Component/ParticleSystem.cpp
        Component/Trigger.cpp
        Entity/Entity.cpp
        Entity/World.cpp
        Geometry/AssetFileHandler.cpp
        Geometry/Cube.cpp
        Geometry/MathFunctions.cpp
        Geometry/MeshData.cpp
        Geometry/Model.cpp
        Manager/Managers.cpp
        Manager/DebugDrawingManager.cpp
        Manager/ParticleManager.cpp
        Manager/ProfilingManager.cpp
        Manager/PhysicsManager.cpp
        Manager/RenderManager.cpp
        Manager/ResourceManager.cpp
        Manager/ScriptManager.cpp
        Manager/SoundManager.cpp
        Manager/TriggerManager.cpp
        Physics/GlmConversion.cpp
        Physics/Shape.cpp
        Physics/Trigger.cpp
        Physics/TriggerObserver.cpp
        Trigger/SuperTrigger.cpp
        Trigger/TriggerRepeat.cpp
        Trigger/TriggerOnce.cpp
        Util/FileSystem.cpp
        Util/GPUProfiling.cpp
        Util/Input.cpp
        Util/Json.cpp
        Util/MousePicking.cpp
        Util/Node.cpp
        Util/RayIntersection.cpp
        Util/Profiling.cpp
        Util/Settings.cpp
        Texture/TextureAsset.cpp
        Script/ScriptFile.cpp
        Input/Input.cpp
    )

set(HEADERS
        Hymn.hpp
        MainWindow.hpp
        Animation/Animation.hpp
        Animation/AnimationAction.hpp
        Animation/AnimationClip.hpp
        Animation/AnimationController.hpp
        Animation/AnimationTransition.hpp
        Animation/Bone.hpp
        Animation/Skeleton.hpp
        Animation/SkeletonBone.hpp
        Audio/SoundBuffer.hpp
        Audio/SoundFile.hpp
        Audio/VorbisFile.hpp
        Component/AnimationController.hpp
        Component/DirectionalLight.hpp
        Component/Lens.hpp
        Component/Listener.hpp
        Component/Material.hpp
        Component/Mesh.hpp
        Component/PointLight.hpp
        Component/RigidBody.hpp
        Component/Script.hpp
        Component/Shape.hpp
        Component/SoundSource.hpp
        Component/SpotLight.hpp
        Component/SuperComponent.hpp
        Component/ParticleSystem.hpp
        Component/Trigger.hpp
        Entity/ComponentContainer.hpp
        Entity/Entity.hpp
        Entity/World.hpp
        Geometry/AssetFileHandler.hpp
        Geometry/Cube.hpp
        Geometry/MathFunctions.hpp
        Geometry/MeshData.hpp
        Geometry/Model.hpp
        Manager/Managers.hpp
        Manager/DebugDrawingManager.hpp
        Manager/ParticleManager.hpp
        Manager/ProfilingManager.hpp
        Manager/PhysicsManager.hpp
        Manager/RenderManager.hpp
        Manager/ResourceManager.hpp
        Manager/ScriptManager.hpp
        Manager/SoundManager.hpp
        Manager/TriggerManager.hpp
        Physics/GlmConversion.hpp
        Physics/Shape.hpp
        Physics/Trigger.hpp
        Physics/TriggerObserver.hpp
        Trigger/SuperTrigger.hpp
        Trigger/TriggerRepeat.hpp
        Trigger/TriggerOnce.hpp
        Util/FileSystem.hpp
        Util/GPUProfiling.hpp
        Util/Input.hpp
        Util/Json.hpp
        Util/MousePicking.hpp
        Util/Node.hpp
        Util/RayIntersection.hpp
        Util/Profiling.hpp
        Util/Settings.hpp
        Texture/TextureAsset.hpp
        Script/ScriptFile.hpp
        Input/Input.hpp
    )

# Setup embedded source
set(EMBEDDED_INPUT
    ../../assets/Camera.png
    ../../assets/DefaultAlbedo.png
    ../../assets/DefaultNormal.png
    ../../assets/DefaultMetallic.png
    ../../assets/DefaultRoughness.png
    ../../assets/Light.png
    ../../assets/ParticleAtlas.png
    ../../assets/ParticleEmitter.png
    ../../assets/SoundSource.png
)

foreach(item ${EMBEDDED_INPUT})
    ProcessWithHeaderize(EMBEDDED_HEADER EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

set_property(SOURCE ${SRCS} APPEND PROPERTY OBJECT_DEPENDS ${EMBEDDED_HEADER})

create_directory_groups(${SRCS} ${HEADERS})

add_library(Engine STATIC ${SRCS} ${HEADERS} ${EMBEDDED_SOURCE})

target_link_libraries(Engine Video angelscript bullet glfw ${GLFW_LIBRARIES} glm OpenALSoft simpleini stb)

set_property(TARGET Engine PROPERTY CXX_STANDARD 11)
set_property(TARGET Engine PROPERTY CXX_STANDARD_REQUIRED ON)
