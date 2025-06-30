#include <Arduino.h>
#include <pico/stdlib.h>

// output pin
#define IRLED 0

// button pins
#define SP1_PIN 1
#define SP2_PIN 
#define SP3_PIN 
#define SP4_PIN 
#define SP5_PIN 
#define OFF_PIN 2
#define LIGHT_PIN 3
#define TIMER_PIN 8

#define USERCODE 0b11
#define START_SEQ 0b110

typedef const uint8_t Command;

// "command" codes (more on this later)
Command speed_cmd[5] = {
  0b0000001,
  0b0010000,
  0b0001000,
  0b0000100,
  0b0000010,
};
Command off_cmd = 0b0100000;
Command light_cmd = 0b1000110;
Command timer_cmd = 0b1000011;

void sendFrame(uint8_t cmd) {
  // the frame, a 12 bits "command" sent by the remote, is made of
  // a starting sequence (0b110), the user code (2 bits) and the actual command
  // delay(32);
  int frame = (START_SEQ << 9) | (USERCODE << 7) | cmd;
  for (int i = 11; i >= 0; i--) {
    /* the proprietary protocol sends a 38khz signal.
    if the bit is 1, the signal will be high for 1266us (which is 3/4 of the period)
    after that, it will wait other 422us (which is 1/4 of the period)
    otherwise, the signal will be high for 1/4 of the period
    and low for the remaining time */
    if (frame & (1<<i)) {
      analogWrite(IRLED, 128);
      delayMicroseconds(1266);
      analogWrite(IRLED, 0);
      delayMicroseconds(422);
    } else {
      analogWrite(IRLED, 128);
      delayMicroseconds(422);
      analogWrite(IRLED, 0);
      delayMicroseconds(1266);
    }
  }
  
  // if the button stays pressed, the remote must wait additional time before sending another frame
  delayMicroseconds(6750);
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);

  pinMode(IRLED, OUTPUT);

  // yes, those should be inputs, but for some reason, if they are configured as inputs, nothing works on my setup
  pinMode(SP1_PIN, OUTPUT);
  pinMode(OFF_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(TIMER_PIN, OUTPUT);

  analogWriteFreq(38000);
  analogWrite(IRLED, 0);

  Serial.println("Powered on!");
}

void loop() {
  if (digitalRead(SP1_PIN))
    sendFrame(speed_cmd[0]);
  else if (digitalRead(OFF_PIN))
    sendFrame(off_cmd);
  else if (digitalRead(LIGHT_PIN))
    sendFrame(light_cmd);
  else if (digitalRead(TIMER_PIN))
    sendFrame(timer_cmd);
}
