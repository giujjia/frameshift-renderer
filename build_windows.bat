@echo off
setlocal EnableExtensions EnableDelayedExpansion
chcp 65001 >nul

rem ============================================================
rem Frameshift Renderer / ShiftGL - Windows build script v9
rem Place this file in the project root, beside the "java" and "native" folders.
rem
rem This script intentionally avoids CMake's FindJNI.cmake on Windows because
rem some CMake/JDK/Oracle javapath combinations can break on paths like
rem C:\Program Files\Common Files\Oracle\Java. JNI include paths are supplied
rem explicitly from a real JDK.
rem
rem Optional overrides:
rem   set FRAMESHIFT_CMAKE_GENERATOR=MinGW Makefiles
rem   set FRAMESHIFT_CMAKE_GENERATOR=Ninja
rem   set FRAMESHIFT_CMAKE_GENERATOR=NMake Makefiles
rem   set FRAMESHIFT_CMAKE_PLATFORM=x64
rem   set VCPKG_ROOT=C:\vcpkg
rem   set VCPKG_TARGET_TRIPLET=x64-windows
rem   set JAVA_HOME=C:\Program Files\Java\jdk-21
rem v8: Java source response file uses forward slashes.
rem v9: Prefer the real motor.dll output and strip trailing backslashes from
rem native_bin_path.txt. A trailing backslash before a closing quote can make
rem java.exe parse its arguments incorrectly and print only the usage text.
rem ============================================================

set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"
set "JAVA_DIR=%ROOT_DIR%\java"
set "NATIVE_DIR=%ROOT_DIR%\native"
set "JAVA_BUILD=%JAVA_DIR%\build"
set "MARKER=%ROOT_DIR%\.frameshift_native_build_dir.txt"

rem Copy Program Files environment variables into simple names before using them
rem inside parenthesized IF/FOR blocks. The original variable name ProgramFiles(x86)
rem contains parentheses and can break cmd.exe parsing when expanded directly.
set "PF64=%ProgramFiles%"
set "PFW6432=%ProgramW6432%"
set "PF86=%ProgramFiles(x86)%"
if not defined PF64 set "PF64=C:\Program Files"
if not defined PFW6432 set "PFW6432=%PF64%"



echo.
echo ============================================================
echo Checking project structure
echo ============================================================
if not exist "%JAVA_DIR%\src\main\MainWindow.java" (
    echo [ERROR] MainWindow.java was not found at:
    echo         "%JAVA_DIR%\src\main\MainWindow.java"
    echo         Put this .bat in the project root.
    exit /b 1
)
if not exist "%NATIVE_DIR%\src\bridge.cpp" (
    echo [ERROR] Native sources were not found at:
    echo         "%NATIVE_DIR%\src"
    echo         Put this .bat in the project root.
    exit /b 1
)


echo.
echo ============================================================
echo Checking required tools
echo ============================================================
where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] CMake was not found in PATH.
    echo         Install CMake and enable "Add CMake to PATH".
    exit /b 1
)
set "FIRST_CMAKE="
for /f "delims=" %%P in ('where cmake 2^>nul') do if not defined FIRST_CMAKE set "FIRST_CMAKE=%%P"
echo [OK] CMake: %FIRST_CMAKE%

rem Locate a real JDK without PowerShell. Oracle's Common Files\Oracle\Java javapath is not enough.
set "JDK_HOME_FOUND="
set "JDK_CAND="

rem 1) Respect JAVA_HOME only if it points to a complete JDK.
if defined JAVA_HOME (
    set "JDK_CAND=%JAVA_HOME%"
    if exist "!JDK_CAND!\include\jni.h" if exist "!JDK_CAND!\include\win32\jni_md.h" if exist "!JDK_CAND!\bin\java.exe" if exist "!JDK_CAND!\bin\javac.exe" set "JDK_HOME_FOUND=!JDK_CAND!"
)

rem 2) Try javac.exe in PATH and infer the JDK root from ...\bin\javac.exe.
if not defined JDK_HOME_FOUND (
    for /f "delims=" %%P in ('where javac.exe 2^>nul') do (
        if not defined JDK_HOME_FOUND (
            for %%B in ("%%~dpP..") do set "JDK_CAND=%%~fB"
            if exist "!JDK_CAND!\include\jni.h" if exist "!JDK_CAND!\include\win32\jni_md.h" if exist "!JDK_CAND!\bin\java.exe" if exist "!JDK_CAND!\bin\javac.exe" set "JDK_HOME_FOUND=!JDK_CAND!"
        )
    )
)

