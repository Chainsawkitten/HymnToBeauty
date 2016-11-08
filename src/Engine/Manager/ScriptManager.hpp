#pragma once

#include <string>

class asIScriptEngine;

/// Handles scripting.
class ScriptManager {
    friend class Hub;
    
    public:
        /// Build a script that can later be run.
        /**
         * @param name Name of the script to build.
         */
        void BuildScript(const std::string& name);
        
        /// Run a test script.
        void TestScripting();
        
    private:
        ScriptManager();
        ~ScriptManager();
        ScriptManager(ScriptManager const&) = delete;
        void operator=(ScriptManager const&) = delete;
        
        asIScriptEngine* engine;
};
