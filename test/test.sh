#!/bin/bash
trap terminate SIGINT
terminate(){
    pkill -SIGINT -P $$
    exit
}

./client -r client1.txt,client1.c,client1.txt,client1.c > logs/client1.txt 2>&1 &
echo "Started client 1"
./client -r client2.txt > logs/client2.txt 2>&1 &
echo "Started client 2"
./client -r client3.txt,client3.c,client3.txt,client3.c > logs/client3.txt 2>&1 &
echo "Started client 3"
./client -r client4.txt > logs/client4.txt 2>&1 &
echo "Started client 4"
./client -r client5.txt,client5.c,client5.txt,client5.c > logs/client5.txt 2>&1 &
echo "Started client 5"
./client -r client6.txt > logs/client6.txt 2>&1 &
echo "Started client 6"
./client -r client7.txt,client7.c,client7.txt,client7.c > logs/client7.txt 2>&1 &
echo "Started client 7"
./client -r client8.txt > logs/client8.txt 2>&1 &
echo "Started client 8"
./client -r client9.txt,client9.c,client9.txt,client9.c > logs/client9.txt 2>&1 &
echo "Started client 9"
./client -r client10.txt > logs/client10.txt 2>&1 &
echo "Started client 10"


