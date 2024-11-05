#include <avr/interrupt.h> 
#include <avr/io.h>

#define BUFFER_LENGTH 250
#define ANALOG_PIN A0

const int DATA_PIN = 4;
const int LATCH_PIN = 5;
const int CLOCK_PIN = 6;  

int buffer[BUFFER_LENGTH];
int buffer_index = 0;
volatile char tick = 0; 
int count = 0;

void setup() { 
  cli();
  Serial.begin(9600);
  pinMode(ANALOG_PIN, INPUT);
  
  pinMode(DATA_PIN, OUTPUT);   
  pinMode(CLOCK_PIN, OUTPUT);
  
  DDRB = (1 << LATCH_PIN);
  DDRB &= ~(1 << 2);

  TCCR1A = 0; 
  TCCR1B = 0; 
  TIMSK1 |= B00000001; 
 
  PCICR |= B00000001;
  PCMSK0 |= B00000100; 

  sei();
} 

ISR(PCINT0_vect) {
  PORTB &= ~(1 << 5);
  TCNT1 = 65535;
  TCCR1B |= B00000101;
} 

ISR(TIMER1_OVF_vect) { 
  if (tick == 0){
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, count);
    count++;
    PORTB |= (1 << LATCH_PIN);
    TCNT1 = 0;
    tick = 1;
  } else{
    PORTB &= ~(1 << LATCH_PIN);
    TCCR1B = 0;
    tick = 0;
  }
} 
void loop() { 
  for (int i = 0; i < BUFFER_LENGTH; i++){
    Serial.println(buffer[i]);
  }
  if (buffer_index < BUFFER_LENGTH){
    buffer[buffer_index] = analogRead(ANALOG_PIN);
    buffer_index = (buffer_index + 1) % BUFFER_LENGTH;
  }
}
