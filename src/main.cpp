/**
 * Midi 2 CV converter
 **/

#include <Arduino.h>
#include <MCP492X.h>
#include "table_midi_dac.h"

#define PIN_SPI_CHIP_SELECT_DAC 10
#define PIN_SPI_SDI 11
#define PIN_SPI_SCK 13
#define PIN_SPI_LDAC 5
#define GATE 2
#define MIN_NOTE_MIDI 36 // C octave -2
#define MAX_NOTE_MIDI 96 // C octave +3

byte commandByte;
byte noteByte;
byte velocityByte; // For future work

MCP492X myDac(PIN_SPI_CHIP_SELECT_DAC);

void setup()
{
  // init serial port to midi communication.
  Serial.begin(31250);
  // init DAC
  myDac.begin();
  // Init Gate pin
  pinMode(GATE, OUTPUT);
}

void checkMIDI()
{
  do
  {
    if (Serial.available())
    {
      commandByte = Serial.read();  //read first byte
      noteByte = Serial.read();     //read next byte
      velocityByte = Serial.read(); //read final byte
    }
  } while (Serial.available() > 2); //when at least three bytes available
}

void setGate(byte level){
  digitalWrite(GATE, level);
}

void setNote(byte noteByte){
  // values below MIN_NOTE_MIDI an above MAX_NOTE_MIDI aren't allowed
  if(noteByte < MIN_NOTE_MIDI || noteByte > MAX_NOTE_MIDI) return;
  
  myDac.analogWrite(dac[noteByte - MIN_NOTE_MIDI]);
}

void parseMIDI(){
  switch(commandByte){
    case 0x80: // Note Off omnichannel
      setGate(LOW);
      break;
    case 0x90: // Note On ommnichannel
      setGate(HIGH);
      setNote(noteByte);
      break;
    case 0xE0: // Pitch bend omnichannel
      break;
  }
}

void loop()
{
  checkMIDI();
  delay(100); // this helps to relax de loop rate
}