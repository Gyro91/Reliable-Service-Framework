#!/bin/bash

rm -rf log/*

sleep 1

./RSF_start_broker &
./RSF_deployment_unit -s 0 -n 3 &

sleep 1

kill -9 $(pgrep RSF_server | head -n 1)
kill -9 $(pgrep RSF_server | tail -n 1)
./RSF_client -s 0

sleep 8

kill -9 $(pgrep RSF)