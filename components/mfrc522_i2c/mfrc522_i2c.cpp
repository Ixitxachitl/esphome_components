#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

// Called when a tag is scanned
void MFRC522I2C::on_scan() {
  // Set x to the tag ID using your logic
  this->x_ = pcd_read_register(PcdRegister::TagIDReg);  // Replace with your actual tag ID register

  // Read FIFO data and set y
  read_fifo_data(10);  // Adjust the count as needed
  this->y_ = get_fifo_data_as_string();  // Store FIFO data as a string

  ESP_LOGD(TAG, "Tag ID (x): %u", this->x_);
  ESP_LOGD(TAG, "FIFO Data (y): %s", this->y_.c_str());
}

// Reads data from the FIFO buffer
void MFRC522I2C::read_fifo_data(uint8_t count) {
  if (count > 0 && count <= MAX_FIFO_SIZE) {
    // Read FIFODataReg for count bytes
    pcd_read_register(PcdRegister::FIFODataReg, count, this->fifo_data_, 0);
    this->fifo_data_length_ = count;
  }
}

// Converts FIFO data to a string (y)
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
  if (!read_byte(reg >> 1, &value))
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
  read_bytes(reg >> 1, values, count);

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
  write_bytes(reg >> 1, values, count);
}

}  // namespace mfrc522_i2c
}  // namespace esphome
