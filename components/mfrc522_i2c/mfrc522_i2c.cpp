#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

// Define register constants specific to MFRC522
enum MFRC522_Register {
  FIFODataReg = 0x09,       // FIFO data register
  FIFOLevelReg = 0x0A,      // FIFO level register
  ControlReg = 0x0C,        // Control register
  UIDStartReg = 0x20,       // Register for UID start
  UIDSizeReg = 0x21         // Register for UID length
};

// Called when a tag is scanned
void MFRC522I2C::on_scan() {
  // Read UID and set x_
  uint8_t uid[10];  // Adjust size based on UID length
  uint8_t uid_length = this->read_uid(uid);
  if (uid_length > 0) {
    // Combine UID bytes into a single integer for x_
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
  this->y_ = this->get_fifo_data_as_string();  // Store FIFO data as a string

  ESP_LOGD(TAG, "FIFO Data (y_): %s", this->y_.c_str());
}

// Reads the UID from the MFRC522 and returns its length
uint8_t MFRC522I2C::read_uid(uint8_t *uid) {
  // Read the UID size from the UIDSizeReg
  uint8_t uid_length = this->pcd_read_register(static_cast<PcdRegister>(MFRC522_Register::UIDSizeReg));
  if (uid_length > 0) {
    // Read UID data from UIDStartReg
    this->pcd_read_register(static_cast<PcdRegister>(MFRC522_Register::UIDStartReg), uid_length, uid, 0);
  }
  return uid_length;
}

// Reads FIFO data into a buffer and converts it to a string
void MFRC522I2C::read_fifo_data(uint8_t count) {
  if (count > 0 && count <= MAX_FIFO_SIZE) {
    this->pcd_read_register(static_cast<PcdRegister>(MFRC522_Register::FIFODataReg), count, this->fifo_data_, 0);
    this->fifo_data_length_ = count;
  }
}

// Converts FIFO data to a space-separated string
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

// Reads a uint8_t from a specified register in the MFRC522 chip
uint8_t MFRC522I2C::pcd_read_register(PcdRegister reg) {
  uint8_t value;
  if (!this->read_byte(reg >> 1, &value))
    return 0;
  ESP_LOGVV(TAG, "read_register_(%x) -> %u", reg, value);
  return value;
}

// Reads multiple uint8_ts from a specified register
void MFRC522I2C::pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
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
void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t value) {
  this->write_byte(reg >> 1, value);
}

// Writes multiple uint8_ts to a specified register
void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) {
  this->write_bytes(reg >> 1, values, count);
}

// Returns the UID value (x_)
uint32_t MFRC522I2C::get_x() const {
  return this->x_;
}

// Returns the FIFO data string (y_)
std::string MFRC522I2C::get_y() const {
  return this->y_;
}

}  // namespace mfrc522_i2c
}  // namespace esphome
