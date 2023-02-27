// timer interrupt sample
// yluo modified for lab1 of EECE.4520/5520
// 9/12/2021
//
// original source:-
//timer interrupts
//by Amanda Ghassaei
//June 2012
//https://www.instructables.com/id/Arduino-Timer-Interrupts/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/

//timer setup for timer0, timer1, and timer2.
//For arduino uno or any board with ATMEL 328/168.. diecimila, duemilanove, lilypad, nano, mini...

//this code will enable one arduino timer interrupt
//timer1 will interrupt at 1Hz

//storage variables
int toggle1 = 0;
int count = 0;
int buzzer = 2;
int button = 3;
int red = 4;
int yellow = 5;
int green = 6;
unsigned int lightState = 0;

void setup(){
  
  //set pins as outputs
  pinMode(buzzer, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  Serial.begin(9600);

  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0

  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);

  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10); 

  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts

}//end setup

// Timer1's interrupt service routing (ISR)
// The code in ISR will be executed every time timer1 interrupt occurs
// That is, the code will be called once every second
// TODO
//   you need to set a flag to trigger an evaluation of the conditions
//   in your state machine, then potentially transit to next state
//

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
    if (toggle1 == 1)
    {
      digitalWrite(red,HIGH);
      toggle1 = 0;
    }
    else if (toggle1 == 0)
    {
      digitalWrite(red,LOW);
      toggle1 = 1;
    }
    else if (toggle1 == 2)
    {
      count = count + 1;
      Serial.println(count);
      TIMSK1 &= ~(1 << OCIE1A);
      return;
    }
}

void loop()
{ //do other things here
  switch(lightState)
  {
    case 1:
      digitalWrite(red, HIGH);
      count = 0;
      while(count < 17){
        TIMSK1 |= (1 << OCIE1A);
      }
      digitalWrite(buzzer, HIGH);
      count = 0;
      while(count < 3){
        TIMSK1 |= (1 << OCIE1A);
      }
      digitalWrite(red, LOW);
      digitalWrite(buzzer, LOW);
      lightState = 2;
      break;
    case 2:
      digitalWrite(green, HIGH);
      count = 0;
      while(count < 17){
        TIMSK1 |= (1 << OCIE1A);
      }
      digitalWrite(buzzer, HIGH);
      count = 0;
      while(count < 3){
        TIMSK1 |= (1 << OCIE1A);
      }
      digitalWrite(green, LOW);
      digitalWrite(buzzer, LOW);
      lightState = 3;
      break;
    case 3:
      digitalWrite(yellow, HIGH);
      digitalWrite(buzzer, HIGH);
      count = 0;
      while(count < 3){
        TIMSK1 |= (1 << OCIE1A);
      }
      digitalWrite(yellow, LOW);
      digitalWrite(buzzer, LOW);
      lightState = 1;
      break;
    default:
      if(digitalRead(button) == LOW)
      {
        lightState = 1;
        toggle1 = 2;
      }
   }
}
