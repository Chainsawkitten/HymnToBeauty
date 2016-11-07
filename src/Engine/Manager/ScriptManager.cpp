#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include "../Util/Log.hpp"

void MessageCallback(const asSMessageInfo* message, void* param) {
    Log() << message->section << " (" << message->row << ", " << message->col << " : ";
    
    switch (message->type) {
    case asMSGTYPE_ERROR:
        Log() << "Error";
        break;
    case asMSGTYPE_INFORMATION:
        Log() << "Information";
        break;
    case asMSGTYPE_WARNING:
        Log() << "Warning";
        break;
    }
    
    Log() << " : " << message->message << "\n";
}

void print(const std::string& message) {
    Log() << message;
}

ScriptManager::ScriptManager() {
    
}

ScriptManager::~ScriptManager() {
    
}

void ScriptManager::TestScripting() {
    // Create the script engine
    asIScriptEngine* engine = asCreateScriptEngine();
    
    // Set the message callback to receive information on errors in human readable form.
    int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert(r >= 0);
    
    // Register add-ons.
    RegisterStdString(engine);
    
    // Register functions.
    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert(r >= 0);
    
    // Define the test script.
    std::string script = "void main() { print(\"Hello world\\n\"); }";
    
    // Create, load and build script module.
    asIScriptModule* module = engine->GetModule("module", asGM_ALWAYS_CREATE);
    module->AddScriptSection("script.as", script.c_str());
    r = module->Build();
    if (r < 0)
        Log() << "Couldn't build script module.\n";
    
    // Clean up.
    engine->ShutDownAndRelease();
}
