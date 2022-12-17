//Katherine Co
//CS2600
//Final: Tic-Tac-Toe
#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

char ADDRESS [] = "tcp://test.mosquitto.org:1883";
char CLIENTID [] = "test.mosquitto.org";
const char TOPIC1 [] = "kco_p1Move";
const char TOPIC2[]  = "kco_p2Move";
int QOS = 1;
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

char board[3][3];
int stop = 1;
char player1 = 'X';
char player2 = 'O';
char p1Moves [9];
char p2Moves [9];

void boardDisplay();
void player2Move();
int spacesLeft();
void resetBoard();
char checkWin();
void sayWinner(char);
int validInput(int, int);

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char *payloadptr;
    printf("Message arrived\n");
    printf("Topic: %s\n", topicName);
    printf("Message: ");
    payloadptr = message->payload;

    for(i=0; i<message->payloadlen; i++)
    {
        p1Moves[i] = *payloadptr;
        putchar(*payloadptr++);
    }    

    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
    char userAgain = ' ';
    char winner = ' ';
    int row;
    int column;
    int size = 0;
    MQTTClient client;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    int ch;

    printf("Welcome to the game of Tic-Tac-Toe!\n");

    printf("You are Player 2\n");
    printf("\nPlayer 1 is 'X' and Player 2 is 'O'\n\n");

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    //Subscribing
    
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n", TOPIC1, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC1, QOS);

/*
    do
    {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');
*/
    board[0][0] = p1Moves[0];
    board[0][1] = p1Moves[1];
    board[0][2] = p1Moves[2];
    board[1][0] = p1Moves[3];
    board[1][1] = p1Moves[4];
    board[1][2] = p1Moves[5];
    board[2][0] = p1Moves[6];
    board[2][1] = p1Moves[7];
    board[2][2] = p1Moves[8];

    boardDisplay();
    fflush(stdin);
    printf("Player 2 please enter your move with range of 0-2 for row and 0-2 for column.\n");
    printf("Row: ");
    scanf("%d", &row);
    fflush(stdin);
    printf("Column: ");
    scanf("%d", &column);
    fflush(stdin);
    do
    {
        printf("Invalid move! Please try again.\n");
        printf("Row: ");
        scanf("%d", &row);
        fflush(stdin);
        printf("Column: ");
        scanf("%d", &column);
        fflush(stdin);

    } while (validInput(row, column) == 1);

    board[row][column] = player2;

    p2Moves[1] = board[0][1];
    p2Moves[2] = board[0][2];
    p2Moves[3] = board[1][0];
    p2Moves[4] = board[1][1];
    p2Moves[5] = board[1][2];
    p2Moves[6] = board[2][0];
    p2Moves[7] = board[2][1];
    p2Moves[8] = board[2][2];

    for (int i=0; i<9; i++)
    {
        printf("%c", p2Moves[i]);
    }

    boardDisplay();

    pubmsg.payload = p2Moves;
    pubmsg.payloadlen = strlen(p2Moves);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;
    MQTTClient_publishMessage(client, TOPIC2, &pubmsg, &token);
    printf("Waiting for publication of %s\n"
            "on topic %s for client with ClientID: %s\n", p2Moves, TOPIC2, CLIENTID);
    while(deliveredtoken != token);

    // Continue to play back and forth until there is a win
    winner = checkWin();
    if (winner != '-')
    {
        printf("\nHere are the results\n");
        boardDisplay();
        // Displays the winner and if no winner, will display a tie
        sayWinner(winner);


        printf("This is the end of the game. Thank you for playing!");
    //resetBoard();
    //boardDisplay();
    }

    MQTTClient_disconnect(client, 10000);     //add at end of loop
    MQTTClient_destroy(&client);      //add at end of loop
    return rc;
}

// An empty layout of the tic-tac-toe board
void boardDisplay()
{
    printf("+-----------+\n");
    printf("| %c | %c | %c |\n", board[0][0], board[0][1], board[0][2]);
    printf("+-----------+\n");
    printf("| %c | %c | %c |\n", board[1][0], board[1][1], board[1][2]);
    printf("+-----------+\n");
    printf("| %c | %c | %c |\n", board[2][0], board[2][1], board[2][2]);
    printf("+-----------+\n");   
}

// Resets the board so that the spaces are ' '
void resetBoard()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = '-';
        }
    }
}

int validInput(int r, int c)
{
    if (r != 0 && r != 1 && r != 2 || c != 0 && c != 1 && c != 2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Asks player 2 to make their move and input their move on the board
void publishP2()
{    
    int row;
    int column;
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    
    p2Moves[0] = board[0][0];
    p2Moves[1] = board[0][1];
    p2Moves[2] = board[0][2];
    p2Moves[3] = board[1][0];
    p2Moves[4] = board[1][1];
    p2Moves[5] = board[1][2];
    p2Moves[6] = board[2][0];
    p2Moves[7] = board[2][1];
    p2Moves[8] = board[2][2];

    pubmsg.payload = p2Moves;
    pubmsg.payloadlen = strlen(p2Moves);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;
    MQTTClient_publishMessage(client, TOPIC2, &pubmsg, &token);
    printf("Waiting for publication of %s\n"
            "on topic %s for client with ClientID: %s\n", p2Moves, TOPIC2, CLIENTID);
    while(deliveredtoken != token);
    //return rc;
    MQTTClient_disconnect(client, 10000);     //add at end of loop
    MQTTClient_destroy(&client);      //add at end of loop
}

// This is to check for a tie for person vs. computer
int spacesLeft()
{
    int spacesLeft = 9;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(board[i][j] != '-')
            {
                spacesLeft--;
            }
        }
    }
    
    return spacesLeft;
}

//Checks to see if there is a winner
char checkWin()
{
    // Checks for row win
    for(int i = 0; i < 3; i++)
    {
        if(board[i][i] == '-')
        {
            continue;
        }
        if(i == 0)
        {
            if(board[i][i] == board[i][i + 1] && board[i][i] == board[i][i + 2] || 
            board[i][i] == board[i + 1][i] && board[i][i] == board[i + 2][i])
            {
                return board[i][i];
            }
        }
        if(i == 1)
        {
            if(board[i][i - 1] == board[i][i] && board[i][i] == board[i][i + 1] || 
            board[i - 1][i] == board[i][i] && board[i][i] == board[i + 1][i] ||
            board[i - 1][i - 1] == board[i][i] && board[i][i] == board[i + 1][i + 1]||
            board[i - 1][i + 1] == board[i][i] && board[i][i] == board[i + 1][i - 1])
            {
                return board[i][i];
            }
        }
        if(i == 2)
        {
            if(board[i][i] == board[i][i + 1] && board[i][i] == board[i][i + 2] || 
            board[i][i] == board[i - 1][i] && board[i][i] == board[i - 2][i])
            {
                return board[i][i];
            }
        }
    }
    return '-';
}

// Winning message for person vs. person
void sayWinner(char winner)
{
    if (winner == player1)
    {
        printf("Player 1 wins!");
    }
    else if (winner == player2)
    {
        printf("Player 2 wins!");
    }
    else
    {
        printf("You ended the game with a tie!");
    }
}