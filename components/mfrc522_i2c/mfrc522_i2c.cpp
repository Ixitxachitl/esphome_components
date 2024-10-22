#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

void MFRC522I2C::setup() {
  this->pcd_reset_();  // Reset the device
  delay(50);  // Wait for reset to complete
  this->pcd_antenna_on_();  // Activate antenna
  ESP_LOGI(TAG, "Antenna activated and setup complete.");
}

void MFRC522I2C::loop() {
  static uint32_t last_run = 0;
  // Run every 2 seconds to allow for sufficient communication time
  if (millis() - last_run < 2000) return;
  last_run = millis();

  uint8_t uid[10] = {0};
  uint8_t uid_length = 0;
  uint8_t sak = 0;
  uint16_t atqa = 0;

  if (this->read_full_uid(uid, &uid_length)) {
    if (this->validate_data(uid, uid_length)) {
      ESP_LOGI(TAG, "Tag detected: %s", format_hex_pretty(uid, uid_length).c_str());

      if (this->read_sak(&sak)) {
        ESP_LOGI(TAG, "SAK: 0x%02X", sak);
      } else {
        ESP_LOGW(TAG, "Failed to read SAK.");
      }

      if (this->read_atqa(&atqa)) {
        ESP_LOGI(TAG, "ATQA: 0x%04X", atqa);
      } else {
        ESP_LOGW(TAG, "Failed to read ATQA.");
      }
    } else {
      ESP_LOGW(TAG, "Invalid or repeated UID detected.");
    }
  } else {
    ESP_LOGD(TAG, "No tag detected.");
  }
}

void MFRC522I2C::dump_config() {
  ESP_LOGCONFIG(TAG, "MFRC522I2C:");
  LOG_I2C_DEVICE(this);
}

uint8_t MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg) {
  uint8_t value = 0;
  if (!this->read_byte(static_cast<uint8_t>(reg), &value)) {
    ESP_LOGW(TAG, "Failed to read register 0x%02X", reg);
  }
  delay(5);  // Add delay to stabilize I2C communication
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
  delay(5);  // Add delay after read operation
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t value) {
  if (!this->write_byte(static_cast<uint8_t>(reg), value)) {
    ESP_LOGW(TAG, "Failed to write 0x%02X to register 0x%02X", value, reg);
  }
  delay(5);  // Add delay to stabilize I2C communication
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values) {
  if (!this->write_bytes(static_cast<uint8_t>(reg), values, count)) {
    ESP_LOGW(TAG, "Failed to write %d bytes to register 0x%02X", count, reg);
  }
  delay(5);  // Add delay after write operation
}

bool MFRC522I2C::read_full_uid(uint8_t *uid, uint8_t *uid_length) {
  this->pcd_write_register(rc522::RC522::PcdRegister::COMMAND_REG, 0x93);  // Anticollision command
  *uid_length = 5;
  this->pcd_read_register(rc522::RC522::PcdRegister::FIFO_DATA_REG, *uid_length, uid, 0);

  if (*uid_length == 0 || this->is_repeated_data(uid, *uid_length)) {
    ESP_LOGW(TAG, "Failed to read UID or repeated data detected.");
    return false;
  }
  return true;
}

bool MFRC522I2C::read_sak(uint8_t *sak) {
  this->pcd_write_register(rc522::RC522::PcdRegister::COMMAND_REG, 0x70);  // Select command
  *sak = this->pcd_read_register(rc522::RC522::PcdRegister::FIFO_DATA_REG);

  if (*sak == 0) {
    ESP_LOGW(TAG, "Failed to read SAK.");
    return false;
  }
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
  return true;
}

bool MFRC522I2C::validate_data(uint8_t *data, uint8_t length) {
  // Check if all bytes are the same (e.g., 0x22) to filter out static or invalid data
  for (uint8_t i = 1; i < length; i++) {
    if (data[i] != data[0]) {
      return true;  // Valid data
    }
  }
  return false;  // All bytes are the same, likely invalid
}

bool MFRC522I2C::is_repeated_data(uint8_t *data, uint8_t length) {
  // Implement additional logic to prevent reading repeated data if necessary
  static uint8_t last_data[10] = {0};
  static uint8_t last_length = 0;

  if (length == last_length && memcmp(data, last_data, length) == 0) {
    return true;  // Data is repeated
  }

  memcpy(last_data, data, length);
  last_length = length;
  return false;  // New data
}

}  // namespace mfrc522_i2c
}  // namespace esphome
