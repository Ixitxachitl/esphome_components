#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

// Define integer constants for register addresses
static const uint8_t COMMAND_REG = 0x0D;
static const uint8_t FIFO_DATA_REG = 0x09;
static const uint8_t CONTROL_REG = 0x0C;

void MFRC522I2C::dump_config() {
  RC522::dump_config();
  LOG_I2C_DEVICE(this);
}

// Add the reset function for MFRC522
void MFRC522I2C::pcd_reset_() {
  // Send a soft reset command to the MFRC522
  this->pcd_write_register(static_cast<PcdRegister>(COMMAND_REG), 0x0F);  // PCD_SoftReset (0x0F)
  delay(50);  // Wait for reset to complete

  // Verify reset
  if (this->pcd_read_register(static_cast<PcdRegister>(COMMAND_REG)) != 0x00) {
    ESP_LOGW(TAG, "Failed to reset MFRC522, communication error");
  }

  // Enable the antenna driver
  this->pcd_write_register(static_cast<PcdRegister>(0x14), 0x03);  // TXControlReg (0x14)
}

bool MFRC522I2C::read_full_uid(uint8_t *uid, uint8_t *uid_length) {
  // Write to CommandReg to initiate anticollision
  this->pcd_write_register(static_cast<PcdRegister>(COMMAND_REG), 0x93);  // PCD_Anticoll (0x93)

  // Read UID from FIFODataReg
  *uid_length = 5;  // Assuming a 5-byte UID
  this->pcd_read_register(static_cast<PcdRegister>(FIFO_DATA_REG), *uid_length, uid, 0);

  ESP_LOGI(TAG, "UID: %s", format_hex_pretty(uid, *uid_length).c_str());
  return true;
}

bool MFRC522I2C::read_sak(uint8_t *sak) {
  // Write to CommandReg to initiate Select
  this->pcd_write_register(static_cast<PcdRegister>(COMMAND_REG), 0x70);  // PCD_Select (0x70)

  // Read SAK from FIFODataReg
  *sak = this->pcd_read_register(static_cast<PcdRegister>(FIFO_DATA_REG));
  ESP_LOGI(TAG, "SAK: 0x%02X", *sak);
  return true;
}

bool MFRC522I2C::read_atqa(uint16_t *atqa) {
  // Write to CommandReg to initiate REQA
  this->pcd_write_register(static_cast<PcdRegister>(COMMAND_REG), 0x26);  // PCD_REQA (0x26)

  // Read ATQA from FIFODataReg
  uint8_t atqa_buf[2] = {0};
  this->pcd_read_register(static_cast<PcdRegister>(FIFO_DATA_REG), 2, atqa_buf, 0);
  *atqa = (atqa_buf[0] << 8) | atqa_buf[1];

  ESP_LOGI(TAG, "ATQA: 0x%04X", *atqa);
  return true;
}

// Implementations of pure virtual methods with error handling and retries
uint8_t MFRC522I2C::pcd_read_register(PcdRegister reg) {
  uint8_t value;
  for (int attempt = 0; attempt < 3; ++attempt) {
    if (this->read_byte(static_cast<uint8_t>(reg), &value)) {
      return value;
    }
    delay(10);  // Retry delay
  }
  ESP_LOGW(TAG, "Failed to read register 0x%02X after multiple attempts", reg);
  return 0;  // Return a default value in case of failure
}

void MFRC522I2C::pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  for (int attempt = 0; attempt < 3; ++attempt) {
    if (this->read_bytes(static_cast<uint8_t>(reg), values, count)) {
      if (rx_align) {
        uint8_t mask = 0xFF << rx_align;
        values[0] = (values[0] & mask);
      }
      return;  // Success
    }
    delay(10);  // Retry delay
  }
  ESP_LOGW(TAG, "Failed to read multiple bytes from register 0x%02X", reg);
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t value) {
  for (int attempt = 0; attempt < 3; ++attempt) {
    if (this->write_byte(static_cast<uint8_t>(reg), value)) {
      return;  // Success
    }
    delay(10);  // Retry delay
  }
  ESP_LOGW(TAG, "Failed to write to register 0x%02X after multiple attempts", reg);
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) {
  for (int attempt = 0; attempt < 3; ++attempt) {
    if (this->write_bytes(static_cast<uint8_t>(reg), values, count)) {
      return;  // Success
    }
    delay(10);  // Retry delay
  }
  ESP_LOGW(TAG, "Failed to write multiple bytes to register 0x%02X", reg);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
