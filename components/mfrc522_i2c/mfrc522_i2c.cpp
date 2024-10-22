#include "mfrc522_i2c.h"
#include "esphome/core/log.h"
#include <string>

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

void MFRC522I2C::dump_config() {
  RC522::dump_config();
  LOG_I2C_DEVICE(this);
}

bool MFRC522I2C::read_full_uid(uint8_t *uid, uint8_t *uid_length) {
  // Send Anti-collision command
  this->pcd_write_register(0x0D, 0x93);  // CommandReg = PCD_Anticoll (0x93)
  
  // Read UID from FIFODataReg (register 0x09)
  *uid_length = 5;  // Assuming a 5-byte UID
  this->pcd_read_register(0x09, *uid_length, uid, 0);  // FIFODataReg is 0x09

  ESP_LOGI(TAG, "UID: %s", format_hex_pretty(uid, *uid_length).c_str());
  return true;
}

bool MFRC522I2C::read_sak(uint8_t *sak) {
  // Send SELECT command to get SAK
  this->pcd_write_register(0x0D, 0x70);  // CommandReg = PCD_Select (0x70)

  // Read SAK from FIFODataReg
  *sak = this->pcd_read_register(0x09);  // FIFODataReg is 0x09
  ESP_LOGI(TAG, "SAK: 0x%02X", *sak);
  return true;
}

bool MFRC522I2C::read_atqa(uint16_t *atqa) {
  // Send REQA command to get ATQA
  this->pcd_write_register(0x0D, 0x26);  // CommandReg = PCD_REQA (0x26)

  // Read ATQA from FIFODataReg
  uint8_t atqa_buf[2] = {0};
  this->pcd_read_register(0x09, 2, atqa_buf, 0);  // FIFODataReg is 0x09
  *atqa = (atqa_buf[0] << 8) | atqa_buf[1];

  ESP_LOGI(TAG, "ATQA: 0x%04X", *atqa);
  return true;
}

}  // namespace mfrc522_i2c
}  // namespace esphome
