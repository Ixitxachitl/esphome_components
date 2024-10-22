#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

void MFRC522I2C::dump_config() {
  RC522::dump_config();
  LOG_I2C_DEVICE(this);
}

bool MFRC522I2C::read_full_uid(uint8_t *uid, uint8_t *uid_length) {
  // Use the original working logic for UID reading
  this->pcd_write_register(PcdRegister::CommandReg, 0x93);  // PCD_Anticoll (0x93)

  // Read UID from FIFODataReg
  *uid_length = 5;  // Assuming a 5-byte UID
  this->pcd_read_register(PcdRegister::FIFODataReg, *uid_length, uid, 0);

  ESP_LOGI(TAG, "UID: %s", format_hex_pretty(uid, *uid_length).c_str());
  return true;
}

bool MFRC522I2C::read_sak(uint8_t *sak) {
  // Use the original working logic for SAK reading
  this->pcd_write_register(PcdRegister::CommandReg, 0x70);  // PCD_Select (0x70)

  // Read SAK from FIFODataReg
  *sak = this->pcd_read_register(PcdRegister::FIFODataReg);
  ESP_LOGI(TAG, "SAK: 0x%02X", *sak);
  return true;
}

bool MFRC522I2C::read_atqa(uint16_t *atqa) {
  // Use the original working logic for ATQA reading
  this->pcd_write_register(PcdRegister::CommandReg, 0x26);  // PCD_REQA (0x26)

  // Read ATQA from FIFODataReg
  uint8_t atqa_buf[2] = {0};
  this->pcd_read_register(PcdRegister::FIFODataReg, 2, atqa_buf, 0);
  *atqa = (atqa_buf[0] << 8) | atqa_buf[1];

  ESP_LOGI(TAG, "ATQA: 0x%04X", *atqa);
  return true;
}

// Revert register read/write methods to the original logic
uint8_t MFRC522I2C::pcd_read_register(PcdRegister reg) {
  // Direct read from register using I2C
  uint8_t value;
  this->read_byte(static_cast<uint8_t>(reg), &value);
  return value;
}

void MFRC522I2C::pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  // Direct read from register with original logic
  this->read_bytes(static_cast<uint8_t>(reg), values, count);

  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] = (values[0] & mask);
  }
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t value) {
  // Direct write to register using I2C
  this->write_byte(static_cast<uint8_t>(reg), value);
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) {
  // Direct write to register with original logic
  this->write_bytes(static_cast<uint8_t>(reg), values, count);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
