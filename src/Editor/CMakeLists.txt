set(SRCS
        main.cpp
        DefaultScript.cpp
        Editor.cpp
        Resources.cpp
        ImGui/Json.cpp
        ImGui/Implementation.cpp
        ImGui/Splitter.cpp
        ImGui/Theme.cpp
        ImGui/GuiHelpers.cpp
        GUI/SelectHymnWindow.cpp
        GUI/SavePromptWindow.cpp
        GUI/InputWindow.cpp
        GUI/FileSelector.cpp
        GUI/FolderNameWindow.cpp
        GUI/ProfilingWindow.cpp
        GUI/ResourceSelector.cpp
        GUI/ResourceView.cpp
        GUI/SettingsWindow.cpp
        GUI/LogView.cpp
        GUI/Editors/AssetEditor.cpp
        GUI/Editors/BoxShapeEditor.cpp
        GUI/Editors/CapsuleShapeEditor.cpp
        GUI/Editors/ConeShapeEditor.cpp
        GUI/Editors/CylinderShapeEditor.cpp
        GUI/Editors/EntityEditor.cpp
        GUI/Editors/GLTFEditor.cpp
        GUI/Editors/PlaneShapeEditor.cpp
        GUI/Editors/RigidBodyEditor.cpp
        GUI/Editors/SceneEditor.cpp
        GUI/Editors/ScriptEditor.cpp
        GUI/Editors/SoundEditor.cpp
        GUI/Editors/SphereShapeEditor.cpp
        GUI/Editors/TextureEditor.cpp
        GUI/Editors/TriggerEditor.cpp
        Util/EditorSettings.cpp
        Util/GLTFImporter.cpp
    )

set(HEADERS
        Editor.hpp
        Resources.hpp
        ImGui/Json.hpp
        ImGui/Implementation.hpp
        ImGui/Splitter.hpp
        ImGui/Theme.hpp
        ImGui/GuiHelpers.hpp
        GUI/SelectHymnWindow.hpp
        GUI/SavePromptWindow.hpp
        GUI/InputWindow.hpp
        GUI/FileSelector.hpp
        GUI/FolderNameWindow.hpp
        GUI/ProfilingWindow.hpp
        GUI/ResourceSelector.hpp
        GUI/ResourceView.hpp
        GUI/SettingsWindow.hpp
        GUI/LogView.hpp
        GUI/Editors/AssetEditor.hpp
        GUI/Editors/BoxShapeEditor.hpp
        GUI/Editors/CapsuleShapeEditor.hpp
        GUI/Editors/ConeShapeEditor.hpp
        GUI/Editors/CylinderShapeEditor.hpp
        GUI/Editors/EntityEditor.hpp
        GUI/Editors/IShapeEditor.hpp
        GUI/Editors/GLTFEditor.hpp
        GUI/Editors/PlaneShapeEditor.hpp
        GUI/Editors/RigidBodyEditor.hpp
        GUI/Editors/SceneEditor.hpp
        GUI/Editors/ScriptEditor.hpp
        GUI/Editors/SoundEditor.hpp
        GUI/Editors/SphereShapeEditor.hpp
        GUI/Editors/TextureEditor.hpp
        GUI/Editors/TriggerEditor.hpp
        Util/EditorSettings.hpp
        Util/GLTFImporter.hpp
    )

# Setup embedded source
set(EMBEDDED_INPUT
    ../../assets/Camera.png
    ../../assets/DefaultAlbedo.png
    ../../assets/DefaultNormal.png
    ../../assets/DefaultRoughnessMetallic.png
    ../../assets/Light.png
    ../../assets/SoundSource.png
)

# Setup shaders
set(SHADERS
    shaders/ImGUI.vert
    shaders/ImGUI.frag
)

foreach(item ${EMBEDDED_INPUT})
    ProcessWithHeaderize(EMBEDDED_HEADER EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# Process shaders.
foreach(item ${SHADERS})
    ProcessShader(SHADER_HEADER SHADER_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR} "")
endforeach()

create_directory_groups(${SRCS} ${HEADERS})

add_executable(Editor ${SRCS} ${HEADERS} ${EMBEDDED_SOURCE} ${EMBEDDED_HEADER} ${SHADER_SOURCE} ${SHADER_HEADER})
target_link_libraries(Editor Engine imgui imguizmo)
set_property(TARGET Editor PROPERTY CXX_STANDARD 11)
set_property(TARGET Editor PROPERTY CXX_STANDARD_REQUIRED ON)
