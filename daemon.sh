#!/bin/bash

broker="test.mosquitto.org"
player1Move = "TicTacToe/kco_p1Move"
gameStarted=0

while true
do
if(($gameStarted == 0)); then
    echo "ESP32 is starting game"
fi
while(($gameStarted == 0))
do
    msg=$(mosquitto_sub -h $broker -t $player1Move)
    if ((msg == 1)); then
        gameStarted=1
        open -a Terminal ~/ # Input path script.sh location on my machine
    fi
done

if(($gameStarted == 1)); then
    gameStarted=$(mosquitto_sub -h $broker -t $player1Move)
fi

done