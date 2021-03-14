#pragma once

#include <vector>
#include <string>
#include <json/json.h>

/// Information about a file containing a script.
class ScriptFile {
  public:
    /// Saves the scriptfile to Json value.
    /**
     * @return The saved data in a JSON node.
     */
    Json::Value Save() const;

    /// Loads the script information.
    /**
     * @param name Name of the script.
     */
    void Load(const std::string& name);

    /// The name of the script.
    std::string name = "";

    /// The folder containing the script file.
    std::string path;

    /// The name of the module.
    std::string module = "";

    /// A list containing all the functions for the script.
    std::vector<std::string> functionList;
};
