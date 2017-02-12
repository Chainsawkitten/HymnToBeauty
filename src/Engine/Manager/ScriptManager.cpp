#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>
#include "../Util/Log.hpp"
#include "../Util/FileSystem.hpp"
#include "../Hymn.hpp"
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Script.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Listener.hpp"
#include "../Component/Physics.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SpotLight.hpp"
#include "../Entity/Entity.hpp"
#include "../Input/Input.hpp"
#include "../Script/ScriptFile.hpp"

#include "Managers.hpp"

using namespace Component;

void print(const std::string& message) {
    Log() << message;
}

Entity* GetEntity() {
    return Managers().scriptManager->currentEntity;
}

void RegisterUpdate() {
    Managers().scriptManager->RegisterUpdate(GetEntity());
}

bool Input(int button_index) {
    return Input::GetInstance().Check_Button(button_index);
}

ScriptManager::ScriptManager() {
    // Create the script engine
    engine = asCreateScriptEngine();
    
    // Set the message callback to receive information on errors in human readable form.
    engine->SetMessageCallback(asFUNCTION(AngelScriptMessageCallback), 0, asCALL_CDECL);
    
    // Register add-ons.
    RegisterStdString(engine);

    RegisterInput();

    // Register GLM types.
    engine->RegisterObjectType("vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD);
    engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x));
    engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y));
    engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z));
    
    // Register Entity.
    engine->RegisterObjectType("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Entity", "string name", asOFFSET(Entity, name));
    engine->RegisterObjectProperty("Entity", "vec3 position", asOFFSET(Entity, position));
    engine->RegisterObjectProperty("Entity", "vec3 scale", asOFFSET(Entity, scale));
    engine->RegisterObjectProperty("Entity", "vec3 rotation", asOFFSET(Entity, rotation));
    engine->RegisterObjectMethod("Entity", "void Kill()", asMETHOD(Entity, Kill), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsKilled() const", asMETHOD(Entity, IsKilled), asCALL_THISCALL);
    
    // Register components.
    engine->SetDefaultNamespace("Component");
    
    engine->RegisterObjectType("DirectionalLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("DirectionalLight", "vec3 color", asOFFSET(DirectionalLight, color));
    engine->RegisterObjectProperty("DirectionalLight", "float ambientCoefficient", asOFFSET(DirectionalLight, ambientCoefficient));
    
    engine->RegisterObjectType("Lens", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Lens", "float fieldOfView", asOFFSET(Lens, fieldOfView));
    engine->RegisterObjectProperty("Lens", "float zNear", asOFFSET(Lens, zNear));
    engine->RegisterObjectProperty("Lens", "float zFar", asOFFSET(Lens, zFar));
    
    engine->RegisterObjectType("Listener", 0, asOBJ_REF | asOBJ_NOCOUNT);
    
    engine->RegisterObjectType("Physics", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Physics", "vec3 velocity", asOFFSET(Physics, velocity));
    engine->RegisterObjectProperty("Physics", "float maxVelocity", asOFFSET(Physics, maxVelocity));
    engine->RegisterObjectProperty("Physics", "vec3 angularVelocity", asOFFSET(Physics, angularVelocity));
    engine->RegisterObjectProperty("Physics", "float maxAngularVelocity", asOFFSET(Physics, maxAngularVelocity));
    engine->RegisterObjectProperty("Physics", "vec3 acceleration", asOFFSET(Physics, acceleration));
    engine->RegisterObjectProperty("Physics", "vec3 angularAcceleration", asOFFSET(Physics, angularAcceleration));
    engine->RegisterObjectProperty("Physics", "float velocityDragFactor", asOFFSET(Physics, velocityDragFactor));
    engine->RegisterObjectProperty("Physics", "float angularDragFactor", asOFFSET(Physics, angularDragFactor));
    engine->RegisterObjectProperty("Physics", "float gravityFactor", asOFFSET(Physics, gravityFactor));
    engine->RegisterObjectProperty("Physics", "vec3 momentOfInertia", asOFFSET(Physics, momentOfInertia));
    
    engine->RegisterObjectType("PointLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("PointLight", "vec3 color", asOFFSET(PointLight, color));
    engine->RegisterObjectProperty("PointLight", "float ambientCoefficient", asOFFSET(PointLight, ambientCoefficient));
    engine->RegisterObjectProperty("PointLight", "float attenuation", asOFFSET(PointLight, attenuation));
    engine->RegisterObjectProperty("PointLight", "float intensity", asOFFSET(PointLight, intensity));
    
    engine->RegisterObjectType("SpotLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("SpotLight", "vec3 color", asOFFSET(SpotLight, color));
    engine->RegisterObjectProperty("SpotLight", "float ambientCoefficient", asOFFSET(SpotLight, ambientCoefficient));
    engine->RegisterObjectProperty("SpotLight", "float attenuation", asOFFSET(SpotLight, attenuation));
    engine->RegisterObjectProperty("SpotLight", "float intensity", asOFFSET(SpotLight, intensity));
    engine->RegisterObjectProperty("SpotLight", "float coneAngle", asOFFSET(SpotLight, coneAngle));
    
    engine->SetDefaultNamespace("");
    
    // Register adding and getting components..
    engine->RegisterObjectMethod("Entity", "Component::DirectionalLight@ AddDirectionalLight()", asMETHODPR(Entity, AddComponent<DirectionalLight>, (), DirectionalLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::DirectionalLight@ GetDirectionalLight()", asMETHODPR(Entity, GetComponent<DirectionalLight>, (), DirectionalLight*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::Lens@ AddLens()", asMETHODPR(Entity, AddComponent<Lens>, (), Lens*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Lens@ GetLens()", asMETHODPR(Entity, GetComponent<Lens>, (), Lens*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::Listener@ AddListener()", asMETHODPR(Entity, AddComponent<Listener>, (), Listener*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Listener@ GetListener()", asMETHODPR(Entity, GetComponent<Listener>, (), Listener*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::Physics@ AddPhysics()", asMETHODPR(Entity, AddComponent<Physics>, (), Physics*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Physics@ GetPhysics()", asMETHODPR(Entity, GetComponent<Physics>, (), Physics*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::PointLight@ AddPointLight()", asMETHODPR(Entity, AddComponent<PointLight>, (), PointLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::PointLight@ GetPointLight()", asMETHODPR(Entity, GetComponent<PointLight>, (), PointLight*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::SpotLight@ AddSpotLight()", asMETHODPR(Entity, AddComponent<SpotLight>, (), SpotLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::SpotLight@ GetSpotLight()", asMETHODPR(Entity, GetComponent<SpotLight>, (), SpotLight*), asCALL_THISCALL);
    
    // Register functions.
    engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    engine->RegisterGlobalFunction("Entity@ GetEntity()", asFUNCTION(GetEntity), asCALL_CDECL);
    engine->RegisterGlobalFunction("void RegisterUpdate()", asFUNCTION(::RegisterUpdate), asCALL_CDECL);
    //engine->RegisterGlobalFunction("bool Input(input button)", asFUNCTION(Input), asCALL_CDECL);

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

void ScriptManager::BuildSpecificScript(const std::string& path) {
    std::string filename = path;
    if (!FileSystem::FileExists(filename.c_str())) {
        Log() << "Script file does not exist: " << filename << "\n";
        return;
    }

    // Create and build script module.
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, path.c_str());
    if (r < 0)
        Log() << "Couldn't start new module: " << path << ".\n";

    r = builder.AddSectionFromFile(filename.c_str());
    if (r < 0)
        Log() << "File section could not be added: " << filename << ".\n";

    r = builder.BuildModule();
    if (r < 0)
        Log() << "Compile errors.\n";
}

void ScriptManager::BuildAllScripts() {

    std::string path = Hymn().GetPath() + FileSystem::DELIMITER + "Scripts" + FileSystem::DELIMITER;
    std::vector<std::string> files = FileSystem::DirectoryContents(path, FileSystem::FILE);
    
    for (ScriptFile* file : Hymn().scripts) {

        if (!FileSystem::FileExists(file->path.c_str())) {
            Log() << "Script file does not exist: " << file->path << "\n";
            return;
        }
        // Create and build script module.
        CScriptBuilder builder;
        asIScriptModule* module = engine->GetModule(file->module.c_str());
        if (module == nullptr) {

            int r = builder.StartNewModule(engine, file->module.c_str());
            if (r < 0)
                Log() << "Couldn't start new module: " << path << ".\n";
            r = builder.AddSectionFromFile(file->path.c_str());
            if (r < 0)
                Log() << "File section could not be added: " << file->path << ".\n";

            r = builder.BuildModule();
            if (r < 0)
                Log() << "Compile errors.\n";

        }
        else {

            std::string script;
            LoadScriptFile(file->path.c_str(), script);
            module->AddScriptSection(file->path.c_str(), script.c_str());
            
            int r = module->Build();
            if (r < 0)
                Log() << file->module.c_str() << "Compile errors.\n";

        }

    }

}
// Load the entire script file into a string buffer
void ScriptManager::LoadScriptFile(const char *fileName, std::string &script){

    // Open the file in binary mode
    FILE* f = fopen(fileName, "rb");

    // Determine the size of the file
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Load the entire file in one call
    script.resize(len);
    fread(&script[0], len, 1, f);

    fclose(f);
}

void ScriptManager::Update(World& world) {
    // Init.
    for (Script* script : world.GetComponents<Script>()) {
        if (!script->initialized) {
            CallSpecificScript(script->entity, script->scriptFile, "void Init()");
            script->initialized = true;
        }
    }
    
    // Update.
    for (Entity* entity : world.GetUpdateEntities())
        CallSpecificScript(entity, entity->GetComponent<Script>()->scriptFile, "void Update()");
    
    // Register entities for events.
    for (Entity* entity : updateEntities)
        world.RegisterUpdate(entity);
    updateEntities.clear();
}

void ScriptManager::RegisterUpdate(Entity* entity) {
    updateEntities.push_back(entity);
}

void ScriptManager::RegisterInput() {

    engine->RegisterEnum("input");
    for (int i = 0; i < Input::GetInstance().buttons.size(); i++) {

        engine->RegisterEnumValue("input", std::string(Input::GetInstance().buttons[i]->action).c_str(), i);
            
    }

    //engine->RegisterGlobalFunction("bool Input(int Button)", asFUNCTION(Input), asCALL_CDECL);

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
void ScriptManager::CallSpecificScript(Entity* entity, ScriptFile* script, const std::string& functionName) {

    currentEntity = entity;

    // Get script module.
    asIScriptModule* module = engine->GetModule(script->module.c_str(), asGM_ONLY_IF_EXISTS);

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
