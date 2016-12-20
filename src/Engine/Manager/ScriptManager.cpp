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

void print(const std::string& message) {
    Log() << message;
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
    for (Component::Script* script : scene.GetComponents<Component::Script>()) {
        if (!script->initialized) {
            // Create, load and build script module.
            asIScriptModule* module = engine->GetModule(script->entity->name.c_str(), asGM_ONLY_IF_EXISTS);
            
            // Find function to call.
            asIScriptFunction* function = module->GetFunctionByDecl("void Init()");
            if (function == nullptr)
                Log() << "Couldn't find \"void Init()\" function.\n";
            
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
            
            script->initialized = true;
        }
    }
}
