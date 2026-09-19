# LearnOpenGL

A project built on [SDL3](https://wiki.libsdl.org/SDL3/README-linux#build-dependencies). See that page for required system build dependencies.

## Building

Fetch submodules once before the first build:

```sh
git submodule update --init --recursive
```

### Debug

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

### Release

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
