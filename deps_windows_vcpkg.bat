@echo off
setlocal EnableExtensions EnableDelayedExpansion
chcp 65001 >nul

rem Optional dependency helper for users who use vcpkg.
rem This installs GLFW and FreeType for the compiler family you are likely using.

set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"

if not defined VCPKG_ROOT (
    if exist "%ROOT_DIR%\vcpkg\vcpkg.exe" set "VCPKG_ROOT=%ROOT_DIR%\vcpkg"
    if not defined VCPKG_ROOT if exist "%USERPROFILE%\vcpkg\vcpkg.exe" set "VCPKG_ROOT=%USERPROFILE%\vcpkg"
    if not defined VCPKG_ROOT if exist "C:\vcpkg\vcpkg.exe" set "VCPKG_ROOT=C:\vcpkg"
)

if not defined VCPKG_ROOT (
    echo [ERROR] vcpkg was not found.
    echo.
    echo Install vcpkg or set VCPKG_ROOT manually. Example:
    echo   git clone https://github.com/microsoft/vcpkg C:\vcpkg
    echo   C:\vcpkg\bootstrap-vcpkg.bat
    echo   set VCPKG_ROOT=C:\vcpkg
    echo   deps_windows_vcpkg.bat
    exit /b 1
)

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [ERROR] vcpkg.exe was not found at: "%VCPKG_ROOT%\vcpkg.exe"
    echo         Run bootstrap-vcpkg.bat in that folder first.
    exit /b 1
)

if defined VCPKG_TARGET_TRIPLET (
    set "TRIPLET=%VCPKG_TARGET_TRIPLET%"
) else (
    where gcc >nul 2>nul
    if not errorlevel 1 (
        set "TRIPLET=x64-mingw-dynamic"
    ) else (
        set "TRIPLET=x64-windows"
    )
)

echo.
echo ============================================================
echo Installing native dependencies with vcpkg
echo ============================================================
echo [INFO] VCPKG_ROOT: %VCPKG_ROOT%
echo [INFO] Triplet: %TRIPLET%
echo.

"%VCPKG_ROOT%\vcpkg.exe" install glfw3:%TRIPLET% freetype:%TRIPLET%
if errorlevel 1 (
    echo.
    echo [ERROR] vcpkg dependency installation failed.
    exit /b 1
)

echo.
echo [OK] Dependencies installed.
echo Run now:
echo   set VCPKG_ROOT=%VCPKG_ROOT%
echo   set VCPKG_TARGET_TRIPLET=%TRIPLET%
echo   run_windows.bat
exit /b 0
