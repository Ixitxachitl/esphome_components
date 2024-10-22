#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rc522/rc522.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mfrc522_i2c {

class MFRC522I2C : public rc522::RC522, public i2c::I2CDevice {
 public:
  // Constructor
  MFRC522I2C() : x_(0), y_("") {}

  // Function to call when a tag is scanned
  void on_scan();

  // Getters for x_ and y_
  uint32_t get_x() const;
  std::string get_y() const;

 protected:
  uint8_t read_uid(uint8_t *uid);
  void read_fifo_data(uint8_t count);
  std::string get_fifo_data_as_string();

 private:
  uint32_t x_;            // Store UID as an integer
  std::string y_;         // Store FIFO data as a string
  uint8_t fifo_data_[64]; // Buffer for FIFO data
  uint8_t fifo_data_length_;
};

}  // namespace mfrc522_i2c
}  // namespace esphome