rem 3) Search common 64-bit JDK installation directories. Only accept folders with JNI headers.
if not defined JDK_HOME_FOUND (
    for %%R in ("!PF64!\Java" "!PFW6432!\Java" "!PF64!\Eclipse Adoptium" "!PFW6432!\Eclipse Adoptium" "!PF64!\Microsoft" "!PFW6432!\Microsoft" "!PF64!\Amazon Corretto" "!PFW6432!\Amazon Corretto" "!PF64!\BellSoft" "!PFW6432!\BellSoft" "!PF64!\Zulu" "!PFW6432!\Zulu") do (
        if not defined JDK_HOME_FOUND if exist "%%~R" (
            for /f "delims=" %%D in ('dir /b /ad /o-n "%%~R" 2^>nul') do (
                if not defined JDK_HOME_FOUND (
                    set "JDK_CAND=%%~R\%%D"
                    if exist "!JDK_CAND!\include\jni.h" if exist "!JDK_CAND!\include\win32\jni_md.h" if exist "!JDK_CAND!\bin\java.exe" if exist "!JDK_CAND!\bin\javac.exe" set "JDK_HOME_FOUND=!JDK_CAND!"
                )
            )
        )
    )
)

rem 4) Also check 32-bit Program Files as a last fallback. Prefer 64-bit JDK above.
if not defined JDK_HOME_FOUND (
    for %%R in ("!PF86!\Java" "!PF86!\Eclipse Adoptium" "!PF86!\Microsoft" "!PF86!\Amazon Corretto" "!PF86!\BellSoft" "!PF86!\Zulu") do (
        if not defined JDK_HOME_FOUND if exist "%%~R" (
            for /f "delims=" %%D in ('dir /b /ad /o-n "%%~R" 2^>nul') do (
                if not defined JDK_HOME_FOUND (
                    set "JDK_CAND=%%~R\%%D"
                    if exist "!JDK_CAND!\include\jni.h" if exist "!JDK_CAND!\include\win32\jni_md.h" if exist "!JDK_CAND!\bin\java.exe" if exist "!JDK_CAND!\bin\javac.exe" set "JDK_HOME_FOUND=!JDK_CAND!"
                )
            )
        )
    )
)

set "JDK_HOME=%JDK_HOME_FOUND%"
if not defined JDK_HOME (
    echo [ERROR] A full JDK with JNI headers was not found.
    echo         Install a 64-bit JDK 17+ or 21+ and set JAVA_HOME to its root folder.
    echo         Example:
    echo           set JAVA_HOME=C:\Program Files\Java\jdk-21
    echo         The folder must contain include\jni.h and include\win32\jni_md.h.
    echo         Do not use C:\Program Files\Common Files\Oracle\Java as JAVA_HOME.
    exit /b 1
)
for %%I in ("%JDK_HOME%") do set "JDK_HOME=%%~fI"
set "JAVA_EXE=%JDK_HOME%\bin\java.exe"
set "JAVAC_EXE=%JDK_HOME%\bin\javac.exe"
set "JNI_INCLUDE=%JDK_HOME%\include"
set "JNI_INCLUDE_WIN32=%JDK_HOME%\include\win32"

if not exist "%JAVA_EXE%" (
    echo [ERROR] java.exe not found in detected JDK: "%JAVA_EXE%"
    exit /b 1
)
if not exist "%JAVAC_EXE%" (
    echo [ERROR] javac.exe not found in detected JDK: "%JAVAC_EXE%"
    exit /b 1
)
if not exist "%JNI_INCLUDE%\jni.h" (
    echo [ERROR] jni.h not found: "%JNI_INCLUDE%\jni.h"
    exit /b 1
)
if not exist "%JNI_INCLUDE_WIN32%\jni_md.h" (
    echo [ERROR] jni_md.h not found: "%JNI_INCLUDE_WIN32%\jni_md.h"
    exit /b 1
)

echo [OK] JDK/JNI: %JDK_HOME%
set "PATH=!JDK_HOME!\bin;!PATH!"


