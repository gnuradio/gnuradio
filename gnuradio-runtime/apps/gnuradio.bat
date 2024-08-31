@echo off

pushd "%~dp0%"
set PATH=%CD%;%CD%\..\Python311\Lib\site-packages\gi\gtk\bin;%PATH%
..\Python311\python.exe -i setup_gr.py
pause
popd