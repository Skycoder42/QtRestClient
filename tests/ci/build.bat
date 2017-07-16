@echo on

call .\tests\auto\restclient\rest-db-setup.bat || (
	taskkill /F /IM node.exe
	exit /B 1
)

call .\qtmodules-travis\ci\win\build.bat || (
	taskkill /F /IM node.exe
	exit /B 1
)

taskkill /F /IM node.exe
