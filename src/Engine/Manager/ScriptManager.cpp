#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>
#include "../Util/Log.hpp"
#include "../Util/FileSystem.hpp"
#include "../Hymn.hpp"
#include "../Scene/Scene.hpp"
#include "../Component/Script.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Listener.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/Transform.hpp"
#include "../Entity/Entity.hpp"
#include "Managers.hpp"

void print(const std::string& message) {
    Log() << message;
}

Entity* GetEntity() {
    return Managers().scriptManager->currentEntity;
}

void RegisterUpdate() {
    Managers().scriptManager->RegisterUpdate(GetEntity());
}

ScriptManager::ScriptManager() {
    // Create the script engine
    engine = asCreateScriptEngine();
    
    // Set the message callback to receive information on errors in human readable form.
    engine->SetMessageCallback(asFUNCTION(AngelScriptMessageCallback), 0, asCALL_CDECL);
    
    // Register add-ons.
    RegisterStdString(engine);
    
    // Register GLM types.
    engine->RegisterObjectType("vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD);
    engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x));
    engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y));
    engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z));
    
    // Register Entity.
    engine->RegisterObjectType("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Entity", "string name", asOFFSET(Entity, name));
    engine->RegisterObjectMethod("Entity", "void Kill()", asMETHOD(Entity, Kill), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsKilled() const", asMETHOD(Entity, IsKilled), asCALL_THISCALL);
    
    // Register components.
    engine->SetDefaultNamespace("Component");
    
    engine->RegisterObjectType("DirectionalLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("DirectionalLight", "vec3 color", asOFFSET(Component::DirectionalLight, color));
    engine->RegisterObjectProperty("DirectionalLight", "float ambientCoefficient", asOFFSET(Component::DirectionalLight, ambientCoefficient));
    
    engine->RegisterObjectType("Lens", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Lens", "float fieldOfView", asOFFSET(Component::Lens, fieldOfView));
    engine->RegisterObjectProperty("Lens", "float zNear", asOFFSET(Component::Lens, zNear));
    engine->RegisterObjectProperty("Lens", "float zFar", asOFFSET(Component::Lens, zFar));
    
    engine->RegisterObjectType("Listener", 0, asOBJ_REF | asOBJ_NOCOUNT);
    
    engine->RegisterObjectType("PointLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("PointLight", "vec3 color", asOFFSET(Component::PointLight, color));
    engine->RegisterObjectProperty("PointLight", "float ambientCoefficient", asOFFSET(Component::PointLight, ambientCoefficient));
    engine->RegisterObjectProperty("PointLight", "float attenuation", asOFFSET(Component::PointLight, attenuation));
    engine->RegisterObjectProperty("PointLight", "float intensity", asOFFSET(Component::PointLight, intensity));
    
    engine->RegisterObjectType("SpotLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("SpotLight", "vec3 color", asOFFSET(Component::SpotLight, color));
    engine->RegisterObjectProperty("SpotLight", "float ambientCoefficient", asOFFSET(Component::SpotLight, ambientCoefficient));
    engine->RegisterObjectProperty("SpotLight", "float attenuation", asOFFSET(Component::SpotLight, attenuation));
    engine->RegisterObjectProperty("SpotLight", "float intensity", asOFFSET(Component::SpotLight, intensity));
    engine->RegisterObjectProperty("SpotLight", "float coneAngle", asOFFSET(Component::SpotLight, coneAngle));
    
    engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Transform", "vec3 position", asOFFSET(Component::Transform, position));
    engine->RegisterObjectProperty("Transform", "vec3 scale", asOFFSET(Component::Transform, scale));
    engine->RegisterObjectProperty("Transform", "vec3 rotation", asOFFSET(Component::Transform, rotation));
    
    engine->SetDefaultNamespace("");
    
    // Register adding and getting components..
    engine->RegisterObjectMethod("Entity", "Component::DirectionalLight@ AddDirectionalLight()", asMETHODPR(Entity, AddComponent<Component::DirectionalLight>, (), Component::DirectionalLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::DirectionalLight@ GetDirectionalLight()", asMETHODPR(Entity, GetComponent<Component::DirectionalLight>, (), Component::DirectionalLight*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::Lens@ AddLens()", asMETHODPR(Entity, AddComponent<Component::Lens>, (), Component::Lens*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Lens@ GetLens()", asMETHODPR(Entity, GetComponent<Component::Lens>, (), Component::Lens*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::Listener@ AddListener()", asMETHODPR(Entity, AddComponent<Component::Listener>, (), Component::Listener*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Listener@ GetListener()", asMETHODPR(Entity, GetComponent<Component::Listener>, (), Component::Listener*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::PointLight@ AddPointLight()", asMETHODPR(Entity, AddComponent<Component::PointLight>, (), Component::PointLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::PointLight@ GetPointLight()", asMETHODPR(Entity, GetComponent<Component::PointLight>, (), Component::PointLight*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::SpotLight@ AddSpotLight()", asMETHODPR(Entity, AddComponent<Component::SpotLight>, (), Component::SpotLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::SpotLight@ GetSpotLight()", asMETHODPR(Entity, GetComponent<Component::SpotLight>, (), Component::SpotLight*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::Transform@ AddTransform()", asMETHODPR(Entity, AddComponent<Component::Transform>, (), Component::Transform*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Transform@ GetTransform()", asMETHODPR(Entity, GetComponent<Component::Transform>, (), Component::Transform*), asCALL_THISCALL);
    
    // Register functions.
    engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    engine->RegisterGlobalFunction("Entity@ GetEntity()", asFUNCTION(GetEntity), asCALL_CDECL);
    engine->RegisterGlobalFunction("void RegisterUpdate()", asFUNCTION(::RegisterUpdate), asCALL_CDECL);
}

ScriptManager::~ScriptManager() {
    engine->ShutDownAndRelease();
}

void ScriptManager::BuildScript(const std::string& name) {
    std::string filename = Hymn().GetPath() + FileSystem::DELIMITER + "Scripts" + FileSystem::DELIMITER + name + ".as";
    if (!FileSystem::FileExists(filename.c_str())) {
        Log() << "Script file does not exist: " << filename << "\n";
        return;
    }
    
    // Create and build script module.
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, name.c_str());
    if (r < 0)
        Log() << "Couldn't start new module: " << name << ".\n";
    
    r = builder.AddSectionFromFile(filename.c_str());
    if (r < 0)
        Log() << "File section could not be added: " << filename << ".\n";
    
    r = builder.BuildModule();
    if (r < 0)
        Log() << "Compile errors.\n";
}

void ScriptManager::Update(Scene& scene) {
    // Init.
    for (Component::Script* script : scene.GetComponents<Component::Script>()) {
        if (!script->initialized) {
            CallScript(script->entity, "void Init()");
            script->initialized = true;
        }
    }
    
    // Update.
    for (Entity* entity : scene.GetUpdateEntities())
        CallScript(entity, "void Update()");
    
    // Register entities for events.
    for (Entity* entity : updateEntities)
        scene.RegisterUpdate(entity);
    updateEntities.clear();
}

void ScriptManager::RegisterUpdate(Entity* entity) {
    updateEntities.push_back(entity);
}

void ScriptManager::CallScript(Entity* entity, const std::string& functionName) {
    currentEntity = entity;
    
    // Get script module.
    asIScriptModule* module = engine->GetModule(entity->name.c_str(), asGM_ONLY_IF_EXISTS);
    
    // Find function to call.
    asIScriptFunction* function = module->GetFunctionByDecl(functionName.c_str());
    if (function == nullptr)
        Log() << "Couldn't find \"" + functionName + "\" function.\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(function);
    int r = context->Execute();
    if (r != asEXECUTION_FINISHED) {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION) {
          // An exception occurred, let the script writer know what happened so it can be corrected.
          Log() << "An exception '" << context->GetExceptionString() << "' occurred. Please correct the code and try again.\n";
        }
    }
    
    // Clean up.
    context->Release();
}
