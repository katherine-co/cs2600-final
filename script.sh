#!/bin/bash

broker = "test.mosquitto.org"
player1Move = "TicTacToe/kco_p1Move"
player2Move = "TicTacToe/kco_p2Move"
gameStarted = 1
output =- 1
autoplay = 0

cd ~/               # Input path to where TicTacToe.c is located

gcc TicTacToe.c -o a

./TicTacToe 0
mosquitto_sub -h $broker -t $playerMove

while((gameStarted == 1))
do

validMove=-1
unset position1
unset position2

if ((player2Move)); then
    while ((validMove == -1))
    do
        if ((autoplay == 0)); then
            read -p "Enter number for row (0-2): " -t 10 position1
            read -p "Enter number for column (0-2): " -t 10 position2
        fi

        if [[ $position1 -eq 0 && $position2 -eq 0 || autoplay == 1 ]]; then
            if((autoplay == 0)); then
                echo "Moves will be automatically made until the end of the game."
                autoplay=1
            fi
            while((validMove == -1))
            do
                position1 = $(( ( RANDOM % 9 )  + 1 ))
                position2 = $(( ( RANDOM % 9 )  + 1 ))
                ./TicTacToe $position1 $position2
                output=$?
                if(($output == 0 || $output == 1 || $output == 2 || $output == 3)); then
                    validMove=1
                fi
            done
                
        elif (( $position1 < 0  && $position1 > 2 || $position2 < 0 && $position2 > 2 )); then
            echo "Invalid position"
        else
            ./TicTacToe $position1 $position2
            output=$?
            if(($output == 0 || $output == 1 || $output == 2 || $output == 3)); then
                validMove = 1
            fi
        fi
    done
elif ((player1Move)); then
    echo "Player 1's turn"
    while ((validMove == -1))
    do
        position=$(mosquitto_sub -h $broker -t $player2Move)
        if(($position > 0 || $position < 10)); then
            ./TicTacToe $player $position
            output=$?
            if(($output == 0 || $output == 1 || $output == 2 || $output == 3)); then
                validMove = 1
            fi
        fi
    done
fi

mosquitto_pub -h $broker -t $player1Move

if(($output == 1 || $output == 2 || $output == 3)); then
    gameStarted=0
    msg=$(mosquitto_pub -h $broker -t $player1Move -m $gameStarted)
fi

done