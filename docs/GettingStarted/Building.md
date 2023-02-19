\page Building Building

## Installing dependencies
Hymn to Beauty uses git and CMake. In addition, on Linux there are some libraries that need to be installed. You will also need a C++ compiler.

### Windows
Download [git](https://git-scm.com/download/win) and install it.

Get the latest Windows installer for [CMake](https://cmake.org/download/) and install it.

Hymn to Beauty is tested using [Visual Studio](https://visualstudio.microsoft.com/) (you can get the Community version for free), but you could also use [Clang](https://clang.llvm.org/).

### Mac
Get the latest binary distribution of [CMake](https://cmake.org/download/) and install it.

### Linux
To install all the dependencies, run the following command:

```
sudo apt install xorg-dev libgl1-mesa-dev libglu1-mesa-dev git cmake gcc
```

## Getting the source code
Hymn to Beauty uses git for version control.

To clone Hymn to Beauty and get all the source files you need, run:
```
git clone --recurse-submodules https://github.com/Chainsawkitten/HymnToBeauty.git
```

This will download the source code, including all submodules. Submodules are external dependencies used by Hymn to Beauty.

## Configuring CMake
Hymn to Beauty uses CMake. This means you can use whatever IDE/compiler you want, but first you need to run CMake in order to generate the appropriate project files.

First you need to create a directory for your build.

```
mkdir HymnToBeauty-build
cd HymnToBeauty-build
```

Then you generate the project files depending on which build system you're using.

### Make
This is the common way to build on Linux and Mac. To generate makefiles run:
```
cmake ../HymnToBeauty 
```

### Visual Studio
```
cmake ../HymnToBeauty -G "Visual Studio 15 2017"
```

See the [CMake documentation](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators) for which strings to use for which versions of Visual Studio.

## Building

### Make
Run
```
make
```

### Visual Studio
Open `HymnToBeauty.sln` and click on Build->Build Solution.

## Building the Vulkan backend
By default, Hymn to Beauty is built with the OpenGL backend, and without Vulkan.

If you want to build Hymn to Beauty with the Vulkan backend, you need to install the [Vulkan SDK](https://vulkan.lunarg.com/).

Additionally, you need to enable the VulkanRenderer CMake option when configuring.

## Building the documentation
[Doxygen](https://www.doxygen.nl/index.html) is used to build this documentation. You don't need to build the documentation yourself. You can, after all, continue reading this. But if you do want to build the documentation, you need to install Doxygen.

To install Doxygen, download the binaries of the latest release. If you're on Linux, you can alternatively install Doxygen through apt:
```
sudo apt install doxygen
```

Then use CMake to configure the project as previously described. There will now be a `docs` target that can be built. This target will not be built by default and needs to be built explicitly.

### Make
```
make docs
```

### Visual Studio
Right click on the `docs` target in the solution and press Build.

## Subpages
 - \subpage BuildingAndroid
 - \subpage BuildingWebGPU
