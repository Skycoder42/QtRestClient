@echo on

echo %CD%
call .\tests\auto\restclient\rest-db-setup.bat
call .\qtmodules-travis\ci\win\build.bat

taskkill /F /IM node.exe
