#pragma once

/// Handles scripting.
class ScriptManager {
    friend class Hub;
    
    public:
        /// Run a test script.
        void TestScripting();
        
    private:
        ScriptManager();
        ~ScriptManager();
        ScriptManager(ScriptManager const&) = delete;
        void operator=(ScriptManager const&) = delete;
};
