\page wgpu

## Installing dependencies
Download and install the wgpu-native prerequisites described in the [docs](https://github.com/gfx-rs/wgpu-native/wiki/Getting-Started). You don't need the dependencies for the native examples.

## Setting up wgpu
Open a command prompt and navigate to the `externals` directory.

### Download the wgpu-native source code
```
git clone https://github.com/gfx-rs/wgpu-native.git wgpu-native
cd wgpu-native
git submodule update --init
```

### Build wgpu-native
```
cargo update
cargo build --release
```

## Building for Android
You need NDK version 23 or newer.

Navigate to the `externals/wgpu-native` directory.

### Setup aarch64 target
```
rustup target add aarch64-linux-android
```

### Setup environment variables
If you're using Windows as the host, run:
```
$env:BINDGEN_EXTRA_CLANG_ARGS="-isysroot NDK_DIRECTORY/toolchains/llvm/prebuilt/windows-x86_64/sysroot"
$env:CLANG_PATH="NDK_DIRECTORY/toolchains/llvm/prebuilt/windows-x86_64/bin/aarch64-linux-android24-clang++.cmd"
$env:CC=$env:CLANG_PATH
$env:CXX=$env:CLANG_PATH
$env:CARGO_TARGET_AARCH64_LINUX_ANDROID_LINKER=$env:CLANG_PATH
```
Replace `NDK_DIRECTORY` with the path to your NDK.

If you're using Linux as the host, run:
```
export BINDGEN_EXTRA_CLANG_ARGS="-isysroot NDK_DIRECTORY/toolchains/llvm/prebuilt/linux-x86_64/sysroot"
export CLANG_PATH="NDK_DIRECTORY/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang"
export CC=$CLANG_PATH
export CXX=$CLANG_PATH++
export CARGO_TARGET_AARCH64_LINUX_ANDROID_LINKER=$CLANG_PATH
```
Replace `NDK_DIRECTORY` with the path to your NDK.

### Build wgpu-native
```
cargo build --target aarch64-linux-android --release
```

## Configure Hymn to Beauty
Configure Hymn to Beauty and enable the WebGPURenderer option.

## Known issues
Local lights will not currently work in wgpu due to missing atomics support.