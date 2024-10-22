#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

// Define integer constants for register addresses
static const uint8_t COMMAND_REG = 0x0D;
static const uint8_t FIFO_DATA_REG = 0x09;

void MFRC522I2C::dump_config() {
  RC522::dump_config();
  LOG_I2C_DEVICE(this);
}

bool MFRC522I2C::read_full_uid(uint8_t *uid, uint8_t *uid_length) {
  // Write to CommandReg to initiate anticollision
  this->pcd_write_register(COMMAND_REG, 0x93);  // PCD_Anticoll (0x93)

  // Read UID from FIFODataReg
  *uid_length = 5;  // Assuming a 5-byte UID
  this->pcd_read_register(FIFO_DATA_REG, *uid_length, uid, 0);

  ESP_LOGI(TAG, "UID: %s", format_hex_pretty(uid, *uid_length).c_str());
  return true;
}

bool MFRC522I2C::read_sak(uint8_t *sak) {
  // Write to CommandReg to initiate Select
  this->pcd_write_register(COMMAND_REG, 0x70);  // PCD_Select (0x70)

  // Read SAK from FIFODataReg
  *sak = this->pcd_read_register(FIFO_DATA_REG);
  ESP_LOGI(TAG, "SAK: 0x%02X", *sak);
  return true;
}

bool MFRC522I2C::read_atqa(uint16_t *atqa) {
  // Write to CommandReg to initiate REQA
  this->pcd_write_register(COMMAND_REG, 0x26);  // PCD_REQA (0x26)

  // Read ATQA from FIFODataReg
  uint8_t atqa_buf[2] = {0};
  this->pcd_read_register(FIFO_DATA_REG, 2, atqa_buf, 0);
  *atqa = (atqa_buf[0] << 8) | atqa_buf[1];

  ESP_LOGI(TAG, "ATQA: 0x%04X", *atqa);
  return true;
}

// Implementations of pure virtual methods from rc522::RC522
uint8_t MFRC522I2C::pcd_read_register(PcdRegister reg) {
  uint8_t value;
  this->read_byte(static_cast<uint8_t>(reg), &value);
  return value;
}

void MFRC522I2C::pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  this->read_bytes(static_cast<uint8_t>(reg), values, count);

  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] = (values[0] & mask);
  }
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t value) {
  this->write_byte(static_cast<uint8_t>(reg), value);
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) {
  this->write_bytes(static_cast<uint8_t>(reg), values, count);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
