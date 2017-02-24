#pragma once

#include <string>
#include <json/json.h>
    
/// Information about a file containing a script.
class ScriptFile {
    public:
        /// The name of the script.
        std::string name = "";

        /// The module the script executes in.
        std::string module = "";
        
        /// Saves the scriptfile to Json value.
        /**
         * @return The saved data in a JSON node.
         */
        Json::Value Save() const;
        
        ///Loads the script from a Json value.
        /**
         * @param node JSON node to load from.
         */
        void Load(const Json::Value& node);
};
