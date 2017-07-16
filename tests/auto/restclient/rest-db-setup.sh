#!/bin/bash
set -e

json-server --port 3000 --host 127.0.0.1 --no-gzip --watch "$(dirname "$0")/test-rest-db.json" &
