# System Audio Sharing

A simple project consisting of two subprojects, `sender` and `receiver`. Its main purpose is to transmit system audio from the sender to the receiver over a TCP connection.

The project was originally developed for personal use and as a learning project to explore PC audio systems.

## Building

### Prerequisites

Make sure you have the following:

- A desktop operating system with [CMake](https://cmake.org/) installed and a working C/C++ compiler that supports modern C++23 features.
- To build `sender`, you need Windows 10 or later with support for [WASAPI](https://learn.microsoft.com/en-us/windows/win32/coreaudio/wasapi).

### Getting the Source Code

Clone the repository:

```shell
git clone https://github.com/stra-lib/sas.git
cd sas
```

### Configuring and Building

Configure the CMake project. Depending on your environment, this may take a few minutes.

```shell
# Append '-DCMAKE_BUILD_TYPE=Debug' to create a debug build.
cmake -B build
```

Build the desired target by specifying either `sender` or `receiver`:

```shell
cmake --build build --target receiver
```

Once the build is complete, you can find the executable in the corresponding target directory under `build/`.


