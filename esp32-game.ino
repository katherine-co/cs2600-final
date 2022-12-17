/**********************************************************************
  Name: Katherine Co
  Assignment: Tic-Tac-Toe
  Date: 12/13/22 
**********************************************************************/
#include <Keypad.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>

char ssid[] = "KateiPhone";   //Input own SSID
char pass[] = "2r48qw5feaotn";  //Input password 

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
int port = 1883;
const char topic1[] = "kco_p1Move";
const char topic2[]  = "kco_p2Move";

//set interval for sending messages (milliseconds)
const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

int latchPin = 2;          // Pin connected to ST_CP of 74HC595（Pin12）
int clockPin = 4;          // Pin connected to SH_CP of 74HC595（Pin11）
int dataPin = 15;          // Pin connected to DS of 74HC595（Pin14）
char player1 = 'X';
char player2 = 'O';
char winner = ' ';
char p1Move[9]; 

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[4] = {14, 27, 26, 25}; // connect to the row pinouts of the keypad
byte colPins[4] = {13, 21, 22, 23};   // connect to the column pinouts of the keypad

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);
int lcdLights[8]= {0, 0, 0, 0, 0, 0, 0, 0};

// Define the data of numbers and letters, and save them in flash area
char board[3][3] = {
  {'-', '-', '-'},
  {'-', '-', '-'},
  {'-', '-', '-'}
};

void setup()
{
  // set pins to output
  Serial.begin(115200); // Initialize the serial port and set the baud rate to 115200
  while (!Serial)
  {
    ; //wait for serial port to connect. Needed for native USB port only
  }
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();


  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(topic2);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(topic2);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Topic: ");
  Serial.println(topic2);

  Serial.println();
}


void loop()
{
  // call poll() regularly to allow the library to send MQTT keep alive which
  mqttClient.poll();

  // Define a one-byte variable (8 bits) which is used to represent the selected state of 8 column.
  char keyPressed = myKeypad.getKey();
  int cols;

  if (keyPressed)
  {  
      Serial.println(keyPressed);
  }

  if (keyPressed == '1')
  {
    if (board[0][0] != '-')
    {
      Serial.println("Spot is already taken!");
    }
    else
    {
      board[0][0] = player1;
      lcdLights[0] += 128;
      lcdLights[1] += 64;
      sendToP2();
    }
  }

  if (keyPressed == '2')
  {
    if (board[0][1] != '-')
    {
      Serial.println("Spot is already taken!");
    }
    else
    {
      board[0][1] = player1;
      lcdLights[3] += 128;
      lcdLights[4] += 64;
      sendToP2();
    }
  }

  if (keyPressed == '3')
  {
    if (board[0][2] != '-')
    {
      Serial.println("Spot is already taken!");
    }
    else
    {
      board[0][2] = player1;
      lcdLights[6] += 128;
      lcdLights[7] += 64;
      sendToP2();
    }
  }
  
  if (keyPressed == '4')
  {
    if (board[1][0] != '-')
    {
      Serial.println("Spot is already taken!");
    }
    else
    {
      board[1][0] = player1;
      lcdLights[0] += 16;
      lcdLights[1] += 8;
      sendToP2();
    }
  }

  if (keyPressed == '5')
  {
    if (board[1][1] != '-')
    {
      Serial.println("Spot is already taken!");
    }
    else
    {
      board[1][1] = player1;
      lcdLights[3] += 16;
      lcdLights[4] += 8;
      sendToP2();
    }
  }

  if (keyPressed == '6')
  {
    if (board[1][2] != '-')
    {
      Serial.println("Spot is already taken!");
    }
    else
    {
      board[1][2] = player1;
      lcdLights[6] += 16;
      lcdLights[7] += 8;
      sendToP2();
    }
  }

  if (keyPressed == '7')
  {
    if (board[2][0] != '-')
    {
      Serial.println("Spot is already taken!");
    }
    else
    {
      board[2][0] = player1;
      lcdLights[0] += 2;
      lcdLights[1] += 1;
      sendToP2();
    }
  }

  if (keyPressed == '8')
  {
    if (board[2][1] != '-')
    {
      Serial.println("Spot is already taken!");
    }
    else
    {
      board[2][1] = player1;
      lcdLights[3] += 2;
      lcdLights[4] += 1;
      sendToP2();
    }
  }

  if (keyPressed == '9')
  {
    if (board[2][2] != '-')
    {
      Serial.println("Spot is already taken!");
    }
    else
    {
      board[2][2] = player1;
      lcdLights[6] += 2;
      lcdLights[7] += 1;
      sendToP2();
    }
  }

  if (keyPressed == '0')
  {
    Serial.println("This game will restart!");
    resetBoard();
    resetLights();
    for (int i = 0; i < 9; i++)
    {
      p1Move[i] = '-'; 
    }
    sendToP2();
  }
  
  //updateLights();
  cols = 0x01;
  for (int i = 0; i < 8; i++)
  {
    matrixRowsVal(lcdLights[i]);  // display the data in this column
    matrixColsVal(~cols);          // select this column
    delay(1);                     // display them for a period of time
    matrixRowsVal(0x00);          // clear the data of this column
    cols <<= 1;                   // shift"cols" 1 bit left to select the next column
  }
}

