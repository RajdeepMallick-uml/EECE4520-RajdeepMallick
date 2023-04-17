/************************
EECE 4520 - 201
LAB - 3
Design a controller to control a fan driven by a DC motor with the follwing requirements - 
(1) Use the Arduino to control the motor that turns a fan. The fan can run at different speeds
and in either direction (clockwise or counterclockwise)
(2) Set up a Real Time Clock (RTC) and retrieve time information
(3) Display the real-time clock and the fan’s rotation direction (“C” for clockwise, or “CC” or
counterclockwise) and speed (“Full”, “3/4”, “1/2”, or “0”) on a 2-row LCD display
(4) Update the information every second (so that the clock is counting up on the display)
(5) Allow pressing a push button to change the rotation direction of the fan.
************************/
/************************
Shiv Patel - #01907453
Rajdeep Mallick - #01857803
************************/

#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal.h>
#define ENABLE 5
#define DIRA 3
#define DIRB 4
#define button 13

DS3231 clock;
RTCDateTime dt;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int speed = 0;

void setup() 
{
  pinMode(ENABLE,OUTPUT);                 // Set ENABLE as OUTPUT.
  pinMode(DIRA,OUTPUT);                   // Set DIRA as OUTPUT.
  pinMode(DIRB,OUTPUT);                   // Set DIRB as OUTPUT.
  Serial.begin(9600);                     // Set Baud Rate to 9600.
  clock.begin();                          // Initialize DS3231.
  clock.setDateTime(__DATE__, __TIME__);  // Send sketch compiling time to Arduino.
  lcd.begin(16, 2);                       // Set up the LCD's number of columns and rows.
  pinMode(button,INPUT);
  digitalWrite(DIRA,LOW);                  
  digitalWrite(DIRB,LOW);
  
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


  // Button interrupts
  PCICR = 0x1;                         // Set bit 0 of PCICR for enabling PCI0
  PCMSK0 = 0x80;                       // Set bit 5 of PCMSK0 for enablign PCINT7
  

  sei();//allow interrupts
}

// Timer1's interrupt service routing (ISR)
// The code in ISR will be executed every time timer1 interrupt occurs
// That is, the code will be called once every second
//timer1 interrupt 1Hz 
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)

ISR(TIMER1_COMPA_vect)
{
  lcd.clear();                          // Clear Display.
  lcd.setCursor(0, 0);                  // Set cursor to (0,0) location.
  lcd.print(dt.hour);                   // Display hour.
  lcd.print(":");
  lcd.print(dt.minute);                 // Display minute.
  lcd.print(":");
  lcd.print(dt.second);                 // Display second.

  if((digitalRead(DIRA) == HIGH) && (digitalRead(DIRB) == LOW))         // Clockwise direction.
  {
    lcd.setCursor(0, 1);
    lcd.print("C");
  }
  else if((digitalRead(DIRA) == LOW) && (digitalRead(DIRB) == HIGH))    // Counter-Clockwise direction.
  {
    lcd.setCursor(0, 1);
    lcd.print("CC");
  }
  else
  {
    lcd.setCursor(0, 1);                // No Direction because motor is OFF.
    lcd.print("NO DIR");
  }

  if(speed == 0)                        // Motor OFF.
  {
    lcd.setCursor(7, 1);
    lcd.print("0");
  }
  else if(speed == 1)                   // Motor at 1/2 speed.
  {
    lcd.setCursor(7, 1);
    lcd.print("1/2");
  }
  else if(speed == 2)                   // Motor at 3/4 speed.
  {
    lcd.setCursor(7, 1);
    lcd.print("3/4");
  }
  else if(speed == 3)                   // Motor at FULL speed.
  {
    lcd.setCursor(7, 1);
    lcd.print("FULL");
  }
}

ISR(PCINT0_vect)                        // PCINT - Interrupts everytime a value change is detected.
{
  if(digitalRead(button) == LOW)        
  {
    if((digitalRead(DIRA) == HIGH) && (digitalRead(DIRB) == LOW))
    {
      digitalWrite(DIRA, LOW);          // Configure to Counter-Clockwise direction.
      digitalWrite(DIRB, HIGH);
    }
    else if((digitalRead(DIRA) == LOW) && (digitalRead(DIRB) == HIGH))
    {
      digitalWrite(DIRA, HIGH);         // Configure to clockwise direction.
      digitalWrite(DIRB, LOW);
    }
  }
}

void loop() 
{
  dt = clock.getDateTime();             // Get current Date and Time.

  if(dt.second == 0)                    // Start motor at 0 seconds.      
  {
    digitalWrite(DIRA, HIGH);           // Default to Clockwise direction.
    digitalWrite(DIRB, LOW);
    analogWrite(ENABLE,128);            // Configure motor at 1/2 Speed.
    speed = 1;
  }
  else if(dt.second == 10)
  {
    analogWrite(ENABLE,192);            // Configure motor at 3/4 Speed.
    speed = 2;
  }
  else if(dt.second == 20)
  {
    analogWrite(ENABLE,255);            // Configure motor at Full Speed.
    speed = 3;
  }
  else if(dt.second >= 30)
  {
    digitalWrite(DIRA, LOW);
    digitalWrite(DIRB, LOW);
    digitalWrite(ENABLE,LOW);           // Motor off.
    speed = 0;
  }

  delay(1000);
}
