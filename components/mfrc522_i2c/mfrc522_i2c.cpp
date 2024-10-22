#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

void MFRC522I2C::dump_config() {
  RC522::dump_config();
  LOG_I2C_DEVICE(this);
}

uint8_t MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg) {
  uint8_t value;
  if (!read_byte(static_cast<uint8_t>(reg), &value))
    return 0;
  ESP_LOGVV(TAG, "read_register_(%x) -> %u", reg, value);
  return value;
}

void MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  if (count == 0) {
    return;
  }

  uint8_t b = values[0];
  read_bytes(static_cast<uint8_t>(reg), values, count);

  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] = (b & ~mask) | (values[0] & mask);
  }
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t value) {
  this->write_byte(static_cast<uint8_t>(reg), value);
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values) {
  write_bytes(static_cast<uint8_t>(reg), values, count);
}

std::string MFRC522I2C::get_uid() {
  uint8_t buffer[10];
  uint8_t buffer_size = sizeof(buffer);

  // Use the RC522 method to read the card UID
  if (this->request_tag(rc522::RC522::PICC_Command::PICC_CMD_REQA) && this->anti_collision(buffer, &buffer_size)) {
    this->uid_ = "";
    for (uint8_t i = 0; i < buffer_size; i++) {
      char hex[3];
      snprintf(hex, sizeof(hex), "%02X", buffer[i]);
      this->uid_ += hex;
    }
  } else {
    ESP_LOGW(TAG, "Failed to read UID");
    this->uid_ = "UNKNOWN";
  }

  return this->uid_;
}

std::string MFRC522I2C::get_fifo_data_string() {
  uint8_t fifo_size = this->pcd_read_register(rc522::RC522::PcdRegister::FIFOLevelReg);
  uint8_t buffer[fifo_size];

  // Read FIFO data
  this->pcd_read_register(rc522::RC522::PcdRegister::FIFODataReg, fifo_size, buffer, 0);

  this->fifo_data_ = "";
  for (uint8_t i = 0; i < fifo_size; i++) {
    if (buffer[i] >= 32 && buffer[i] <= 126) {
      this->fifo_data_ += static_cast<char>(buffer[i]);
    } else {
      this->fifo_data_ += '.';
    }
  }

  return this->fifo_data_;
}

bool MFRC522I2C::request_tag(rc522::RC522::PICC_Command command) {
  // Send a command to request a tag
  this->pcd_write_register(rc522::RC522::PcdRegister::CommandReg, static_cast<uint8_t>(command));

  // Wait for a response
  uint8_t status = this->pcd_read_register(rc522::RC522::PcdRegister::Status1Reg);
  if (status & 0x01) { // Status register bit 0 indicates if a tag is present
    return true;
  } else {
    ESP_LOGW(TAG, "No tag detected");
    return false;
  }
}

bool MFRC522I2C::anti_collision(uint8_t *buffer, uint8_t *buffer_size) {
  // Perform anti-collision procedure to detect and select a tag
  this->pcd_write_register(rc522::RC522::PcdRegister::BitFramingReg, 0x00);

  // Read UID bytes
  this->pcd_read_register(rc522::RC522::PcdRegister::FIFODataReg, *buffer_size, buffer, 0);

  if (*buffer_size > 0) {
    return true;
  } else {
    ESP_LOGW(TAG, "Anti-collision failed");
    return false;
  }
}

}  // namespace mfrc522_i2c
}  // namespace esphome
