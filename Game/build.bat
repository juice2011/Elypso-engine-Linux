:: Copyright (C) 2024 Lost Empire Entertainment
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md for more information.

@echo off
:: Batch script to build the executable for the game

:: Reusable message types printed to console
set "gexc=[GAME_EXCEPTION]"
set "ginf=[GAME_INFO]"
set "gcln=[GAME_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmexc=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"
set "cpinf=[CPACK_INFO]"
set "cpexc=[CPACK_EXCEPTION]"
set "cpsuc=[CPACK_SUCCESS]"

set "documentsPath=%USERPROFILE%\Documents\Game"
set "outPath=%~dp0out"
set "vsPath=%~dp0.vs"

set "buildPath=%~dp0build"

:menu
cls

echo Game setup
echo.
echo Copyright (C) 2024 Lost Empire Entertainment
echo.
echo This program comes with ABSOLUTELY NO WARRANTY.
echo This is free software, and you are welcome to redistribute it under certain conditions.
echo Read LICENSE.md for more information.

echo.

echo ========================================================

echo.

echo Write the number of your choice to choose the action.
echo.
echo 1. Reconfigure CMake
echo 2. Build Game
echo 3. Exit
echo.
echo 9. Clean Visual Studio (DELETES OUT AND .VS FOLDERS)
echo 0. Clean game (DELETES BUILD AND ENGINE DOCUMENTS FOLDERS)
echo.
set /p choice="Choice: "

:: Process user input
if "%choice%"=="1" goto cmake
if "%choice%"=="2" goto build
if "%choice%"=="3" exit

if "%choice%"=="9" goto cleanvs
if "%choice%"=="0" goto cleang

echo %prexc% Invalid choice! Please enter a valid number.
pause
goto menu

:cmake
:: Change to the script directory
cd /d "%~dp0"
	
:: Clean the build directory before configuration
if exist "%buildPath%" (
	echo %gcln% Deleted folder: build
	rd /s /q "%buildPath%"
)
mkdir "%buildPath%"
cd "%buildPath%"

echo %cminf% Started CMake configuration.

:: Configure the project (Release build)
cmake -DCMAKE_BUILD_TYPE=Release ..

if %errorlevel% neq 0 (
	echo %cmexc% CMake configuration failed.
	pause
	goto menu
) else (
	echo %cmsuc% Cmake configuration succeeded!
)
	
goto build

:build
:: Change to the script directory
cd /d "%~dp0"
	
if not exist "%buildPath%" (
	echo %gexc% Did not find build folder. Running 'Reconfigure CMake'.
	goto cmake
) else (
	cd "%buildPath%"
		
	:: Build the project
	echo %cminf% Started build generation.
	cmake --build . --config Release
	
	if %errorlevel% neq 0 (
		echo %cmexc% Build failed because Game.exe did not get generated properly.
	) else (
		echo %cmsuc% Build succeeded!
	)
)

pause
goto menu

:cleanvs
:: Change to the script directory
cd /d "%~dp0"
	
echo %ginf% Running vs clean...
if not exist "%vsPath%" (
	if not exist "%outPath%" (
		echo %gcln% There are no Visual Studio folders to remove.
		pause
		goto menu
	)
)

if exist "%vsPath%" (
	echo %gcln% Deleted folder: .vs
	rd /s /q "%vsPath%"
)
echo "%outPath%"
if exist "%outPath%" (
	echo %gcln% Deleted folder: out
	rd /s /q "%outPath%"
)
	
pause
goto menu

:cleang
:: Change to the script directory
cd /d "%~dp0"
	
if not exist "%buildPath%" (
	if not exist "%documentsPath%" (
		echo %gcln% There are no game folders to remove.
		pause
		goto menu
	)
)

if exist "%buildPath%" (
	echo %encln% Deleted folder: build
	rd /s /q "%buildPath%"
)
if exist "%documentsPath%" (
	echo %gcln% Deleted folder: Documents/Game
	rd /s /q "%documentsPath%"
)
	
pause
goto menu