#!/bin/bash

cd "$(dirname "$(readlink -f "$0")")"

export NODE_ENV=production
export PORT=80

cd server

nohup node server.js > ../server.log &
