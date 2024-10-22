#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

MFRC522I2C::MFRC522I2C() {
  this->x.resize(2);  // Ensure x has two elements
}

void MFRC522I2C::on_scan() {
  // Read UID and update x[0]
  uint8_t uid[10];  // Adjust size based on UID length
  uint8_t uid_length = this->read_uid(uid);
  if (uid_length > 0) {
    this->x[0] = std::string(uid, uid + uid_length);
    ESP_LOGD(TAG, "UID: %s", this->x[0].c_str());
  } else {
    ESP_LOGW(TAG, "Failed to read UID");
  }

  // Read FIFO data and update x[1]
  this->read_fifo_data(10);  // Adjust the count as needed
  this->x[1] = std::string(this->fifo_data_, this->fifo_data_ + this->fifo_data_length_);
  ESP_LOGD(TAG, "FIFO Data: %s", this->x[1].c_str());
}

uint8_t MFRC522I2C::read_uid(uint8_t *uid) {
  // Example: Adjust to use a generic read for UID
  uint8_t uid_length = this->pcd_read_register(0x00);  // Replace with correct register for UID length
  if (uid_length > 0) {
    this->pcd_read_register(0x01, uid_length, uid, 0);  // Replace with correct register for UID data
  }
  return uid_length;
}

void MFRC522I2C::read_fifo_data(uint8_t count) {
  if (count > 0 && count <= MAX_FIFO_SIZE) {
    this->pcd_read_register(0x02, count, this->fifo_data_, 0);  // Replace with correct FIFO register
    this->fifo_data_length_ = count;
  }
}

uint8_t MFRC522I2C::pcd_read_register(uint8_t reg) {
  uint8_t value;
  if (!this->read_byte(reg, &value))
    return 0;
  ESP_LOGVV(TAG, "read_register_(%x) -> %u", reg, value);
  return value;
}

void MFRC522I2C::pcd_read_register(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  if (count == 0) return;

  uint8_t b = values[0];
  this->read_bytes(reg, values, count);

  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] = (b & ~mask) | (values[0] & mask);
  }
}

void MFRC522I2C::pcd_write_register(uint8_t reg, uint8_t value) {
  this->write_byte(reg, value);
}

void MFRC522I2C::pcd_write_register(uint8_t reg, uint8_t count, uint8_t *values) {
  this->write_bytes(reg, values, count);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
