#include <Wire.h>

const uint8_t PD_ADDR = 0x22;
uint8_t tx_buf[80];
uint8_t rx_buf[80];
uint8_t temp_buf[80];

void sendPacket( \
      uint8_t num_data_objects, \
      uint8_t message_id, \
      uint8_t port_power_role, \
      uint8_t spec_rev, \
      uint8_t port_data_role, \
      uint8_t message_type, \
      uint8_t *data_objects );

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Hello");

  setReg(0x0C, 0x01); // Reset FUSB302
  setReg(0x0B, 0x0F); // FULL POWER!
  setReg(0x07, 0x04); // Flush RX
  setReg(0x02, 0x0B); // Switch on MEAS_CC2
  setReg(0x03, 0x26); // Enable BMC Tx on CC2
  readAllRegs();
  
  sendPacket( 0, 2, 0, 1, 0, 0x7, NULL );
  delay(1000);
  while ( getReg(0x41) & 0x20 ) {
    delay(1);
  }
  receivePacket();
  while ( getReg(0x41) & 0x20 ) {
    delay(1);
  }
  receivePacket();
  Serial.print("0x");
  Serial.println(getReg(0x41), HEX);
  
  temp_buf[0] = 0b01100100;
  temp_buf[1] = 0b10010000;
  temp_buf[2] = 0b00000001;
  temp_buf[3] = 0b00100000;
  sendPacket( 1, 3, 0, 1, 0, 0x2, temp_buf );
  delay(1000);
  while ( getReg(0x41) & 0x20 ) {
    delay(1);
  }
  receivePacket();
  while ( getReg(0x41) & 0x20 ) {
    delay(1);
  }
  receivePacket();
  

  readAllRegs();

}

void loop() {

  //delay(1000);
}

void readAllRegs() {
  Wire.beginTransmission(PD_ADDR);
  Wire.write(0x01);
  Wire.endTransmission(false);
  Wire.requestFrom((int)PD_ADDR, 16, 1);
  for (int i=1; i<=16; i++) {
    uint8_t c = Wire.read();
    Serial.print("Address: 0x");
    Serial.print(i, HEX);
    Serial.print(", Value: 0x");
    Serial.println(c, HEX);
  }
  
  Wire.beginTransmission(PD_ADDR);
  Wire.write(0x3C);
  Wire.endTransmission(false);
  Wire.requestFrom((int)PD_ADDR, 7, true);
  for (int i=0x3C; i<=0x42; i++) {
    uint8_t c = Wire.read();
    Serial.print("Address: 0x");
    Serial.print(i, HEX);
    Serial.print(", Value: 0x");
    Serial.println(c, HEX);
  }
  
  Serial.println();
  Serial.println();
}

void setReg(uint8_t addr, uint8_t value) {
  Wire.beginTransmission(PD_ADDR);
  Wire.write(addr);
  Wire.write(value);
  Wire.endTransmission(true);
}

uint8_t getReg(uint8_t addr) {
  Wire.beginTransmission(PD_ADDR);
  Wire.write(addr);
  Wire.endTransmission(false);
  Wire.requestFrom((int)PD_ADDR, 1, true);
  return Wire.read();
}

void sendBytes(uint8_t *data, uint16_t length) {
  if (length > 0) {
    Wire.beginTransmission(PD_ADDR);
    Wire.write(0x43);
    for (uint16_t i=0; i<length; i++) {
      Wire.write(data[i]);
    }
    Wire.endTransmission(true);
  }
}

void receiveBytes(uint8_t *data, uint16_t length) {
  if (length > 0) {
    Wire.beginTransmission(PD_ADDR);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom((int)PD_ADDR, (int)length, true);
    for (uint16_t i=0; i<length; i++) {
      data[i] = Wire.read();
    }
  }
}

