
//TODO:
//-figure out the following bug: the algoMusic cannot play in the same pin as the tone() function . The PWM signal from tone seems to mess up the setup of the registers on pin 10 making AlgoMusic. At the moment the workaround is to get regular tone()s from pin 3 and AlgoMusic from pin 10, and both are connected to the buzzer. this is irrelevant for the basic functionality and it could ommited completely.
//-output the sound in a soundsystem, probably speakers that sound through holes at the front or top.
//-charlieplex the leds and change the relevant code to free up pins
//-put an RGB LED in the head.
//-feed the big red switch signal to arduino and control the motors through a mosfet to the independent system.
//-put air pumps in the hands and control them


#include "pitches.h"
#include "MelodyUtils.h"

int state;     //the state the robot is in. this code emulates a statemachine
const int protein = 11;      // the pin that the protein LED array is attached
const int cog = A0;    // pin that the cog potentiometer is attached to
const int buzzerAlgoM = 10;   //pin the buzzer is attached to. algoMusic only works on pin 10, as the appropriate timer registers are there(?)
const int buzzer = 3;          //pin the buzzer is also attached to. This is the proper pin for making sounds based on tone()
const int bigRed = 1;   //pin the big red LED of the switch that controls the head is attached to
float voltage;            //voltage that the variable resistance is giving(proxy for cog position)
float average;            //for the cog calibration
float last_average;
float average_some_time_ago; float increment=20;
float maxl=400;
float minl=0;
int order[]={4,5,9,6,7,8};     //order in which the proteins will be dispensed, so that the operator knows which will be for the kid and the proteins are dispensed equally
int physical_order[]={5,6,7,8,9,4};       //actual order of the LEDs, left to right, in terms of the pins they are in
int dispensed_protein=0;


void putb(byte b)   //for creation of sounds from the processor. Can be ommited.
{
 static long m;
 long t;
 while((t=micros())-m < 125);
 m=t;
 
 OCR1B=b;
}

void algoMusic(){                           //creates algorithmic music and some visualisations of all the LEDs. Can be ommited. basically an easter egg, Dave goes wild
  long v=0,g=0,t=0;
  int p=0,r= random(1,13);
  while(t<100000){
    t++;
    if (t%1000==0){   //toggle the LEDs
      g++;
      int flip=g%2;
      digitalWrite(protein,flip);
      digitalWrite(bigRed,flip);
      for(p=0;p<6;p++){digitalWrite(physical_order[p],(p+flip)%2);}
    }
    if(r==1){
      putb(t*((t>>12|t>>8)&63&t>>4));
    }
    if(r==2){
      putb(((-t&4095)*(255&t*(t&t>>13))>>12)+(127&t*(234&t>>8&t>>3)>>(3&t>>14)));
    }
    if(r==3){
      putb(t*(t>>11&t>>8&123&t>>3));
    }
    if(r==4){
      putb(t*((t>>9|t>>13)&25&t>>6));
    }
    if(r==5){
      putb((t*(t>>5|t>>8))>>(t>>16));
    }
    if(r==6){
      putb(((t*(t>>8|t>>9)&46&t>>8))^(t&t>>13|t>>6));
    }
    if(r==7){
      putb(((t&4096)?((t*(t^t%255)|(t>>4))>>1):(t>>3)|((t&8192)?t<<2:t)));
    }
    if(r==8){
      putb((t>>7|t|t>>6)*10+4*(t&t>>13|t>>6));
    }
    if(r==9){
      putb(t*5&(t>>7)|t*3&(t*4>>10));
    }
    if(r==10){
      putb((t|(t>>9|t>>7))*t&(t>>11|t>>9));
    }
    if(r==11){
      putb(v=(v>>1)+(v>>4)+t*(((t>>16)|(t>>6))&(69&(t>>9))));
    }
    if(r==12){
      putb((t>>6|t|t>>(t>>16))*10+((t>>11)&7));
    }
    if(r==13){
      putb((t*(4|7&t>>13)>>((~t>>11)&1)&128) + ((t)*(t>>11&t>>13)*((~t>>9)&3)&127));
    }
  }
  digitalWrite(protein,0);
  digitalWrite(bigRed,0);
  for(p=0;p<6;p++)digitalWrite(physical_order[p],0);
}


