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
    
    // Find function to call.
    asIScriptFunction* function = module->GetFunctionByDecl("void main()");
    if (function == nullptr)
        Log() << "Couldn't find \"void main()\" function.\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(function);
    r = context->Execute();
    if (r != asEXECUTION_FINISHED) {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION) {
          // An exception occurred, let the script writer know what happened so it can be corrected.
          Log() << "An exception '" << context->GetExceptionString() << "' occurred. Please correct the code and try again.\n";
        }
    }
    
    // Clean up.
    context->Release();
    engine->ShutDownAndRelease();
}
