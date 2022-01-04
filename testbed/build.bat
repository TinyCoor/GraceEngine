REM Build Script for testbed
@echo off
SetLocal EnableDelayedExpansion

REM Get a list of all .c files
SET cFileNames =
FOR /R %%f in (*.c) do (
    set cFileNames=!cFileNames! %%f
)

REM echo "Files:" %cFileNames%

SET assembly=testbed
SET compilerFlags=-g
REM -Wall -Werror
SET includeFlags=-Isrc -I../engine/src/
set linkerFlags=-L../bin/ -lengine.lib
SET defines=-D_DEBUG -DKIMPORT 

echo "Building %assembly%..."
clang %cFileNames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%