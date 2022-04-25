/*
   RadioLib SX128x Transmit Example

   This example transmits packets using SX1280 LoRa radio module.
   Each packet contains up to 256 bytes of data, in the form of:
    - Arduino String
    - null-terminated char array (C-string)
    - arbitrary binary data (byte array)

   Other modules from SX128x family can also be used.

   For default module settings, see the wiki page
   https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx128x---lora-modem

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h> // https://github.com/jgromes/RadioLib
#include <lorawan_builder.h>

#define Frame_PORT 1

// SX1280 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// NRST pin:  3
// BUSY pin:  9
SX1280 radio = new Module(10, 2, 3, 9);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1280 radio = RadioShield.ModuleA;

uint32_t DevAddr = 0x260B7AC6;
uint8_t NwkSKey[] = {0xF3, 0x4B, 0x7E, 0xC4, 0x65, 0x3C, 0x9E, 0x78, 0x05, 0xAC, 0x21, 0x44, 0x2E, 0x1B, 0x47, 0x2B};
uint8_t AppSKey[] = {0x2E, 0x1B, 0x2E, 0x2E, 0x88, 0x36, 0x3E, 0x22, 0x16, 0x48, 0x5B, 0xA8, 0xFD, 0xC2, 0xCC, 0x14};

uint16_t Frame_Counter = 0;

char payload[255];
uint8_t payload_len = 0;

uint8_t encoded_packet[50]; // encoded_packet's size at least = payload_len + 13;
uint8_t encoded_packet_length;

uint8_t counter = 0;

void print_num_array(uint8_t *packet_to_print, uint8_t len, bool hex_format = false);
void print_char_array(char *packet_to_print, uint8_t len);

void setup() {
  Serial.begin(9600);

  // initialize SX1280 with default settings
  Serial.print(F("[SX1280] Initializing ... "));
  // carrier frequency:           2400.0 MHz
  // bandwidth:                   812.5 kHz
  // spreading factor:            9
  // coding rate:                 7
  // output power:                10 dBm
  // preamble length:             12 symbols
  // CRC:                         enabled
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  radio.setFrequency(2425);
  radio.setBandwidth(812.5);
  radio.setSpreadingFactor(12);
  radio.setCodingRate(8);
  radio.setPreambleLength(8);
  radio.setSyncWord(0x21); // Public syncword

  // some modules have an external RF switch
  // controlled via two pins (RX enable, TX enable)
  // to enable automatic control of the switch,
  // call the following method
  // RX enable:   4
  // TX enable:   5
  /*
    radio.setRfSwitchPins(4, 5);
  */
}

void loop() {
  Serial.print(F("[SX1280] Transmitting packet ... "));

  sprintf(payload, "Hello %d LoRaWAN world on 2.4GHz!\n", counter++);
  payload_len = strlen(payload);

  encoded_packet_length = build_lorawan_packet(payload, payload_len, Frame_Counter, encoded_packet, NwkSKey, AppSKey, DevAddr, Frame_PORT);
  Frame_Counter++;

  int state = radio.transmit(encoded_packet, encoded_packet_length);

  Serial.println("*********************************\n");
  Serial.print("Input packet ");
  Serial.print(payload_len);
  Serial.print(" byte(s): ");
  print_char_array(payload, payload_len);

  Serial.print("Frame counter: ");
  Serial.println(Frame_Counter);
  
  Serial.print("Frame port: ");
  Serial.println(Frame_PORT);

  Serial.print("Encoded packet ");
  Serial.print(encoded_packet_length);
  Serial.print(" byte(s): ");
  print_num_array(encoded_packet, encoded_packet_length, true);

  Serial.println("\n");

  if (state == RADIOLIB_ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F("success!"));

  } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F("too long!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }

  // wait for a second before transmitting again
  delay(30 * 3000);
}

void print_num_array(uint8_t *packet_to_print, uint8_t len, bool hex_format) {
  for (int i = 0; i < len; i++) {
    if (packet_to_print[i] < 16) Serial.print('0');
    Serial.print(packet_to_print[i], hex_format ? HEX : DEC);
    Serial.print(" ");
  }
  Serial.println();
}

void print_char_array(char *packet_to_print, uint8_t len) {
  for (int i = 0; i < len; i++) {
    Serial.print(packet_to_print[i]);
  }
  Serial.println();
}