void sendToP2()
{
  unsigned long currentMillis = millis();
  
  p1Move[0] = board[0][0];
  p1Move[1] = board[0][1];
  p1Move[2] = board[0][2];
  p1Move[3] = board[1][0];
  p1Move[4] = board[1][1];
  p1Move[5] = board[1][2];
  p1Move[6] = board[2][0];
  p1Move[7] = board[2][1];
  p1Move[8] = board[2][2];


  if (currentMillis - previousMillis >= interval)
  {
    // save the last time a message was sent
    previousMillis = currentMillis;

    char random [] = "X-OO--X-O";

    //record random value from A0, A1 and A2

    Serial.print("Sending message to topic: ");
    Serial.println(topic1);
    Serial.println(p1Move);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic1);
    mqttClient.print(p1Move);
    mqttClient.endMessage();

    Serial.println();
  }
}

void updateLights()
{ 
  if (board[0][0] == 'X')
  {
    lcdLights[0] += 128;
    lcdLights[1] += 64;
  }
  if (board[0][0] == 'O')
  {
    lcdLights[0] += 192;
    lcdLights[1] += 192;
  }
  if (board[0][1] == 'X')
  {
    lcdLights[3] += 128;
    lcdLights[4] += 64;
  }
  if (board[0][1] == 'O')
  {
    lcdLights[3] += 192;
    lcdLights[4] += 192;
  }
  if (board[0][2] == 'X')
  {
    lcdLights[6] += 128;
    lcdLights[7] += 64;
  }
  if (board[0][2] == 'O')
  {
    lcdLights[6] += 192;
    lcdLights[7] += 192;
  }
  if (board[1][0] == 'X')
  {
    lcdLights[0] += 16;
    lcdLights[1] += 8;
  }
  if (board[1][0] == 'O')
  {
    lcdLights[0] += 24;
    lcdLights[1] += 24;
  }
  if (board[1][1] == 'X')
  {
    lcdLights[3] += 16;
    lcdLights[4] += 8;
  }
  if (board[1][1] == 'O')
  {
    lcdLights[3] += 24;
    lcdLights[4] += 24;
  }
  if (board[1][2] == 'X')
  {
    lcdLights[6] += 16;
    lcdLights[7] += 8;
  }
  if (board[1][2] == 'O')
  {
    lcdLights[6] += 24;
    lcdLights[7] += 24;
  }

  if (board[2][0] == 'X')
  {
    lcdLights[0] += 2;
    lcdLights[1] += 1;
  }
  if (board[2][0] == 'O')
  {
    lcdLights[0] += 3;
    lcdLights[1] += 3;
  }
  if (board[2][1] == 'X')
  {
    lcdLights[3] += 2;
    lcdLights[4] += 1;
  }
  if (board[2][1] == 'O')
  {
    lcdLights[3] += 3;
    lcdLights[4] += 3;
  }
  if (board[2][2] == 'X')
  {
    lcdLights[6] += 2;
    lcdLights[7] += 1;
  }
  if (board[2][2] == 'O')
  {
    lcdLights[6] += 3;
    lcdLights[7] += 3;
  }
}

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

void resetLights()
{
  for (int i = 0; i < 8; i++)
  {
    lcdLights[i] = 0;
  }
}

void matrixRowsVal(int value)
{
  // make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  // make latchPin output high level, then 74HC595 will update the data to parallel output
  digitalWrite(latchPin, LOW);
}

void matrixColsVal(int value)
{
  // make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, MSBFIRST, value);
  // make latchPin output high level, then 74HC595 will update the data to parallel output
  digitalWrite(latchPin, HIGH);
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  //resetBoard();

  while (mqttClient.available())
  {
    //Serial.println((char)mqttClient.read());
    for (int i=0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        board[i][j] = (char)mqttClient.read();
      }
    }
  }
  resetLights();
  updateLights();
  Serial.println();
  Serial.println();
}
