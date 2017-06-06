#!/bin/bash
/bin/kill -9 $(ps -e | grep server | head -n1 | awk '{print $1}')
/bin/kill -9 $(ps -e | grep server | tail -n1 | awk '{print $1}')
./client -s 0

