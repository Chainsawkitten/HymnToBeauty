\page BuildingAndroid Building for Android

## Installing dependencies

Download and install [Android Studio](https://developer.android.com/studio), [git](https://git-scm.com/download/win) and [CMake](https://cmake.org/download/).

You will also need the [Vulkan SDK](https://vulkan.lunarg.com/). The OpenGL backend is not supported on Android.

Some tools will be built for the host architecture, so you will also need a C++ compiler for your operating system.

### Windows
Hymn to Beauty is tested using [Visual Studio](https://visualstudio.microsoft.com/) (you can get the Community version for free), but you could also use [Clang](https://clang.llvm.org/).

### Mac
Use clang.

### Linux
Use gcc or clang.

## Building
Navigate to `src/Game/Android` and build using:
```
./gradlew build
```

An apk will be generated in `src/Game/Android/app/build/outputs/apk`. The editor is not supported on Android.