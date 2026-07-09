@echo off
setlocal EnableExtensions EnableDelayedExpansion
chcp 65001 >nul

set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"
set "MARKER=%ROOT_DIR%\.frameshift_native_build_dir.txt"

call "%ROOT_DIR%\build_windows.bat"
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed. Native test was not started.
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

set "TEST_EXE="
for %%F in ("%NATIVE_BUILD%\ShiftGL_Test.exe" "%NATIVE_BUILD%\Release\ShiftGL_Test.exe" "%NATIVE_BUILD%\Debug\ShiftGL_Test.exe" "%NATIVE_BUILD%\RelWithDebInfo\ShiftGL_Test.exe" "%NATIVE_BUILD%\MinSizeRel\ShiftGL_Test.exe") do if not defined TEST_EXE if exist "%%~fF" set "TEST_EXE=%%~fF"
if not defined TEST_EXE for /f "delims=" %%F in ('dir /b /s "%NATIVE_BUILD%\ShiftGL_Test.exe" 2^>nul ^| findstr /i /v "\\CMakeFiles\\"') do if not defined TEST_EXE set "TEST_EXE=%%F"
if not defined TEST_EXE (
    echo [ERROR] ShiftGL_Test.exe was not found under: "%NATIVE_BUILD%"
    pause
    exit /b 1
)

for %%I in ("%TEST_EXE%") do set "TEST_BIN=%%~dpI"
set "PATH=!TEST_BIN!;!PATH!"
if exist "%NATIVE_BUILD%\vcpkg_bin_path.txt" (
    set /p VCPKG_BIN=<"%NATIVE_BUILD%\vcpkg_bin_path.txt"
    set "PATH=!VCPKG_BIN!;!PATH!"
)
if exist "%NATIVE_BUILD%\toolchain_bin_path.txt" (
    set /p TOOLCHAIN_BIN=<"%NATIVE_BUILD%\toolchain_bin_path.txt"
    set "PATH=!TOOLCHAIN_BIN!;!PATH!"
)

echo.
echo ============================================================
echo Running native OpenGL test
echo ============================================================
echo [INFO] Executable: %TEST_EXE%
echo.

pushd "%ROOT_DIR%"
"%TEST_EXE%"
set "TEST_EXIT=%ERRORLEVEL%"
popd

if not "%TEST_EXIT%"=="0" (
    echo.
    echo [ERROR] Native test exited with code %TEST_EXIT%.
    pause
    exit /b %TEST_EXIT%
)
exit /b 0
