#include <lorawan_builder.h>

#define Frame_PORT 1

// Device information archived from Network Server
// YOU SHOULD CAHNGE THIS TO YOUR DEVICE ADDR & KEYS!
uint32_t DevAddr = 0x260B7AC6;
uint8_t NwkSKey[] = {0xF3, 0x4B, 0x7E, 0xC4, 0x65, 0x3C, 0x9E, 0x78, 0x05, 0xAC, 0x21, 0x44, 0x2E, 0x1B, 0x47, 0x2B};
uint8_t AppSKey[] = {0x2E, 0x1B, 0x2E, 0x2E, 0x88, 0x36, 0x3E, 0x22, 0x16, 0x48, 0x5B, 0xA8, 0xFD, 0xC2, 0xCC, 0x14};

uint16_t Frame_Counter = 0;

uint8_t payload[] = {'H', 'E', 'L', 'L', 'O', 'W', 'O', 'R', 'L', 'D', '!'};
uint8_t payload_len = 11;

uint8_t encoded_packet[50]; // encoded_packet's size at least = payload_len + 13;
uint8_t encoded_packet_length;

void print_num_array(uint8_t *packet_to_print, uint8_t len, bool hex_format = false);
void print_char_array(char *packet_to_print, uint8_t len);

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  /*********** BUILD LORAWAN PACKET ***********/
  encoded_packet_length = build_lorawan_packet(payload, payload_len, Frame_Counter, encoded_packet, NwkSKey, AppSKey, DevAddr, Frame_PORT);

  /*********** PRINT RESULT ***********/
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

  /*********** UPDATE FRAME COUNTER & DELAY ***********/
  Frame_Counter++;
  delay(5000);
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