echo.
echo ============================================================
echo Checking runtime data files
echo ============================================================
set "MISSING_DATA=0"
for %%D in (codons.tsv refseqHumanFullNM.fasta nm_cds_positions.tsv Antonio-Bold.ttf) do (
    if exist "%JAVA_DIR%\data\%%D" (
        echo [OK] java\data\%%D
    ) else (
        echo [WARN] Missing: java\data\%%D
        set "MISSING_DATA=1"
    )
)
if "%MISSING_DATA%"=="1" (
    echo.
    echo [WARN] The project can compile without all data files, but mutation processing needs them.
)


echo.
echo ============================================================
echo Selecting CMake generator
echo ============================================================
set "GENERATOR_NAME="
set "GENERATOR_TAG="
set "GENERATOR_PLATFORM_ARG="
set "C_COMPILER="
set "CXX_COMPILER="
set "TOOLCHAIN_BIN="

if defined FRAMESHIFT_CMAKE_GENERATOR (
    set "GENERATOR_NAME=%FRAMESHIFT_CMAKE_GENERATOR%"
    set "GENERATOR_TAG=manual"
    if defined FRAMESHIFT_CMAKE_PLATFORM set "GENERATOR_PLATFORM_ARG=-A %FRAMESHIFT_CMAKE_PLATFORM%"
)

if not defined GENERATOR_NAME (
    where cl >nul 2>nul
    if not errorlevel 1 (
        where nmake >nul 2>nul
        if not errorlevel 1 (
            set "GENERATOR_NAME=NMake Makefiles"
            set "GENERATOR_TAG=msvc-nmake"
        )
    )
)

