# Arduino LED Control with 74HC595 Shift Register

This project demonstrates how to use an **Arduino** with a **74HC595 shift register** to control **8 LEDs** in sequence. The LEDs are connected to the shift register and controlled using Arduino code with interrupts for timing.

## Project Overview

- **Shift Register**: The **74HC595** is used to control 8 LEDs using just 3 Arduino pins (`DATA_PIN`, `CLOCK_PIN`, and `LATCH_PIN`).
- **LEDs**: The project controls 8 LEDs using the shift register.
- **Resistors**: Each LED is connected in series with a **330 ohm resistor** between the **VCC (power)** and the **anode** of the LED.
- **Timer Interrupts**: The Arduino uses timer interrupts to control the timing of data shifting in the shift register.

## Components Used

- **Arduino (e.g., Arduino Uno)**: Main microcontroller that runs the code and controls the shift register.
- **74HC595 Shift Register**: 8-bit shift register used to control the LEDs.
- **8 LEDs**: Used to indicate the output of the shift register.
- **8x 330 ohm Resistors**: Current-limiting resistors for each LED (placed between VCC and the LED's anode).
- **Wires**: For connecting the components.
- **Breadboard**: To organize and connect components.

## Circuit Overview

### Circuit Diagram:
- **74HC595 Shift Register**:
  - **Pin 14 **: Connected to Arduino `DATA_PIN` (Pin 4).
  - **Pin 11 **: Connected to Arduino `CLOCK_PIN` (Pin 6).
  - **Pin 12 **: Connected to Arduino `LATCH_PIN` (Pin 5).
  - **Pins 15 to 7 (Q0 to Q7)**: Connected to the LEDs, each through a 330 ohm resistor to **VCC**.

### Pinout of the Arduino:
- **Pin 4**: `DATA_PIN` (for shifting out data to the shift register).
- **Pin 5**: `LATCH_PIN` (to latch the data into the shift register).
- **Pin 6**: `CLOCK_PIN` (for clocking the shift register to shift the data).

### LED Connections:
- Connect the **anode** (positive side) of each LED to the corresponding `Q0` to `Q7` pins of the 74HC595 shift register.
- **330-ohm resistors** are connected **between the anode of the LED and VCC** to limit the current and prevent damage to the LEDs.
- Connect the **cathode** (negative side) of each LED to **ground**.

## Code Overview
Below is the Arduino code used to control the LEDs through the 74HC595 shift register. The code utilizes interrupts to manage timing and shift data to the shift register for controlling the LEDs.
```cpp
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