void armpit_green_sound(){
  MelodyUtils mel(buzzer);
  randomSeed(analogRead(3));
  int j = random(1,10);
  if(j==1){
    //mel.Trem(NOTE_G6, 3, 3);
    mel.Glis(NOTE_G5,NOTE_G6,0);
    delay(300);
    mel.Glis(NOTE_A7,NOTE_A5,0);
    delay(100);
    mel.Trem(NOTE_C3,1,3);
    delay(100);
    mel.Trem(NOTE_C3,1,3);
    delay(100);
    mel.Glis(NOTE_C5,NOTE_C7,0);
    mel.Glis(NOTE_G5,NOTE_F7,0);
  }else if(j==2){
    mel.Glis(NOTE_D5,NOTE_D7,0);
    delay(500);
    mel.Trem(NOTE_A6,1,3);
    delay(100);
    mel.Trem(NOTE_F5,1,3);
    delay(100);
    mel.Glis(NOTE_C5,NOTE_C7,0);
    mel.Glis(NOTE_G5,NOTE_F7,0);
  }else if(j==3){
    mel.Glis(NOTE_C3,NOTE_A5,0);
    delay(200);
    mel.Glis(NOTE_C7,NOTE_C5,0);
    mel.Glis(NOTE_G5,NOTE_F7,0);
  }else if(j==4){
    mel.Trem(NOTE_A6,1,3);
    delay(100);
    mel.Trem(NOTE_A6,1,3);
    delay(100);
    mel.Trem(NOTE_G5,1,3);
    delay(100);
    mel.Trem(NOTE_A6,1,3);
    delay(100);
    mel.Trem(NOTE_B6,1,3);
    delay(100);
    mel.Trem(NOTE_A6,1,3);
  }else if(j==5){
    mel.Glis(NOTE_D5,NOTE_D7,0);
    delay(500);
    mel.Trem(NOTE_A6,1,3);
    delay(100);
    mel.Trem(NOTE_F5,1,3);
    delay(100);
    mel.Glis(NOTE_C5,NOTE_C7,0);
    mel.Glis(NOTE_G5,NOTE_F7,0);
  }
  delay(100); //for stability of reads
}

void armpit_red_sound(){
  MelodyUtils mel(buzzer);
  randomSeed(analogRead(3));
  int j = random(1,10);
  if(j==1){
    //mel.Trem(NOTE_G6, 10, 3);
    mel.Glis(NOTE_G6,NOTE_G5,0);
    delay(300);
    mel.Glis(NOTE_G6,NOTE_G5,0);
    mel.Glis(NOTE_G6,NOTE_G4,0);
   }else if(j==2){
    mel.Glis(NOTE_D7,NOTE_D5,0);
    delay(500);
    mel.Glis(NOTE_C7,NOTE_C4,0);
    mel.Glis(NOTE_G7,NOTE_F5,0);
  }else if(j==3){
    mel.Glis(NOTE_C5,NOTE_A3,0);
    delay(200);
    mel.Glis(NOTE_C5,NOTE_C7,0);
    delay(100);
    mel.Trem(NOTE_G6,1,3);
    delay(100);
    mel.Trem(NOTE_G6,1,3);
    delay(100);
    mel.Glis(NOTE_G7,NOTE_F5,0);
  }else if (j==4){
    int i;
    for(i=0;i<3;i++){
      mel.Glis(NOTE_G6,NOTE_G5,0);
      delay(100);
    }
  }else if (j==5){
    int i;
    for(i=0;i<3;i++){
      mel.Trem(NOTE_G6,1,3);
      delay(100);
    }
  }
  while(digitalRead(13)==1){
    delay(10); //for stability of reads from the button
  }
}


void awesome_sound(){            /////one awesome sound that is awesome
  MelodyUtils mel(buzzer);
  randomSeed(analogRead(3));
  int j = random(1,3);
  if(j==1){
    mel.Trem(NOTE_G6, 3, 3);
    delay(100);
    int i;
    for(i=0;i<6;i++){
      mel.Glis(NOTE_G2,NOTE_G6,0);
      delay(100);
    }
  }else if (j==2){
    mel.Trem(NOTE_C6, 3, 3);
    delay(100);
    int i;
    for(i=0;i<6;i++){
      mel.Glis(NOTE_C2,NOTE_C6,0);
      delay(100);
    }
  }
}

