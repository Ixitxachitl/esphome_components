#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

// Dump configuration details
void MFRC522I2C::dump_config() {
  RC522::dump_config();
  LOG_I2C_DEVICE(this);
}

// Called when a tag is scanned
void MFRC522I2C::on_scan() {
  // Read UID and set x
  uint8_t uid[10];  // Adjust size based on UID length
  uint8_t uid_length = this->read_uid(uid);
  if (uid_length > 0) {
    this->x.clear();
    for (uint8_t i = 0; i < uid_length; i++) {
      this->x += std::to_string(uid[i]) + " ";
    }
    ESP_LOGD(TAG, "UID (x): %s", this->x.c_str());
  } else {
    ESP_LOGW(TAG, "Failed to read UID");
  }

  // Read FIFO data and set y
  this->read_fifo_data(10);  // Adjust the count as needed
  this->y.clear();
  for (uint8_t i = 0; i < this->fifo_data_length_; i++) {
    this->y += std::to_string(this->fifo_data_[i]) + " ";
  }
  ESP_LOGD(TAG, "FIFO Data (y): %s", this->y.c_str());
}

// Reads the UID from the MFRC522 and returns its length
uint8_t MFRC522I2C::read_uid(uint8_t *uid) {
  // Example implementation, adjust register as needed
  uint8_t uid_length = this->pcd_read_register(rc522::RC522::VersionReg);  // Placeholder
  if (uid_length > 0) {
    this->pcd_read_register(rc522::RC522::ModeReg, uid_length, uid, 0);  // Placeholder
  }
  return uid_length;
}

// Reads FIFO data into a buffer
void MFRC522I2C::read_fifo_data(uint8_t count) {
  if (count > 0 && count <= MAX_FIFO_SIZE) {
    this->pcd_read_register(rc522::RC522::FIFODataReg, count, this->fifo_data_, 0);
    this->fifo_data_length_ = count;
  }
}

// Reads a uint8_t from a specified register in the MFRC522 chip
uint8_t MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg) {
  uint8_t value;
  if (!this->read_byte(reg >> 1, &value))
    return 0;
  ESP_LOGVV(TAG, "read_register_(%x) -> %u", reg, value);
  return value;
}

// Reads multiple uint8_ts from a specified register
void MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  if (count == 0) {
    return;
  }

  uint8_t b = values[0];
  this->read_bytes(reg >> 1, values, count);

  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] = (b & ~mask) | (values[0] & mask);
  }
}

// Writes a uint8_t to a specified register in the MFRC522 chip
void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t value) {
  this->write_byte(reg >> 1, value);
}

// Writes multiple uint8_ts to a specified register
void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values) {
  this->write_bytes(reg >> 1, values, count);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
