#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>
#include "../Util/Log.hpp"
#include "../Util/FileSystem.hpp"
#include "../Hymn.hpp"
#include "../Scene/Scene.hpp"
#include "../Component/Script.hpp"
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
    engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Transform", "vec3 position", asOFFSET(Component::Transform, position));
    engine->RegisterObjectProperty("Transform", "vec3 scale", asOFFSET(Component::Transform, scale));
    engine->RegisterObjectProperty("Transform", "vec3 rotation", asOFFSET(Component::Transform, rotation));
    engine->SetDefaultNamespace("");
    
    // Register GetComponent.
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
    for (Entity* entity : updateEntities)
        CallScript(entity, "void Update()");
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
