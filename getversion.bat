@echo off
git describe --abbrev=0 --tags > %~dp0version.txt 2> nul
if %ERRORLEVEL% NEQ 0 (
	cmd /c "exit /b 0"
	echo|set /p="v1.0.0" > %~dp0version.txt
)
set /p version=<%~dp0version.txt
del %~dp0version.txt
set newver=%version:v=%
echo|set /p="%newver%"

exit /b 0