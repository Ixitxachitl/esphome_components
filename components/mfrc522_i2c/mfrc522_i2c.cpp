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
  // Enable collision detection by writing to CollReg (0x0E)
  this->pcd_write_register(static_cast<rc522::RC522::PcdRegister>(0x0E), 0x80);

  // Read the number of bytes in the FIFO from FIFOLevelReg (0x0A)
  uint8_t uid_length = this->pcd_read_register(static_cast<rc522::RC522::PcdRegister>(0x0A));
  if (uid_length > 0) {
    // Read UID data from FIFODataReg (0x09)
    this->pcd_read_register(static_cast<rc522::RC522::PcdRegister>(0x09), uid_length, uid, 0);
  }
  return uid_length;
}

void MFRC522I2C::read_fifo_data(uint8_t count) {
  if (count > 0 && count <= MAX_FIFO_SIZE) {
    // Read FIFO data from FIFODataReg (0x09)
    this->pcd_read_register(static_cast<rc522::RC522::PcdRegister>(0x09), count, this->fifo_data_, 0);
    this->fifo_data_length_ = count;
  }
}

uint8_t MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg) {
  uint8_t value;
  if (!this->read_byte(static_cast<uint8_t>(reg), &value))
    return 0;
  ESP_LOGVV(TAG, "read_register_(%x) -> %u", reg, value);
  return value;
}

void MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  if (count == 0) return;

  uint8_t b = values[0];
  this->read_bytes(static_cast<uint8_t>(reg), values, count);

  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] = (b & ~mask) | (values[0] & mask);
  }
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t value) {
  this->write_byte(static_cast<uint8_t>(reg), value);
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values) {
  this->write_bytes(static_cast<uint8_t>(reg), values, count);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
