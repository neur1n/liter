# liter
Liter is a command line tool to manipulate endpoint device volume.


## Table of Contents
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)


## Features
1. Get, set, mute, step up, or step down the volume of the **default***
endpoint device (e.g. a headphone).
2. If an endpoint device is connected or disconnected to PC, liter should
detect the change and allow manipulating the new default endpoint device.

\* [Windows API](https://docs.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdeviceenumerator-getdefaultaudioendpoint)


## Installation
### Pre-built Binaries
Checkout [releases](https://github.com/Neur1n/liter/releases).

### Build from Source
#### Dependencies
- **Linux**: (not supported)
- **MacOS**: (not supported)
- **Windows 7, 8, 8.1**: (not tested)
- **Windows 10**: [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/)

#### Building
The `CMakeLists.txt` is provided as an example instead of a ready-to-use CMake
file, since some environment parameters require manually setup. Now let's break
it down.
```cmake
cmake_minimum_required(VERSION 2.8.2)

project(liter)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)  # Set to 1 to enable generating the compilation database.

set(CMAKE_BUILD_TYPE Release)         # Set to Debug if you need to debug.

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

# WINDOWS_SDK_INCLUDE_DIR can be set in CMakeLists.txt or system environment
# variables in Windows. The value should be something like:
#   C:\Windows Kits\10\Include\10.0.17763.0
include_directories(
  ${WINDOWS_SDK_INCLUDE_DIR}/um
  include
)

# Similarly, WINDOWS_SDK_LIB_DIR should be something like:
#   C:\Windows Kits\10\Lib\10.0.17763.0
link_directories(
  ${WINDOWS_SDK_LIB_DIR}/um/x86
)

add_executable(liter src/main.cpp src/liter.cpp)
```

## Usage
The executable of liter takes no arguments, instead, it takes in arguments
after it is launched.

For demonstration or debugging purpose, one may set the `DEBUG` macro to `true`
in `main.cpp` to enable echoing of the inputs. The following demonstrations are
based on such configuration.

### Get Volume
To get the current volume level (0.0 ~ 1.0), pass `g` (i.e. "get") when
`Action:` is prompted in the terminal. Then the volume level will be shown as
`Volume: <level>`. In Windows, the range of volume is from 0 to 100, therefore
a volume level 0.1 means a volume of 10.
```
$ liter
Action: g
Volume: 0.1
```

If `DEBUG` is set to `false`, the interaction should be like:
```
$ liter
g
0.1
```

Which make it easier for a wrapper to parse the stdout result.

### Set Volume
To set a volume level, pass `s` ("set") when `Action:` is prompted
in the terminal. Then it will ask you for another input for `Param:` which is
for the target volume level. The following example sets the volume to 12.
```
$ liter
Action: s
Param: 0.12
Volume: 0.12
```

If a number smaller than 0.0 or bigger than 1.0 is passed, the feedback will be
`-1`.
```
$ liter
Action: s
Param: 1.3
Volume: -1
```

Please notice that, if 0.0 is passed, the volume is set to 0 but you may still
hear sounds, which is different from mute.

### Mute Volume
To mute, besides passing `m` ("mute") when `Action:` is prompted, another
argument is required for `Param:` - non-zero for mute, zero for un-mute. When
mute, the feedback is `0`; when un-mute, the volume level before mute is
feedbacked.

Please notice that, the volume level is in fact remained when mute, but `0` is
feedbacked instead for better indication. To get the volume level after mute,
use `g`.
```
$ liter
Action: g
Volume: 0.12

Action: m
Param: 1.0
Volume: 0

Action: g
Volume: 0.12
```

```
$ liter
Action: m
Param: 0.0
Volume: 0.2
```

### Step Up Volume
To step up the volume, besides passing `u` ("up") when `Action:` is prompted.
In Windows 10, the default step is 0.02.
```
$ liter
Action: g
Volume: 0.12

Action: u
Volume: 0.14
```

### Step Down Volume
To step down the volume, besides passing `d` ("down") when `Action:` is
prompted.  In Windows 10, the default step is 0.02.
```
$ liter
Action: g
Volume: 0.14

Action: d
Volume: 0.12
```
