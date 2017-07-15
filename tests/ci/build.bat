@echo on

echo %CD%
.\tests\auto\restclient\rest-db-setup.bat
.\qtmodules-travis\ci\win\build.bat

taskkill /F /IM nodejs.exe
