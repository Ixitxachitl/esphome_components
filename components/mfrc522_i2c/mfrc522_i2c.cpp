#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

void MFRC522I2C::on_scan() {
  uint8_t uid[10];
  uint8_t uid_length = this->read_uid(uid);
  this->read_fifo_data(10);  // Adjust count as needed

  // Convert UID and FIFO data into a list and store in x
  this->x = this->convert_to_list(uid, uid_length);

  ESP_LOGD(TAG, "UID: %s, FIFO Data: %s", this->x[0].c_str(), this->x[1].c_str());
}

uint8_t MFRC522I2C::read_uid(uint8_t *uid) {
  // Read the UID length (adjust register as needed)
  uint8_t uid_length = this->pcd_read_register(rc522::RC522::ComIrqReg);  // Example placeholder
  if (uid_length > 0) {
    // Read UID data (adjust register as needed)
    this->pcd_read_register(rc522::RC522::FIFOLevelReg, uid_length, uid, 0);  // Example placeholder
  }
  return uid_length;
}

void MFRC522I2C::read_fifo_data(uint8_t count) {
  if (count > 0 && count <= MAX_FIFO_SIZE) {
    this->pcd_read_register(rc522::RC522::FIFODataReg, count, this->fifo_data_, 0);
    this->fifo_data_length_ = count;
  }
}

std::vector<std::string> MFRC522I2C::convert_to_list(const uint8_t *uid, uint8_t uid_length) {
  std::string uid_str, fifo_str;

  // Convert UID to string
  for (uint8_t i = 0; i < uid_length; i++) {
    uid_str += std::to_string(uid[i]);
    if (i < uid_length - 1) {
      uid_str += " ";
    }
  }

  // Convert FIFO data to string
  for (uint8_t i = 0; i < this->fifo_data_length_; i++) {
    fifo_str += std::to_string(this->fifo_data_[i]);
    if (i < this->fifo_data_length_ - 1) {
      fifo_str += " ";
    }
  }

  return {uid_str, fifo_str};
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
