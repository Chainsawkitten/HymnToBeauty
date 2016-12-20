#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>
#include "../Util/Log.hpp"
#include "../Util/FileSystem.hpp"
#include "../Hymn.hpp"
#include "../Scene/Scene.hpp"
#include "../Component/Script.hpp"
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
    
    // Register Entity.
    engine->RegisterObjectType("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Entity", "string name", asOFFSET(Entity, name));
    
    // Register functions.
    engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    engine->RegisterGlobalFunction("Entity@ GetEntity()", asFUNCTION(GetEntity), asCALL_CDECL);
    int r = engine->RegisterGlobalFunction("void RegisterUpdate()", asFUNCTION(::RegisterUpdate), asCALL_CDECL);
    if (r < 0)
        Log() << "asdfsadsad\n";
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
