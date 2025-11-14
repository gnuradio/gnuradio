@echo off

pushd "%~dp0%"
set PATH=%CD%;%CD%\..\Python@PYTHON_SHORT_VER@\Lib\site-packages\gi\gtk\bin;%PATH%
..\Python@PYTHON_SHORT_VER@\python.exe -i setup_gr.py
pause
popd