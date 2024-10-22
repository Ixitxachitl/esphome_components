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
  // Implement reading the full UID from the tag
  if (this->pcd_read_register(PcdRegister::CollReg) & 0x80) {
    ESP_LOGW(TAG, "Collision detected while reading UID");
    return false;
  }
  *uid_length = this->get_uid(uid);
  ESP_LOGI(TAG, "UID: %s", format_hex_pretty(uid, *uid_length).c_str());
  std::string uid_str = format_hex_pretty(uid, *uid_length);
  id(uid_sensor).publish_state(uid_str);
  return true;
}

bool MFRC522I2C::read_sak(uint8_t *sak) {
  // Implement reading the SAK value from the tag
  *sak = this->pcd_read_register(PcdRegister::ControlReg);
  ESP_LOGI(TAG, "SAK: 0x%02X", *sak);
  id(sak_sensor).publish_state(std::to_string(*sak));
  return true;
}

bool MFRC522I2C::read_atqa(uint16_t *atqa) {
  // Implement reading the ATQA value from the tag
  uint8_t atqa_buf[2];
  this->pcd_read_register(PcdRegister::ATQAReg, 2, atqa_buf, 0);
  *atqa = (atqa_buf[0] << 8) | atqa_buf[1];
  ESP_LOGI(TAG, "ATQA: 0x%04X", *atqa);
  id(atqa_sensor).publish_state(std::to_string(*atqa));
  return true;
}

}  // namespace mfrc522_i2c
}  // namespace esphome
