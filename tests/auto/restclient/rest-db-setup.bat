start /b cmd /c json-server --port 3000 --host 127.0.0.1 --no-gzip --watch "%~dp0\test-rest-db.json" || exit \B 1
