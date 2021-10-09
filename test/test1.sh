#!/bin/bash
trap terminate SIGINT
terminate(){
    pkill -SIGINT -P $$
    exit
}

./client -W data/file1.c > logs/client1.txt 2>&1 &
echo "Started client 1"
./client -W data/file2.h,data/file3.c > logs/client2.txt 2>&1 &
echo "Started client 2"
./client -W data/file4.h,data/file5.c > logs/client3.txt 2>&1 &
echo "Started client 3"
./client -W data/file6.c > logs/client4.txt 2>&1 &
echo "Started client 4"



