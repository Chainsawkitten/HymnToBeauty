#include "Shader.hpp"

#include <cstring>
#include <iostream>
#include <sstream>
#include <locale>

using namespace std;

int main(int argc, const char* argv[]) {
    // Print help
    if (argc == 1 || strcmp(argv[0], "--help") == 0) {
        cout << "ShaderProcessor is a tool to generate shader variants for GLSL shaders." << endl
             << "Usage:" << endl
             << "ShaderProcessor input output [--vulkan]" << endl;
        return 0;
    }
    
    // Get input and output names.
    string inputName = argv[1];
    string outputName;
    if (argc > 2)
        outputName = argv[2];
    else
        outputName = inputName;

    // Process remaining arguments.
    bool vulkan = false;
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--vulkan") == 0) {
            vulkan = true;
        }
    }

    // Read input file.
    Shader shader;
    
    if (!shader.Open(inputName))
        return 1;

    ///@todo shader.ParseVariants();

    string headerName = outputName + ".hpp";
    if (!shader.WriteHeader(headerName))
        return 1;

    if (!shader.WriteSource(outputName + ".cpp", headerName, vulkan))
        return 1;
    
    return 0;
}
