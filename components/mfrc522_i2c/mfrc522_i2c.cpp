#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

// Constructor (using default)
MFRC522I2C::MFRC522I2C() {
  this->x.resize(2);  // Ensure x has two elements
}

// Called when a tag is scanned
void MFRC522I2C::on_scan() {
  // Read UID and update x[0]
  uint8_t uid[10];  // Adjust size based on UID length
  uint8_t uid_length = this->read_uid(uid);
  if (uid_length > 0) {
    this->x[0] = std::string(reinterpret_cast<char *>(uid), uid_length);
    ESP_LOGD(TAG, "UID: %s", this->x[0].c_str());
  } else {
    ESP_LOGW(TAG, "Failed to read UID");
  }

  // Read FIFO data and update x[1]
  this->read_fifo_data(10);  // Adjust the count as needed
  this->x[1] = std::string(reinterpret_cast<char *>(this->fifo_data_), this->fifo_data_length_);
  ESP_LOGD(TAG, "FIFO Data: %s", this->x[1].c_str());
}

// Reads the UID from the MFRC522 and returns its length
uint8_t MFRC522I2C::read_uid(uint8_t *uid) {
  uint8_t uid_length = this->pcd_read_register(rc522::RC522::VersionReg);  // Example register read
  if (uid_length > 0) {
    this->pcd_read_register(rc522::RC522::ModeReg, uid_length, uid, 0);
  }
  return uid_length;
}

// Reads FIFO data into the buffer
void MFRC522I2C::read_fifo_data(uint8_t count) {
  if (count > 0 && count <= MAX_FIFO_SIZE) {
    this->pcd_read_register(rc522::RC522::FIFODataReg, count, this->fifo_data_, 0);
    this->fifo_data_length_ = count;
  }
}

// Implement the pure virtual functions from RC522
uint8_t MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg) {
  uint8_t value;
  if (!this->read_byte(reg >> 1, &value))
    return 0;
  ESP_LOGVV(TAG, "read_register_(%x) -> %u", reg, value);
  return value;
}

void MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  if (count == 0) return;

  uint8_t b = values[0];
  this->read_bytes(reg >> 1, values, count);

  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] = (b & ~mask) | (values[0] & mask);
  }
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t value) {
  this->write_byte(reg >> 1, value);
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values) {
  this->write_bytes(reg >> 1, values, count);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