if not defined GENERATOR_NAME (
    set "VSWHERE="
    if defined PF86 if exist "!PF86!\Microsoft Visual Studio\Installer\vswhere.exe" set "VSWHERE=!PF86!\Microsoft Visual Studio\Installer\vswhere.exe"
    if not defined VSWHERE if exist "!PF64!\Microsoft Visual Studio\Installer\vswhere.exe" set "VSWHERE=!PF64!\Microsoft Visual Studio\Installer\vswhere.exe"
    if defined VSWHERE (
        set "VS_INSTALL="
        for /f "usebackq delims=" %%I in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2^>nul`) do if not defined VS_INSTALL set "VS_INSTALL=%%I"
        if defined VS_INSTALL if exist "!VS_INSTALL!\Common7\Tools\VsDevCmd.bat" (
            echo [INFO] Loading Visual Studio build environment: !VS_INSTALL!
            call "!VS_INSTALL!\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
            where cl >nul 2>nul
            if not errorlevel 1 (
                where nmake >nul 2>nul
                if not errorlevel 1 (
                    set "GENERATOR_NAME=NMake Makefiles"
                    set "GENERATOR_TAG=msvc-nmake"
                ) else (
                    where ninja >nul 2>nul
                    if not errorlevel 1 (
                        set "GENERATOR_NAME=Ninja"
                        set "GENERATOR_TAG=msvc-ninja"
                    )
                )
            )
        )
    )
)

if not defined GENERATOR_NAME (
    where gcc >nul 2>nul
    if not errorlevel 1 (
        where g++ >nul 2>nul
        if not errorlevel 1 (
            for /f "delims=" %%P in ('where gcc 2^>nul') do if not defined C_COMPILER set "C_COMPILER=%%P"
            for /f "delims=" %%P in ('where g++ 2^>nul') do if not defined CXX_COMPILER set "CXX_COMPILER=%%P"
            for %%I in ("!CXX_COMPILER!") do set "TOOLCHAIN_BIN=%%~dpI"
            where mingw32-make >nul 2>nul
            if not errorlevel 1 (
                set "GENERATOR_NAME=MinGW Makefiles"
                set "GENERATOR_TAG=mingw"
            ) else (
                where ninja >nul 2>nul
                if not errorlevel 1 (
                    set "GENERATOR_NAME=Ninja"
                    set "GENERATOR_TAG=mingw-ninja"
                )
            )
        )
    )
)

if not defined GENERATOR_NAME (
    echo [ERROR] No usable C/C++ build toolchain was detected.
    echo.
    echo Install one of these:
    echo   1^) Visual Studio Build Tools 2022 + "Desktop development with C++".
    echo   2^) MSYS2/MinGW-w64 with gcc, g++, mingw32-make, GLFW and FreeType.
    echo.
    echo CMake alone is not enough; it needs a compiler and a build tool.
    exit /b 1
)

echo [OK] CMake generator: !GENERATOR_NAME!
if defined GENERATOR_PLATFORM_ARG echo [OK] CMake platform arg: !GENERATOR_PLATFORM_ARG!
if defined C_COMPILER echo [OK] C compiler: !C_COMPILER!
if defined CXX_COMPILER echo [OK] C++ compiler: !CXX_COMPILER!

set "NATIVE_BUILD=%NATIVE_DIR%\build-windows-%GENERATOR_TAG%"
set "CMAKE_WRAPPER_DIR=%NATIVE_DIR%\build-windows-cmake-wrapper"

if /I "%~1"=="clean" (
    if exist "%NATIVE_BUILD%" rmdir /s /q "%NATIVE_BUILD%"
    if exist "%CMAKE_WRAPPER_DIR%" rmdir /s /q "%CMAKE_WRAPPER_DIR%"
)

rem Detect vcpkg if present.
rem v7: Prefer an explicit user vcpkg root such as C:\vcpkg over CMAKE_TOOLCHAIN_FILE
rem inherited from Visual Studio. Visual Studio Build Tools may define a bundled
rem vcpkg toolchain that does not contain the packages installed in C:\vcpkg.
set "VCPKG_TOOLCHAIN="
set "VCPKG_BIN="
set "VCPKG_ROOT_DETECTED="
set "ORIGINAL_CMAKE_TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN_FILE%"

if defined VCPKG_TARGET_TRIPLET (
    set "VCPKG_TRIPLET=%VCPKG_TARGET_TRIPLET%"
) else (
    if /I "%GENERATOR_TAG:~0,5%"=="mingw" (
        set "VCPKG_TRIPLET=x64-mingw-dynamic"
    ) else (
        set "VCPKG_TRIPLET=x64-windows"
    )
)

rem If C:\vcpkg exists, use it first because this is where the user-installed
rem packages normally are. Then try VCPKG_ROOT and local/user copies.
if exist "C:\vcpkg\scripts\buildsystems\vcpkg.cmake" (
    set "VCPKG_ROOT=C:\vcpkg"
    set "VCPKG_TOOLCHAIN=C:\vcpkg\scripts\buildsystems\vcpkg.cmake"
)
if not defined VCPKG_TOOLCHAIN if defined VCPKG_ROOT if exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" set "VCPKG_TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
if not defined VCPKG_TOOLCHAIN if exist "%ROOT_DIR%\vcpkg\scripts\buildsystems\vcpkg.cmake" set "VCPKG_TOOLCHAIN=%ROOT_DIR%\vcpkg\scripts\buildsystems\vcpkg.cmake"
if not defined VCPKG_TOOLCHAIN if exist "%USERPROFILE%\vcpkg\scripts\buildsystems\vcpkg.cmake" set "VCPKG_TOOLCHAIN=%USERPROFILE%\vcpkg\scripts\buildsystems\vcpkg.cmake"

rem Only use inherited CMAKE_TOOLCHAIN_FILE as a last resort.
if not defined VCPKG_TOOLCHAIN if defined ORIGINAL_CMAKE_TOOLCHAIN_FILE if exist "%ORIGINAL_CMAKE_TOOLCHAIN_FILE%" set "VCPKG_TOOLCHAIN=%ORIGINAL_CMAKE_TOOLCHAIN_FILE%"

if defined VCPKG_TOOLCHAIN (
    if defined ORIGINAL_CMAKE_TOOLCHAIN_FILE (
        if /I not "!ORIGINAL_CMAKE_TOOLCHAIN_FILE!"=="!VCPKG_TOOLCHAIN!" echo [INFO] Ignoring inherited CMAKE_TOOLCHAIN_FILE: !ORIGINAL_CMAKE_TOOLCHAIN_FILE!
    )
    for %%I in ("%VCPKG_TOOLCHAIN%") do set "VCPKG_TOOLCHAIN_DIR=%%~dpI"
    for %%I in ("!VCPKG_TOOLCHAIN_DIR!..\..") do set "VCPKG_ROOT_DETECTED=%%~fI"
    set "VCPKG_ROOT=!VCPKG_ROOT_DETECTED!"
    set "CMAKE_TOOLCHAIN_FILE=!VCPKG_TOOLCHAIN!"
    if exist "!VCPKG_ROOT_DETECTED!\installed\!VCPKG_TRIPLET!\bin" set "VCPKG_BIN=!VCPKG_ROOT_DETECTED!\installed\!VCPKG_TRIPLET!\bin"
    if exist "!VCPKG_ROOT_DETECTED!\installed\!VCPKG_TRIPLET!" set "CMAKE_PREFIX_PATH_FORCE=!VCPKG_ROOT_DETECTED!\installed\!VCPKG_TRIPLET!"
    echo [INFO] vcpkg root: !VCPKG_ROOT_DETECTED!
    echo [INFO] vcpkg toolchain: !VCPKG_TOOLCHAIN!
    echo [INFO] vcpkg triplet: !VCPKG_TRIPLET!
) else (
    echo [INFO] vcpkg not detected. CMake will search system paths for GLFW and FreeType.
)

rem Normalize paths for CMake and JAVA_HOME env. This avoids invalid escape sequences in CMake.
set "ROOT_CMAKE=%ROOT_DIR:\=/%"
set "NATIVE_CMAKE=%NATIVE_DIR:\=/%"
set "JDK_CMAKE=%JDK_HOME:\=/%"
set "JNI_INCLUDE_CMAKE=%JNI_INCLUDE:\=/%"
set "JNI_INCLUDE_WIN32_CMAKE=%JNI_INCLUDE_WIN32:\=/%"
set "JAVA_EXE_CMAKE=%JAVA_EXE:\=/%"
set "JAVAC_EXE_CMAKE=%JAVAC_EXE:\=/%"
if defined VCPKG_TOOLCHAIN set "VCPKG_TOOLCHAIN_CMAKE=%VCPKG_TOOLCHAIN:\=/%"
if defined CMAKE_PREFIX_PATH_FORCE set "CMAKE_PREFIX_PATH_FORCE_CMAKE=%CMAKE_PREFIX_PATH_FORCE:\=/%"
if defined C_COMPILER set "C_COMPILER_CMAKE=%C_COMPILER:\=/%"
if defined CXX_COMPILER set "CXX_COMPILER_CMAKE=%CXX_COMPILER:\=/%"

rem Important: set JAVA_HOME with forward slashes for the cmake process.
set "JAVA_HOME=%JDK_CMAKE%"


echo.
echo ============================================================
echo Generating Windows CMake wrapper
echo ============================================================
if not exist "%CMAKE_WRAPPER_DIR%" mkdir "%CMAKE_WRAPPER_DIR%"
> "%CMAKE_WRAPPER_DIR%\CMakeLists.txt" (
    echo cmake_minimum_required^(VERSION 3.10^)
    echo project^(ShiftGLWindows VERSION 1.0 LANGUAGES C CXX^)
    echo set^(CMAKE_CXX_STANDARD 17^)
    echo set^(CMAKE_CXX_STANDARD_REQUIRED True^)
    echo set^(NATIVE_ROOT "%NATIVE_CMAKE%"^)
    echo set^(JDK_INCLUDE "%JNI_INCLUDE_CMAKE%"^)
    echo set^(JDK_INCLUDE_WIN32 "%JNI_INCLUDE_WIN32_CMAKE%"^)
    echo find_package^(OpenGL REQUIRED^)
    echo find_package^(glfw3 REQUIRED^)
    echo find_package^(Freetype REQUIRED^)
    echo add_library^(motor SHARED
    echo     "${NATIVE_ROOT}/src/bridge.cpp"
    echo     "${NATIVE_ROOT}/src/renderer.cpp"
    echo     "${NATIVE_ROOT}/src/text_renderer.cpp"
    echo     "${NATIVE_ROOT}/src/glad.c"
    echo ^)
    echo target_include_directories^(motor PRIVATE "${NATIVE_ROOT}/include" "${JDK_INCLUDE}" "${JDK_INCLUDE_WIN32}" ${FREETYPE_INCLUDE_DIRS}^)
    echo target_link_libraries^(motor PRIVATE glfw OpenGL::GL ${FREETYPE_LIBRARIES}^)
    echo if^(TARGET Freetype::Freetype^)
    echo     target_link_libraries^(motor PRIVATE Freetype::Freetype^)
    echo endif^(^)
    echo add_executable^(ShiftGL_Test
    echo     "${NATIVE_ROOT}/src/renderer.cpp"
    echo     "${NATIVE_ROOT}/src/text_renderer.cpp"
    echo     "${NATIVE_ROOT}/src/glad.c"
    echo ^)
    echo target_include_directories^(ShiftGL_Test PRIVATE "${NATIVE_ROOT}/include" "${JDK_INCLUDE}" "${JDK_INCLUDE_WIN32}" ${FREETYPE_INCLUDE_DIRS}^)
    echo target_link_libraries^(ShiftGL_Test PRIVATE glfw OpenGL::GL ${FREETYPE_LIBRARIES}^)
    echo if^(TARGET Freetype::Freetype^)
    echo     target_link_libraries^(ShiftGL_Test PRIVATE Freetype::Freetype^)
    echo endif^(^)
    echo target_compile_definitions^(ShiftGL_Test PRIVATE SHIFTGL_STANDALONE^)
)
echo [OK] Wrapper: %CMAKE_WRAPPER_DIR%\CMakeLists.txt


echo.
echo ============================================================
echo Configuring native C++ build
echo ============================================================
if not exist "%NATIVE_BUILD%" mkdir "%NATIVE_BUILD%"

if defined VCPKG_TOOLCHAIN_CMAKE (
    if defined C_COMPILER_CMAKE (
        cmake -S "!CMAKE_WRAPPER_DIR!" -B "!NATIVE_BUILD!" -G "!GENERATOR_NAME!" !GENERATOR_PLATFORM_ARG! -DCMAKE_BUILD_TYPE=Release "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=!VCPKG_TOOLCHAIN_CMAKE!" "-DVCPKG_TARGET_TRIPLET=!VCPKG_TRIPLET!" "-DCMAKE_PREFIX_PATH:PATH=!CMAKE_PREFIX_PATH_FORCE_CMAKE!" "-DCMAKE_C_COMPILER:FILEPATH=!C_COMPILER_CMAKE!" "-DCMAKE_CXX_COMPILER:FILEPATH=!CXX_COMPILER_CMAKE!" "-DJAVA_HOME:PATH=!JDK_CMAKE!" "-DJava_JAVA_EXECUTABLE:FILEPATH=!JAVA_EXE_CMAKE!" "-DJava_JAVAC_EXECUTABLE:FILEPATH=!JAVAC_EXE_CMAKE!"
    ) else (
        cmake -S "!CMAKE_WRAPPER_DIR!" -B "!NATIVE_BUILD!" -G "!GENERATOR_NAME!" !GENERATOR_PLATFORM_ARG! -DCMAKE_BUILD_TYPE=Release "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=!VCPKG_TOOLCHAIN_CMAKE!" "-DVCPKG_TARGET_TRIPLET=!VCPKG_TRIPLET!" "-DCMAKE_PREFIX_PATH:PATH=!CMAKE_PREFIX_PATH_FORCE_CMAKE!" "-DJAVA_HOME:PATH=!JDK_CMAKE!" "-DJava_JAVA_EXECUTABLE:FILEPATH=!JAVA_EXE_CMAKE!" "-DJava_JAVAC_EXECUTABLE:FILEPATH=!JAVAC_EXE_CMAKE!"
    )
) else (
    if defined C_COMPILER_CMAKE (
        cmake -S "!CMAKE_WRAPPER_DIR!" -B "!NATIVE_BUILD!" -G "!GENERATOR_NAME!" !GENERATOR_PLATFORM_ARG! -DCMAKE_BUILD_TYPE=Release "-DCMAKE_C_COMPILER:FILEPATH=!C_COMPILER_CMAKE!" "-DCMAKE_CXX_COMPILER:FILEPATH=!CXX_COMPILER_CMAKE!" "-DJAVA_HOME:PATH=!JDK_CMAKE!" "-DJava_JAVA_EXECUTABLE:FILEPATH=!JAVA_EXE_CMAKE!" "-DJava_JAVAC_EXECUTABLE:FILEPATH=!JAVAC_EXE_CMAKE!"
    ) else (
        cmake -S "!CMAKE_WRAPPER_DIR!" -B "!NATIVE_BUILD!" -G "!GENERATOR_NAME!" !GENERATOR_PLATFORM_ARG! -DCMAKE_BUILD_TYPE=Release "-DJAVA_HOME:PATH=!JDK_CMAKE!" "-DJava_JAVA_EXECUTABLE:FILEPATH=!JAVA_EXE_CMAKE!" "-DJava_JAVAC_EXECUTABLE:FILEPATH=!JAVAC_EXE_CMAKE!"
    )
)

if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed.
    echo         Required native packages: OpenGL, GLFW/glfw3, FreeType, JNI headers.
    echo         If GLFW or FreeType are missing, run deps_windows_vcpkg.bat or install them for your compiler.
    exit /b 1
)


echo.
echo ============================================================
echo Building native C++ targets
echo ============================================================
cmake --build "%NATIVE_BUILD%" --config Release
if errorlevel 1 (
    echo.
    echo [ERROR] Native build failed.
    echo         Check whether your compiler, JDK architecture, GLFW and FreeType architecture match.
    exit /b 1
)

set "MOTOR_DLL="
set "NATIVE_BIN="

rem Prefer real output folders. CMake/NMake may create helper files under
rem CMakeFiles\ShowIncludes; these are not valid native library folders.
for %%F in ("%NATIVE_BUILD%\motor.dll" "%NATIVE_BUILD%\Release\motor.dll" "%NATIVE_BUILD%\Debug\motor.dll" "%NATIVE_BUILD%\RelWithDebInfo\motor.dll" "%NATIVE_BUILD%\MinSizeRel\motor.dll") do (
    if not defined MOTOR_DLL if exist "%%~fF" set "MOTOR_DLL=%%~fF"
)
if not defined MOTOR_DLL (
    for /f "delims=" %%F in ('dir /b /s "%NATIVE_BUILD%\motor.dll" 2^>nul ^| findstr /i /v "\\CMakeFiles\\"') do (
        if not defined MOTOR_DLL set "MOTOR_DLL=%%F"
    )
)
if not defined MOTOR_DLL (
    echo [ERROR] motor.dll was not produced by the build.
    echo         Search folder: "%NATIVE_BUILD%"
    exit /b 1
)
for %%I in ("!MOTOR_DLL!") do set "NATIVE_BIN=%%~dpI"
if "!NATIVE_BIN:~-1!"=="\" set "NATIVE_BIN=!NATIVE_BIN:~0,-1!"
> "%NATIVE_BUILD%\native_bin_path.txt" echo !NATIVE_BIN!
if defined VCPKG_BIN > "!NATIVE_BUILD!\vcpkg_bin_path.txt" echo !VCPKG_BIN!
if defined TOOLCHAIN_BIN > "!NATIVE_BUILD!\toolchain_bin_path.txt" echo !TOOLCHAIN_BIN!
> "%MARKER%" echo %NATIVE_BUILD%

echo.
echo ============================================================
echo Compiling Java sources
echo ============================================================
if not exist "%JAVA_BUILD%" mkdir "%JAVA_BUILD%"
set "JAVA_SOURCES=%JAVA_BUILD%\sources.txt"
if exist "!JAVA_SOURCES!" del /q "!JAVA_SOURCES!"
for /r "%JAVA_DIR%\src" %%F in (*.java) do (
    set "SRC_FILE=%%F"
    rem javac @argfiles interpret backslashes as escapes. Convert to forward slashes.
    set "SRC_FILE=!SRC_FILE:\=/!"
    >> "!JAVA_SOURCES!" echo "!SRC_FILE!"
)
"%JAVAC_EXE%" -encoding UTF-8 -d "%JAVA_BUILD%" @"!JAVA_SOURCES!"
if errorlevel 1 (
    echo.
    echo [ERROR] Java compilation failed.
    echo         If a path looks like C:Users...frameshift..., replace the .bat files with v8 or newer.
    exit /b 1
)


echo.
echo ============================================================
echo Build finished
echo ============================================================
echo [OK] Generator: !GENERATOR_NAME!
echo [OK] Native build folder: "%NATIVE_BUILD%"
echo [OK] Native DLL: "%MOTOR_DLL%"
echo [OK] Java classes: "%JAVA_BUILD%"
echo.
echo To run the application, use: run_windows.bat
exit /b 0
