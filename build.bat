@echo off
g++ hookRun.cpp -shared -o hookRun.dll
g++ KeyboardScratchFix.cpp hookRun.dll -o KeyboardScratchFix.exe -lwinmm
