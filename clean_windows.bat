@echo off
setlocal EnableExtensions
chcp 65001 >nul

set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"

echo.
echo ============================================================
echo Cleaning Windows build artifacts
echo ============================================================

if exist "%ROOT_DIR%\java\build" (
    echo Removing java\build
    rmdir /s /q "%ROOT_DIR%\java\build"
)
if exist "%ROOT_DIR%\native\build-windows-msvc-nmake" rmdir /s /q "%ROOT_DIR%\native\build-windows-msvc-nmake"
if exist "%ROOT_DIR%\native\build-windows-msvc-ninja" rmdir /s /q "%ROOT_DIR%\native\build-windows-msvc-ninja"
if exist "%ROOT_DIR%\native\build-windows-mingw" rmdir /s /q "%ROOT_DIR%\native\build-windows-mingw"
if exist "%ROOT_DIR%\native\build-windows-mingw-ninja" rmdir /s /q "%ROOT_DIR%\native\build-windows-mingw-ninja"
if exist "%ROOT_DIR%\native\build-windows-manual" rmdir /s /q "%ROOT_DIR%\native\build-windows-manual"
if exist "%ROOT_DIR%\native\build-windows-cmake-wrapper" rmdir /s /q "%ROOT_DIR%\native\build-windows-cmake-wrapper"
if exist "%ROOT_DIR%\.frameshift_native_build_dir.txt" del /q "%ROOT_DIR%\.frameshift_native_build_dir.txt"

echo [OK] Clean finished.
exit /b 0
