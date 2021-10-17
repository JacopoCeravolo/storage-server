#!/bin/bash
trap terminate SIGINT
terminate(){
    pkill -SIGINT -P $$
    exit
}

./client -W data/file1.c,data/file2.h,data/file3.c,data/file4.h,data/file5.c,data/file6.c > logs/client1.txt 2>&1 
echo "Started client 1"

./client -W data/file1.c,data/file2.h -r data/file4.h > logs/client2.txt 2>&1 &
echo "Started client 2"

./client -W data/file2.c,data/file3.c -r data/file4.h,data/file5.c  > logs/client3.txt 2>&1 & 
echo "Started client 3"

./client -W data/file4.h,data/file5.h -r data/file5.c,data/file6.c  > logs/client4.txt 2>&1 &
echo "Started client 4"

sleep 3 

./client -R 5 > logs/client2.txt 2>&1
echo "Started reader client"


