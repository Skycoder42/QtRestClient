#!/bin/bash
set -e

stat /usr/bin/json-server
echo $PATH
/usr/bin/json-server --port 3000 --host 127.0.0.1 --no-gzip --watch test-rest-db.json &
