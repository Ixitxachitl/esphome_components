#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

// Called when a tag is scanned
void MFRC522I2C::on_scan() {
  // Read UID and set x_
  uint8_t uid[10];  // Adjust size based on UID length
  uint8_t uid_length = this->read_uid(uid);
  if (uid_length > 0) {
    this->x_ = 0;
    for (uint8_t i = 0; i < uid_length; i++) {
      this->x_ = (this->x_ << 8) | uid[i];
    }
    ESP_LOGD(TAG, "UID (x_): %u", this->x_);
  } else {
    ESP_LOGW(TAG, "Failed to read UID");
  }

  // Read FIFO data and set y_
  this->read_fifo_data(10);  // Adjust the count as needed
  this->y_ = this->get_fifo_data_as_string();

  ESP_LOGD(TAG, "FIFO Data (y_): %s", this->y_.c_str());
}

// Read UID and return its length
uint8_t MFRC522I2C::read_uid(uint8_t *uid) {
  uint8_t uid_length = this->pcd_read_register(static_cast<MFRC522_LocalRegister>(UIDSizeReg));
  if (uid_length > 0) {
    this->pcd_read_register(static_cast<MFRC522_LocalRegister>(UIDStartReg), uid_length, uid, 0);
  }
  return uid_length;
}

// Read FIFO data into a buffer
void MFRC522I2C::read_fifo_data(uint8_t count) {
  if (count > 0 && count <= MAX_FIFO_SIZE) {
    this->pcd_read_register(static_cast<MFRC522_LocalRegister>(FIFODataReg), count, this->fifo_data_, 0);
    this->fifo_data_length_ = count;
  }
}

// Convert FIFO data to a string
std::string MFRC522I2C::get_fifo_data_as_string() {
  std::string fifo_output;
  for (uint8_t i = 0; i < this->fifo_data_length_; i++) {
    fifo_output += std::to_string(this->fifo_data_[i]);
    if (i < this->fifo_data_length_ - 1) {
      fifo_output += " ";
    }
  }
  return fifo_output;
}

// Return the UID value (x_)
uint32_t MFRC522I2C::get_x() const {
  return this->x_;
}

// Return the FIFO data string (y_)
std::string MFRC522I2C::get_y() const {
  return this->y_;
}

// Wrapper for reading a single register
uint8_t MFRC522I2C::pcd_read_register(MFRC522_LocalRegister reg) {
  return rc522::RC522::pcd_read_register(static_cast<rc522::RC522::PcdRegister>(reg));
}

// Wrapper for reading multiple registers
void MFRC522I2C::pcd_read_register(MFRC522_LocalRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  rc522::RC522::pcd_read_register(static_cast<rc522::RC522::PcdRegister>(reg), count, values, rx_align);
}

// Wrapper for writing a single register
void MFRC522I2C::pcd_write_register(MFRC522_LocalRegister reg, uint8_t value) {
  rc522::RC522::pcd_write_register(static_cast<rc522::RC522::PcdRegister>(reg), value);
}

// Wrapper for writing multiple registers
void MFRC522I2C::pcd_write_register(MFRC522_LocalRegister reg, uint8_t count, uint8_t *values) {
  rc522::RC522::pcd_write_register(static_cast<rc522::RC522::PcdRegister>(reg), count, values);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
