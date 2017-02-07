#pragma once

#include <string>
#include <json/json.h>
    
/// Wrapper for OpenAL buffers.
class ScriptFile {
    public:

		///The directory path of the script.
		std::string path;

		///The module the script executes in.
		std::string module = "";

		///The module the script executes in.
		std::string name = "";

		///Saves the scriptfile to Json value.
		Json::Value Save() const;

		///Loads the script from a Json value.
        /**
         * @param node JSON node to load from.
         */
		void ScriptFile::Load(const Json::Value& node);

};
