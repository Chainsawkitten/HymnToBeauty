# ShaderProcessor
Tool to convert GLSL shaders into C++ files containing both GLSL and SPIR-V versions.

## Building
Use [CMake](http://www.cmake.org/) to generate make/project files for your compiler/environment and then compile.

## Usage
    ShaderProcessor input output [--vulkan]

--vulkan Generate SPIR-V in addition to GLSL. Otherwise SPIR-V section in output is left blank. Requires having glslangvalidator in your path.