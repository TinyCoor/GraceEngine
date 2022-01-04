REM Build Script for Engine
@echo off
SetLocal EnableDelayedExpansion

REM Get a list of all .c files
SET cFileNames =
FOR /R %%f in (*.c) do (
    SET cFileNames=!cFileNames! %%f
)

REM echo "Files:" %cFileNames%

SET assembly=engine
SET compilerFlags=-g -shared -Wvarargs -Wall -Werror 
REM -Wall -Werror
SET includeFlags=-Isrc -I%VULKAN_SDK%/Include
set linkerFlags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
SET defines=-D_DEBUG -DKEXPORT -D_CRT_SECURE_NO_WARNINGS

echo "Building %assembly%..."
clang %cFileNames% %compilerFlags% -o ../bin/%assembly%.dll %defines% %includeFlags% %linkerFlags%