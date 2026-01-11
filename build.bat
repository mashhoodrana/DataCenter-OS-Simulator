@echo off
echo ================================================
echo  Data Center Simulator - Qt GUI Build Script
echo ================================================
echo.

REM Set Qt path - ADJUST THIS TO YOUR INSTALLATION
set QT_PATH=D:\qt\6.10.1\mingw_64
set MINGW_PATH=D:\qt\Tools\mingw1310_64
set PATH=%QT_PATH%\bin;%MINGW_PATH%\bin;%PATH%

echo Qt Path: %QT_PATH%
echo MinGW Path: %MINGW_PATH%
echo.

REM Verify tools are available
echo Verifying tools...
qmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: qmake not found! Check Qt installation.
    pause
    exit /b 1
)

cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: cmake not found! Check CMake installation.
    pause
    exit /b 1
)

echo All tools found successfully!
echo.

REM Create build directory
if not exist build mkdir build
cd build

echo [1/3] Configuring CMake...
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=%QT_PATH% -DCMAKE_BUILD_TYPE=Debug ..
if %errorlevel% neq 0 (
    echo.
    echo ERROR: CMake configuration failed!
    echo.
    echo Troubleshooting tips:
    echo 1. Verify Qt path: %QT_PATH%
    echo 2. Check if all Qt components are installed
    echo 3. Make sure MinGW is in PATH
    echo.
    cd ..
    pause
    exit /b 1
)

echo.
echo [2/3] Building project...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Build failed!
    echo Check the error messages above.
    echo.
    cd ..
    pause
    exit /b 1
)

echo.
echo ================================================
echo  BUILD SUCCESSFUL!
echo ================================================
echo.
echo Executables created:
echo   - GUI version:     build\bin\datacenter_gui.exe
echo   - Console version: build\bin\datacenter_console.exe
echo.
echo To run the GUI application:
echo   cd build\bin
echo   datacenter_gui.exe
echo.
echo Or double-click: build\bin\datacenter_gui.exe
echo.

cd ..
pause