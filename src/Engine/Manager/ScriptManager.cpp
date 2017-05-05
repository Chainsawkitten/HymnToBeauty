#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptmath/scriptmath.h>
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
#include "../Component/SoundSource.hpp"
#include "../Component/SpotLight.hpp"
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

bool Input(int buttonIndex) {
    return Input::GetInstance().CheckButton(buttonIndex);
}

void SendMessage(Entity* recipient, int type) {
    Managers().scriptManager->SendMessage(recipient, type);
}

void vec3Constructor(float x, float y, float z, void* memory) {
    glm::vec3* vec = static_cast<glm::vec3*>(memory);
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

glm::vec3 vec3Add(const glm::vec3& a, void* memory) {
    return a + *static_cast<glm::vec3*>(memory);
}

ScriptManager::ScriptManager() {
    // Create the script engine
    engine = asCreateScriptEngine();
    
    // Set the message callback to receive information on errors in human readable form.
    engine->SetMessageCallback(asFUNCTION(AngelScriptMessageCallback), 0, asCALL_CDECL);
    
    // Register add-ons.
    RegisterStdString(engine);
    RegisterScriptMath(engine);
    
    engine->RegisterEnum("input");
    
    // Register GLM types.
    engine->RegisterObjectType("vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::vec3>());
    engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x));
    engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y));
    engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z));
    engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(vec3Constructor), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opAdd(const vec3 &in) const", asFUNCTION(vec3Add), asCALL_CDECL_OBJLAST);
    
    // Register Entity.
    engine->RegisterObjectType("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Entity", "string name", asOFFSET(Entity, name));
    engine->RegisterObjectProperty("Entity", "vec3 position", asOFFSET(Entity, position));
    engine->RegisterObjectProperty("Entity", "vec3 scale", asOFFSET(Entity, scale));
    engine->RegisterObjectProperty("Entity", "vec3 rotation", asOFFSET(Entity, rotation));
    engine->RegisterObjectMethod("Entity", "void Kill()", asMETHOD(Entity, Kill), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsKilled() const", asMETHOD(Entity, IsKilled), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ GetParent() const", asMETHOD(Entity, GetParent), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ AddChild(const string &in)", asMETHOD(Entity, AddChild), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ InstantiateScene(const string &in)", asMETHOD(Entity, InstantiateScene), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsScene() const", asMETHOD(Entity, IsScene), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ GetChild(const string &in) const", asMETHOD(Entity, GetChild), asCALL_THISCALL);
    
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
    
    engine->RegisterObjectType("SoundSource", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("SoundSource", "float pitch", asOFFSET(SoundSource, pitch));
    engine->RegisterObjectProperty("SoundSource", "float gain", asOFFSET(SoundSource, gain));
    engine->RegisterObjectProperty("SoundSource", "bool loop", asOFFSET(SoundSource, loop));
    engine->RegisterObjectMethod("SoundSource", "void Play()", asMETHOD(SoundSource, Play), asCALL_THISCALL);
    engine->RegisterObjectMethod("SoundSource", "void Pause()", asMETHOD(SoundSource, Pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("SoundSource", "void Stop()", asMETHOD(SoundSource, Stop), asCALL_THISCALL);
    
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
    
    engine->RegisterObjectMethod("Entity", "Component::SoundSource@ AddSoundSource()", asMETHODPR(Entity, AddComponent<SoundSource>, (), SoundSource*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::SoundSource@ GetSoundSource()", asMETHODPR(Entity, GetComponent<SoundSource>, (), SoundSource*), asCALL_THISCALL);
    
    // Register functions.
    engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    engine->RegisterGlobalFunction("Entity@ GetEntity()", asFUNCTION(GetEntity), asCALL_CDECL);
    engine->RegisterGlobalFunction("void RegisterUpdate()", asFUNCTION(::RegisterUpdate), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool Input(input button)", asFUNCTION(Input), asCALL_CDECL);
    engine->RegisterGlobalFunction("void SendMessage(Entity@, int)", asFUNCTION(::SendMessage), asCALL_CDECL);
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

void ScriptManager::BuildAllScripts() {
    std::string path = Hymn().GetPath() + FileSystem::DELIMITER + "Scripts" + FileSystem::DELIMITER;
    
    for (ScriptFile* file : Hymn().scripts) {
        std::string filename = path + file->name + ".as";
        if (!FileSystem::FileExists(filename.c_str())) {
            Log() << "Script file does not exist: " << filename << "\n";
            return;
        }
        
        // Create and build script module.
        CScriptBuilder builder;
        asIScriptModule* module = engine->GetModule(file->name.c_str());
        if (module == nullptr) {
            int r = builder.StartNewModule(engine, file->name.c_str());
            if (r < 0)
                Log() << "Couldn't start new module: " << file->name << ".\n";
            
            r = builder.AddSectionFromFile(filename.c_str());
            if (r < 0)
                Log() << "File section could not be added: " << filename << ".\n";
            
            r = builder.BuildModule();
            if (r < 0)
                Log() << "Compile errors.\n";
        } else {
            std::string script;
            LoadScriptFile(filename.c_str(), script);
            module->AddScriptSection(filename.c_str(), script.c_str());
            
            int r = module->Build();
            if (r < 0)
                Log() << file->name.c_str() << "Compile errors.\n";
        }
    }
}

void ScriptManager::Update(World& world) {
    // Init.
    for (Script* script : world.GetComponents<Script>()) {
        if (!script->initialized) {
            CreateInstance(script);
            script->initialized = true;
        }
    }
    
    // Update.
    for (Entity* entity : world.GetUpdateEntities())
        CallScript(entity->GetComponent<Script>(), "void Update()");
    
    // Handle messages.
    while (!messages.empty()) {
        std::vector<Message> temp = messages;
        messages.clear();
        
        for (const Message& message : temp)
            CallMessageReceived(message);
    }
    
    // Register entities for events.
    for (Entity* entity : updateEntities)
        world.RegisterUpdate(entity);
    updateEntities.clear();
}

void ScriptManager::RegisterUpdate(Entity* entity) {
    updateEntities.push_back(entity);
}

void ScriptManager::RegisterInput() {
    // Get the input enum.
    asUINT enumCount = engine->GetEnumCount();
    asITypeInfo* inputEnum = nullptr;
    for (asUINT i = 0; i < enumCount; ++i) {
        asITypeInfo* asEnum = engine->GetEnumByIndex(i);
        std::string name = asEnum->GetName();
        if (name == "input") {
            inputEnum = asEnum;
            break;
        }
    }
    
    for (std::size_t i = 0; i < Input::GetInstance().buttons.size(); ++i) {
        Input::Button* button = Input::GetInstance().buttons[i];
        
        // Check if we've already registered the button.
        bool registered = false;
        asUINT inputCount = inputEnum->GetEnumValueCount();
        for (asUINT j = 0; j < inputCount; ++j) {
            int value;
            std::string registeredButton = inputEnum->GetEnumValueByIndex(j, &value);
            if (registeredButton == button->action) {
                registered = true;
                break;
            }
        }
        
        if (!registered)
            engine->RegisterEnumValue("input", std::string(button->action).c_str(), i);
    }
}

void ScriptManager::SendMessage(Entity* recipient, int type) {
    Message message;
    message.recipient = recipient;
    message.type = type;
    messages.push_back(message);
}

void ScriptManager::CreateInstance(Component::Script* script) {
    currentEntity = script->entity;
    ScriptFile* scriptFile = script->scriptFile;
    
    // Find the class to instantiate.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find factory function / constructor.
    std::string factoryName = scriptFile->name + "@ " + scriptFile->name + "(Entity@)";
    asIScriptFunction* factoryFunction = type->GetFactoryByDecl(factoryName.c_str());
    if (factoryFunction == nullptr)
        Log() << "Couldn't find the factory function for " << scriptFile->name << ".\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(factoryFunction);
    context->SetArgObject(0, script->entity);
    ExecuteCall(context);
    
    // Get the newly created object.
    script->instance = *(static_cast<asIScriptObject**>(context->GetAddressOfReturnValue()));
    script->instance->AddRef();
    
    // Clean up.
    context->Release();
}

void ScriptManager::CallScript(Component::Script* script, const std::string& functionName) {
    currentEntity = script->entity;
    ScriptFile* scriptFile = script->scriptFile;
    
    // Get class.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find method to call.
    asIScriptFunction* method = type->GetMethodByDecl(functionName.c_str());
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(method);
    context->SetObject(script->instance);
    ExecuteCall(context);
    
    // Clean up.
    context->Release();
}

void ScriptManager::CallMessageReceived(const Message& message) {
    currentEntity = message.recipient;
    Component::Script* script = currentEntity->GetComponent<Component::Script>();
    ScriptFile* scriptFile = script->scriptFile;
    
    // Get class.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find method to call.
    asIScriptFunction* method = type->GetMethodByDecl("void ReceiveMessage(int)");
    if (method == nullptr)
        Log() << "Can't find method void ReceiveMessage(int)\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(method);
    context->SetObject(script->instance);
    context->SetArgDWord(0, message.type);
    ExecuteCall(context);
    
    // Clean up.
    context->Release();
}

void ScriptManager::LoadScriptFile(const char* fileName, std::string& script){
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

void ScriptManager::ExecuteCall(asIScriptContext* context) {
    int r = context->Execute();
    if (r != asEXECUTION_FINISHED) {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION) {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            Log() << "An exception '" << context->GetExceptionString() << "' occurred. Please correct the code and try again.\n";
        }
    }
}

asITypeInfo* ScriptManager::GetClass(const std::string& moduleName, const std::string& className) {
    // Get script module.
    asIScriptModule* module = engine->GetModule(moduleName.c_str(), asGM_ONLY_IF_EXISTS);
    if (module == nullptr)
        Log() << "Couldn't find \"" << moduleName << "\" module.\n";
    
    // Find the class.
    asUINT typeCount = module->GetObjectTypeCount();
    for (asUINT i = 0; i < typeCount; ++i) {
        asITypeInfo* type = module->GetObjectTypeByIndex(i);
        if (strcmp(type->GetName(), className.c_str()) == 0)
            return type;
    }
    
    Log() << "Couldn't find class \"" << className << "\".\n";
    return nullptr;
}
