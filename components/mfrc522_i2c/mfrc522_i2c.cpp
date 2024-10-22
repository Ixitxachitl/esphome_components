#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

// Directly use PcdRegister constants from the original implementation
uint8_t MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg) {
  uint8_t value = 0;
  if (!this->read_byte(static_cast<uint8_t>(reg), &value)) {
    ESP_LOGW(TAG, "Failed to read register 0x%02X", reg);
  }
  ESP_LOGVV(TAG, "Read 0x%02X from register 0x%02X", value, reg);
  return value;
}

void MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  if (!this->read_bytes(static_cast<uint8_t>(reg), values, count)) {
    ESP_LOGW(TAG, "Failed to read %d bytes from register 0x%02X", count, reg);
    return;
  }
  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] &= mask;
  }
  ESP_LOGVV(TAG, "Read %d bytes from register 0x%02X", count, reg);
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t value) {
  if (!this->write_byte(static_cast<uint8_t>(reg), value)) {
    ESP_LOGW(TAG, "Failed to write 0x%02X to register 0x%02X", value, reg);
  }
  ESP_LOGVV(TAG, "Wrote 0x%02X to register 0x%02X", value, reg);
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values) {
  if (!this->write_bytes(static_cast<uint8_t>(reg), values, count)) {
    ESP_LOGW(TAG, "Failed to write %d bytes to register 0x%02X", count, reg);
  }
  ESP_LOGVV(TAG, "Wrote %d bytes to register 0x%02X", count, reg);
}

bool MFRC522I2C::read_full_uid(uint8_t *uid, uint8_t *uid_length) {
  this->pcd_write_register(rc522::RC522::PcdRegister::COMMAND_REG, 0x93);  // Anticollision command
  *uid_length = 5;
  this->pcd_read_register(rc522::RC522::PcdRegister::FIFO_DATA_REG, *uid_length, uid, 0);

  if (*uid_length == 0) {
    ESP_LOGW(TAG, "Failed to read UID.");
    return false;
  }
  ESP_LOGI(TAG, "UID: %s", format_hex_pretty(uid, *uid_length).c_str());
  return true;
}

bool MFRC522I2C::read_sak(uint8_t *sak) {
  this->pcd_write_register(rc522::RC522::PcdRegister::COMMAND_REG, 0x70);  // Select command
  *sak = this->pcd_read_register(rc522::RC522::PcdRegister::FIFO_DATA_REG);

  if (*sak == 0) {
    ESP_LOGW(TAG, "Failed to read SAK.");
    return false;
  }
  ESP_LOGI(TAG, "SAK: 0x%02X", *sak);
  return true;
}

bool MFRC522I2C::read_atqa(uint16_t *atqa) {
  this->pcd_write_register(rc522::RC522::PcdRegister::COMMAND_REG, 0x26);  // REQA command
  uint8_t atqa_buf[2] = {0};
  this->pcd_read_register(rc522::RC522::PcdRegister::FIFO_DATA_REG, 2, atqa_buf, 0);

  if (atqa_buf[0] == 0 && atqa_buf[1] == 0) {
    ESP_LOGW(TAG, "Failed to read ATQA.");
    return false;
  }
  *atqa = (atqa_buf[0] << 8) | atqa_buf[1];
  ESP_LOGI(TAG, "ATQA: 0x%04X", *atqa);
  return true;
}

}  // namespace mfrc522_i2c
}  // namespace esphome
