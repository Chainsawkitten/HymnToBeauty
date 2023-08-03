\page Dawn Dawn

## Installing dependencies
Download and install [Python](https://www.python.org/downloads/).

Install [depot_tools](http://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up).

### Windows
Add `DEPOT_TOOLS_WIN_TOOLCHAIN` environment variable and set it to `0`.

## Setting up Dawn
Open a command prompt and navigate to the `externals` directory.

### Download the Dawn source code
```
git clone https://dawn.googlesource.com/dawn dawn
cd dawn
```

### Setup additional dependencies
```
cp scripts/standalone.gclient .gclient
gclient sync
```

## Configure Hymn to Beauty
Configure Hymn to Beauty and enable the WebGPURenderer option.