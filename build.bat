@echo off
setlocal enabledelayedexpansion

set SOLUTION_PATH=E:\CODE\Super-Language\Compiler\Super\Super.sln
set MSBUILD_PATH=D:\VSIDE\2026\MSBuild\Current\Bin\MSBuild.exe

echo Starting build...
echo Solution path: %SOLUTION_PATH%
echo MSBuild path: %MSBUILD_PATH%

if not exist "%SOLUTION_PATH%" (
    echo Error: Solution file not found: %SOLUTION_PATH%
    exit /b 1
)

if not exist "%MSBUILD_PATH%" (
    echo Error: MSBuild not found: %MSBUILD_PATH%
    exit /b 1
)

"%MSBUILD_PATH%" "%SOLUTION_PATH%" /p:Configuration=Debug /p:Platform=x64 /t:Build /m:16

if %ERRORLEVEL% EQU 0 (
    echo Build succeeded!
) else (
    echo Build failed, exit code: %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

endlocal
