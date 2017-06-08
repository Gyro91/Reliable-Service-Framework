#!/bin/bash

rm -rf log/*

sleep 1

./RSF_start_broker &
./RSF_deployment_unit -s 0 -n 3 &

sleep 1

kill -9 $(pgrep RSF_broker)

sleep 20

kill -9 $(pgrep RSF)