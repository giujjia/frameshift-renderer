@echo off
setlocal EnableExtensions EnableDelayedExpansion
chcp 65001 >nul

rem Build if needed, then run the Java Swing application.
set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"
set "JAVA_DIR=%ROOT_DIR%\java"
set "JAVA_BUILD=%JAVA_DIR%\build"
set "MARKER=%ROOT_DIR%\.frameshift_native_build_dir.txt"

call "%ROOT_DIR%\build_windows.bat"
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed. The application was not started.
    pause
    exit /b 1
)

set "NATIVE_BUILD="
if exist "%MARKER%" set /p NATIVE_BUILD=<"%MARKER%"
if not defined NATIVE_BUILD (
    echo [ERROR] Could not determine native build folder.
    pause
    exit /b 1
)

set "NATIVE_BIN="
if exist "%NATIVE_BUILD%\native_bin_path.txt" set /p NATIVE_BIN=<"%NATIVE_BUILD%\native_bin_path.txt"
if not defined NATIVE_BIN (
    for /f "delims=" %%F in ('dir /b /s "%NATIVE_BUILD%\motor.dll" 2^>nul ^| findstr /i /v "\\CMakeFiles\\"') do if not defined NATIVE_BIN for %%I in ("%%F") do set "NATIVE_BIN=%%~dpI"
)
if defined NATIVE_BIN if "!NATIVE_BIN:~-1!"=="\" set "NATIVE_BIN=!NATIVE_BIN:~0,-1!"

if not defined NATIVE_BIN (
    echo [ERROR] motor.dll was not found under: "%NATIVE_BUILD%"
    pause
    exit /b 1
)

if exist "%NATIVE_BUILD%\vcpkg_bin_path.txt" (
    set /p VCPKG_BIN=<"%NATIVE_BUILD%\vcpkg_bin_path.txt"
    set "PATH=!VCPKG_BIN!;!PATH!"
)
if exist "%NATIVE_BUILD%\toolchain_bin_path.txt" (
    set /p TOOLCHAIN_BIN=<"%NATIVE_BUILD%\toolchain_bin_path.txt"
    set "PATH=!TOOLCHAIN_BIN!;!PATH!"
)
set "PATH=!NATIVE_BIN!;!PATH!"

rem Use the JDK found by build_windows.bat if JAVA_HOME was set externally; otherwise rely on PATH.
if defined JAVA_HOME if exist "!JAVA_HOME!\bin\java.exe" set "PATH=!JAVA_HOME!\bin;!PATH!"

if not exist "%JAVA_BUILD%\main\MainWindow.class" (
    echo [ERROR] MainWindow.class was not found. Build may have failed.
    pause
    exit /b 1
)

echo.
echo ============================================================
echo Running ShiftGL - Frameshift Mutation Visualizer
echo ============================================================
echo [INFO] Native library path: %NATIVE_BIN%
echo [INFO] Working directory: %JAVA_DIR%
echo.

pushd "%JAVA_DIR%"
java "-Djava.library.path=!NATIVE_BIN!" -cp "%JAVA_BUILD%" main.MainWindow
set "APP_EXIT=%ERRORLEVEL%"
popd

if not "%APP_EXIT%"=="0" (
    echo.
    echo [ERROR] Application exited with code %APP_EXIT%.
    echo         If this says UnsatisfiedLinkError, check motor.dll and DLL dependencies.
    echo         If mutation data is missing, put the files in java\data.
    pause
    exit /b %APP_EXIT%
)
exit /b 0
