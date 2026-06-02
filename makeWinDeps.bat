@echo off
setlocal enabledelayedexpansion

:: Usage: makeWinDeps.bat
:: Builds QuaZip against Qt6 and installs into DEPS\

set "QT=C:\Qt6\6.10.2\msvc2022_64"
set "PREFIX=%CD%\DEPS"

:: ---- Clone QuaZip if not present ----

if not exist quazip (
    echo.
    echo --- Cloning QuaZIP repository ---
    echo.
    git clone --branch v1.4 --depth 1 https://github.com/stachenov/quazip.git
    if errorlevel 1 (
        echo.
        echo FAIL: git clone failed.
        echo.
        exit /b 1
    )
)

:: ---- Create install prefix ----

if not exist "%PREFIX%" mkdir "%PREFIX%"

:: ---- Normalize paths to forward slashes for CMake ----

set "QT_FWD=%QT:\=/%"
set "PREFIX_FWD=%PREFIX:\=/%"
set "CONAN_FWD=%CONAN_BUILD_DIR:\=/%"

:: ---- Configure ----

echo.
echo Configuring QuaZip...
echo.

cmake -B quazip/build -S quazip ^
    -DCMAKE_INSTALL_PREFIX="%PREFIX_FWD%" ^
    -DQUAZIP_QT_MAJOR_VERSION=6 ^
    -DCMAKE_PREFIX_PATH="%QT_FWD%;%CONAN_FWD%" ^
    -DCMAKE_MODULE_PATH="%CONAN_FWD%" ^
    -DBUILD_SHARED_LIBS=ON ^
-DCMAKE_BUILD_TYPE=Release

if errorlevel 1 (
    echo.
    echo FAIL: CMake configure failed.
    echo.
    exit /b 1
)

:: ---- Build ----

echo.
echo Building QuaZip...
echo.

cmake --build quazip/build --config Release
if errorlevel 1 (
    echo.
    echo FAIL: CMake build failed.
    echo.
    exit /b 1
)

:: ---- Install ----

echo.
echo Installing QuaZip to %PREFIX%...
echo.

cmake --install quazip/build --config Release
if errorlevel 1 (
    echo.
    echo FAIL: CMake install failed.
    echo.
    exit /b 1
)

echo.
echo Build complete!
echo.
