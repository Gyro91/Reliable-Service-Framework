#!/bin/bash

rm -rf log/*

sleep 1

./RSF_start_broker &
./RSF_deployment_unit -s 0 -n 3 &
sleep 1
./RSF_client -s 1 &

sleep 3

kill -9 $(pgrep RSF)


