@echo off
mkdir build_vs2019
pushd build_vs2019
"C:\Program Files\CMake\bin\cmake" -G "Visual Studio 16 2019" -A x64 -DADDITIONAL_FIND_PATH="C:\Qt\5.15.2\msvc2019_64" -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE="C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake" ..
popd