bool receivePacket() {
  uint8_t num_data_objects;
  uint8_t message_id;
  uint8_t port_power_role;
  uint8_t spec_rev;
  uint8_t port_data_role;
  uint8_t message_type;
  
  receiveBytes(rx_buf, 1);
  if (rx_buf[0] != 0xE0) {
    // implement other features later
    Serial.print("FAIL: 0x");
    Serial.println(rx_buf[0], HEX);
    return false;
  }

  receiveBytes(rx_buf, 2);
  
  num_data_objects = ((rx_buf[1] & 0x70) >> 4);
  message_id       = ((rx_buf[1] & 0x0E) >> 1);
  port_power_role  = (rx_buf[1] & 0x01);
  spec_rev         = ((rx_buf[0] & 0xC0) >> 6);
  port_data_role   = ((rx_buf[0] & 0x10) >> 5);
  message_type     = (rx_buf[0] & 0x0F);
  Serial.println("Received SOP Packet");
  Serial.print("Header: 0x");
  Serial.println(*(int *)rx_buf, HEX);
  Serial.print("num_data_objects = ");
  Serial.println(num_data_objects, DEC);
  Serial.print("message_id       = ");
  Serial.println(message_id, DEC);
  Serial.print("port_power_role  = ");
  Serial.println(port_power_role, DEC);
  Serial.print("spec_rev         = ");
  Serial.println(spec_rev, DEC);
  Serial.print("port_data_role   = ");
  Serial.println(port_data_role, DEC);
  Serial.print("message_type     = ");
  Serial.println(message_type, DEC);

  receiveBytes(rx_buf, (num_data_objects*4));
  // each data object is 32 bits
  for (uint8_t i=0; i<num_data_objects; i++) {
    Serial.print("Object: 0x");
    Serial.println(*(long *)(rx_buf+(i*4)), HEX);
  }  

  // CRC-32
  receiveBytes(rx_buf, 4);
  Serial.print("CRC-32: 0x");
  Serial.println(*(long *)rx_buf, HEX);
  Serial.println();

  return true;
}

void sendPacket( \
      uint8_t num_data_objects, \
      uint8_t message_id, \
      uint8_t port_power_role, \
      uint8_t spec_rev, \
      uint8_t port_data_role, \
      uint8_t message_type, \
      uint8_t *data_objects ) {

  uint8_t temp;
  tx_buf[0]  = 0x12; // SOP, see USB-PD2.0 page 108
  tx_buf[1]  = 0x12;
  tx_buf[2]  = 0x12;
  tx_buf[3]  = 0x13;
  tx_buf[4]  = (0x80 | (2 + (4*(num_data_objects & 0x1F))));
  tx_buf[5]  = (message_type & 0x0F);
  tx_buf[5] |= ((port_data_role & 0x01) << 5);
  tx_buf[5] |= ((spec_rev & 0x03) << 6);
  tx_buf[6]  = (port_power_role & 0x01);
  tx_buf[6] |= ((message_id & 0x07) << 1);
  tx_buf[6] |= ((num_data_objects & 0x07) << 4);

  Serial.print("Sending Header: 0x");
  Serial.println(*(int *)(tx_buf+5), HEX);
  Serial.println();

  temp = 7;
  for(uint8_t i=0; i<num_data_objects; i++) {
    tx_buf[temp]   = data_objects[(4*i)];
    tx_buf[temp+1] = data_objects[(4*i)+1];
    tx_buf[temp+2] = data_objects[(4*i)+2];
    tx_buf[temp+3] = data_objects[(4*i)+3];
    temp += 4;
  }

  tx_buf[temp] = 0xFF; // CRC
  tx_buf[temp+1] = 0x14; // EOP
  tx_buf[temp+2] = 0xFE; // TXOFF
/*
  Serial.print("Sending ");
  Serial.print((10 + (4*(num_data_objects & 0x1F))), DEC);
  Serial.println(" bytes.");

  for (uint16_t i=0; i<(10 + (4*(num_data_objects & 0x1F))); i++) {
    Serial.print("0x");
    Serial.println(tx_buf[i], HEX);
  }
  Serial.println();
*/
  temp = getReg(0x06);
  sendBytes(tx_buf, (10+(4*(num_data_objects & 0x1F))) );
  setReg(0x06, (temp | (0x01))); // Flip on TX_START
}


