#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

static const uint8_t COMMAND_REG = 0x0D;
static const uint8_t FIFO_DATA_REG = 0x09;
static const uint8_t CONTROL_REG = 0x0C;

void MFRC522I2C::dump_config() {
  RC522::dump_config();
  LOG_I2C_DEVICE(this);
}

void MFRC522I2C::pcd_reset_() {
  ESP_LOGD(TAG, "Resetting MFRC522...");
  this->pcd_write_register(static_cast<PcdRegister>(COMMAND_REG), 0x0F);  // PCD_SoftReset
  delay(50);

  uint8_t reset_status = this->pcd_read_register(static_cast<PcdRegister>(COMMAND_REG));
  if (reset_status != 0x00) {
    ESP_LOGW(TAG, "Reset failed, CommandReg: 0x%02X", reset_status);
  } else {
    ESP_LOGD(TAG, "Reset successful.");
  }

  // Enable the antenna
  this->pcd_write_register(static_cast<PcdRegister>(0x14), 0x03);  // TXControlReg
}

bool MFRC522I2C::read_full_uid(uint8_t *uid, uint8_t *uid_length) {
  ESP_LOGD(TAG, "Reading full UID...");
  this->pcd_write_register(static_cast<PcdRegister>(COMMAND_REG), 0x93);  // PCD_Anticoll

  *uid_length = 5;
  this->pcd_read_register(static_cast<PcdRegister>(FIFO_DATA_REG), *uid_length, uid, 0);

  if (*uid_length == 0) {
    ESP_LOGW(TAG, "Failed to read UID.");
    return false;
  }
  ESP_LOGI(TAG, "UID: %s", format_hex_pretty(uid, *uid_length).c_str());
  return true;
}

bool MFRC522I2C::read_sak(uint8_t *sak) {
  ESP_LOGD(TAG, "Reading SAK...");
  this->pcd_write_register(static_cast<PcdRegister>(COMMAND_REG), 0x70);  // PCD_Select

  *sak = this->pcd_read_register(static_cast<PcdRegister>(FIFO_DATA_REG));
  if (*sak == 0) {
    ESP_LOGW(TAG, "Failed to read SAK.");
    return false;
  }
  ESP_LOGI(TAG, "SAK: 0x%02X", *sak);
  return true;
}

bool MFRC522I2C::read_atqa(uint16_t *atqa) {
  ESP_LOGD(TAG, "Reading ATQA...");
  this->pcd_write_register(static_cast<PcdRegister>(COMMAND_REG), 0x26);  // PCD_REQA

  uint8_t atqa_buf[2] = {0};
  this->pcd_read_register(static_cast<PcdRegister>(FIFO_DATA_REG), 2, atqa_buf, 0);

  if (atqa_buf[0] == 0 && atqa_buf[1] == 0) {
    ESP_LOGW(TAG, "Failed to read ATQA.");
    return false;
  }
  *atqa = (atqa_buf[0] << 8) | atqa_buf[1];
  ESP_LOGI(TAG, "ATQA: 0x%04X", *atqa);
  return true;
}

uint8_t MFRC522I2C::pcd_read_register(PcdRegister reg) {
  uint8_t value;
  for (int attempt = 0; attempt < 3; ++attempt) {
    if (this->read_byte(static_cast<uint8_t>(reg), &value)) {
      ESP_LOGD(TAG, "Read register 0x%02X: 0x%02X", reg, value);
      return value;
    }
    delay(10);
    ESP_LOGW(TAG, "Retry reading register 0x%02X (attempt %d)", reg, attempt + 1);
  }
  ESP_LOGW(TAG, "Failed to read register 0x%02X after multiple attempts", reg);
  return 0;
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t value) {
  for (int attempt = 0; attempt < 3; ++attempt) {
    if (this->write_byte(static_cast<uint8_t>(reg), value)) {
      ESP_LOGD(TAG, "Wrote 0x%02X to register 0x%02X", value, reg);
      return;
    }
    delay(10);
    ESP_LOGW(TAG, "Retry writing to register 0x%02X (attempt %d)", reg, attempt + 1);
  }
  ESP_LOGW(TAG, "Failed to write to register 0x%02X after multiple attempts", reg);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