void unleash_the_deamon_sound(){            /////the fail sound
  MelodyUtils mel(buzzer);
  mel.Trem(NOTE_G6, 3, 3);
  delay(100);
  int i;
  for(i=0;i<6;i++){
    mel.Glis(NOTE_G6,NOTE_G2,0);
    delay(100);
  }
  mel.Glis(NOTE_G4,NOTE_C5,1);
  delay(100);
  mel.Glis(NOTE_F4,NOTE_B5,1);
  delay(100);
  mel.Glis(NOTE_E4,NOTE_A5,1);
}

void awesome_flashing_leds(){
  randomSeed(analogRead(3));
  int i;
  for(i=1;i<100;i++){
    int j=random(4,10);
    if (j==10)j=2; //the LED that used to be at 10 is in 2 now
    digitalWrite(j,1);
    delay(i);
    digitalWrite(j,0);
    tone(buzzer,i);
  }
  if(dispensed_protein>=6){dispensed_protein=0;}
  digitalWrite(order[dispensed_protein],1);
  awesome_sound();
  delay(5000);
  digitalWrite(order[dispensed_protein++],0);
}







void setup() {          // put your setup code here, to run once:
  state=0;
  TCCR1B = (TCCR1B & 0xf8) | 1;  //for algorithmic music
  analogWrite(buzzerAlgoM,1);             //algoMusic only works in pin 10
  MelodyUtils mel(buzzer);
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT); // transcription big red button LED
  pinMode(12, INPUT);    //armpit red
  pinMode(13, INPUT);    //armpit green
  average = analogRead(cog);
  last_average=average;
  maxl=average;minl=average;
}









void loop() {               // put your main code here, to run repeatedly:
  
  MelodyUtils mel(buzzer);
  voltage = (analogRead(cog)/2.9);  //the analog read gets us values from 0 to 1023, so we divide by 4 to get values that the PWM can understand

  //int i; for(i=0;i<6;i++){digitalWrite(order[i],1);};  //debuging, puting all the protein lights up

  if(digitalRead(12) && digitalRead(13)){ //if they manage to press both buttons
    algoMusic();
  }
  if(digitalRead(13)==1){     //if green armpit button is pressed
    armpit_green_sound();
  }
  if(digitalRead(12)==1){     //if red armpit button is pressed
    armpit_red_sound();
  }
  
  
  if(state==0){       //"wait to go to start" state
    analogWrite(protein, 0); //swich off protein LEDs
    average=0;
    if(voltage<20){
      state=1;
      average_some_time_ago=20;
      analogWrite(protein, 40); //swich on first protein LED for a second
      delay(1000);
      analogWrite(protein, 0);
    }
  }
  
  
  else if(state==1){      //start + "translate protein" state, valid if the cog keeps turning correctly
    digitalWrite(bigRed,1);   // switch on the big red LED
    average += (voltage - average)*0.1; //weird function to take an average across reads
    delay(1);  //for stability between reads
    analogWrite(protein, average);  //send a voltage to the chip LM3914N-1 that handles many leds based on input voltage. the output from the pin has to pass through a low pass filter before it goes to the chip
    if(average>average_some_time_ago+increment){
    average_some_time_ago=average; // rachet that increases and must not decrease, otherwise we go to state 2
    }else if(average<average_some_time_ago-increment){
      state=2;   //the kid made a mistake, started translating backwards
      digitalWrite(bigRed,0);   // switch off the big red LED
    }
    if(average>220){       //success!! the kid translated the protein
      digitalWrite(bigRed,0);   // switch off  the big red LED
      state=3;
    }
  }
  
  else if(state==2){    //fail state, the cog turned backwards. Mayhem, anarchy, brace yourselves for the worst
    unleash_the_deamon_sound();
    int i,j;
    for(j=1;j<4.;j++){    //awesome visualisation of the failed transcription of the protein, the kid is in tears and rethinks his life, contemplates on fate, decides to be a Edgar Alan Poet
      for(i=254;i>0;i--){
        analogWrite(protein,i);
        delay(3);
      }
    }
    state=0;
  }
  
  else if(state==3){     //success state
    awesome_sound();
    awesome_flashing_leds();
    state=0; //oh the transience of success 
  }

  Serial.print("voltage=");Serial.print(voltage);Serial.print("    state=");Serial.print(state);Serial.print("    average=");Serial.println(average);
}
