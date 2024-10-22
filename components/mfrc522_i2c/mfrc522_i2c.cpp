#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

void MFRC522I2C::on_scan() {
  uint8_t uid[10];
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

  this->read_fifo_data(10);
  this->y_ = this->get_fifo_data_as_string();
  ESP_LOGD(TAG, "FIFO Data (y_): %s", this->y_.c_str());
}

uint8_t MFRC522I2C::read_uid(uint8_t *uid) {
  uint8_t uid_length = this->pcd_read_register(rc522::RC522::UIDSizeReg);
  if (uid_length > 0) {
    this->pcd_read_register(rc522::RC522::UIDStartReg, uid_length, uid, 0);
  }
  return uid_length;
}

void MFRC522I2C::read_fifo_data(uint8_t count) {
  if (count > 0 && count <= MAX_FIFO_SIZE) {
    this->pcd_read_register(rc522::RC522::FIFODataReg, count, this->fifo_data_, 0);
    this->fifo_data_length_ = count;
  }
}

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

// Implementing pure virtual methods from RC522
uint8_t MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg) {
  uint8_t value;
  rc522::RC522::pcd_read_register(reg, 1, &value, 0);
  return value;
}

void MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  rc522::RC522::pcd_read_register(reg, count, values, rx_align);
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t value) {
  rc522::RC522::pcd_write_register(reg, value);
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values) {
  rc522::RC522::pcd_write_register(reg, count, values);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
