set(SRCS
        main.cpp
        DefaultScript.cpp
        Editor.cpp
        Resources.cpp
        ImGui/Json.cpp
        ImGui/OpenGLImplementation.cpp
        ImGui/Splitter.cpp
        ImGui/Theme.cpp
        ImGui/GuiHelpers.cpp
        GUI/FiltersWindow.cpp
        GUI/SelectHymnWindow.cpp
        GUI/SavePromptWindow.cpp
        GUI/InputWindow.cpp
        GUI/FileSelector.cpp
        GUI/FolderNameWindow.cpp
        GUI/ProfilingWindow.cpp
        GUI/ResourceSelector.cpp
        GUI/ResourceView.cpp
        GUI/SettingsWindow.cpp
        GUI/Editors/AnimationClipEditor.cpp
        GUI/Editors/AnimationControllerEditor.cpp
        GUI/LogView.cpp
        GUI/Editors/BoxShapeEditor.cpp
        GUI/Editors/CapsuleShapeEditor.cpp
        GUI/Editors/ConeShapeEditor.cpp
        GUI/Editors/CylinderShapeEditor.cpp
        GUI/Editors/EntityEditor.cpp
        GUI/Editors/ModelEditor.cpp
        GUI/Editors/NodeEditor.cpp
        GUI/Editors/PlaneShapeEditor.cpp
        GUI/Editors/RigidBodyEditor.cpp
        GUI/Editors/SceneEditor.cpp
        GUI/Editors/ScriptEditor.cpp
        GUI/Editors/SkeletonEditor.cpp
        GUI/Editors/SoundEditor.cpp
        GUI/Editors/SphereShapeEditor.cpp
        GUI/Editors/TextureEditor.cpp
        GUI/Editors/CurveEditor.cpp
        GUI/Editors/TriggerEditor.cpp
        Util/AssetConverter.cpp
        Util/AssetConverterSkeleton.cpp
        Util/AssetMetaData.cpp
        Util/EditorSettings.cpp
    )

set(HEADERS
        Editor.hpp
        Resources.hpp
        ImGui/Json.hpp
        ImGui/OpenGLImplementation.hpp
        ImGui/Splitter.hpp
        ImGui/Theme.hpp
        ImGui/GuiHelpers.hpp
        GUI/FiltersWindow.hpp
        GUI/SelectHymnWindow.hpp
        GUI/SavePromptWindow.hpp
        GUI/InputWindow.hpp
        GUI/FileSelector.hpp
        GUI/FolderNameWindow.hpp
        GUI/ProfilingWindow.hpp
        GUI/ResourceSelector.hpp
        GUI/ResourceView.hpp
        GUI/SettingsWindow.hpp
        GUI/Editors/AnimationClipEditor.hpp
        GUI/Editors/AnimationControllerEditor.hpp
        GUI/LogView.hpp
        GUI/Editors/BoxShapeEditor.hpp
        GUI/Editors/CapsuleShapeEditor.hpp
        GUI/Editors/ConeShapeEditor.hpp
        GUI/Editors/CylinderShapeEditor.hpp
        GUI/Editors/EntityEditor.hpp
        GUI/Editors/IShapeEditor.hpp
        GUI/Editors/ModelEditor.hpp
        GUI/Editors/NodeEditor.hpp
        GUI/Editors/PlaneShapeEditor.hpp
        GUI/Editors/RigidBodyEditor.hpp
        GUI/Editors/SceneEditor.hpp
        GUI/Editors/ScriptEditor.hpp
        GUI/Editors/SkeletonEditor.hpp
        GUI/Editors/SoundEditor.hpp
        GUI/Editors/SphereShapeEditor.hpp
        GUI/Editors/TextureEditor.hpp
        GUI/Editors/CurveEditor.hpp
        GUI/Editors/TriggerEditor.hpp
        Util/AssetConverter.hpp
        Util/AssetConverterSkeleton.hpp
        Util/AssetMetaData.hpp
        Util/EditorSettings.hpp
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

add_executable(Editor ${SRCS} ${HEADERS})
target_link_libraries(Editor Engine imgui imguizmo)
set_property(TARGET Editor PROPERTY CXX_STANDARD 11)
set_property(TARGET Editor PROPERTY CXX_STANDARD_REQUIRED ON)
