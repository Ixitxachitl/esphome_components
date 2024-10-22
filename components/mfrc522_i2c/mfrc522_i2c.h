#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rc522/rc522.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mfrc522_i2c {

static const uint8_t MAX_FIFO_SIZE = 64;  // Maximum size of the FIFO buffer

// Define local register constants specific to MFRC522
enum MFRC522_LocalRegister : uint8_t {
  FIFODataReg = 0x09,   // FIFO data register
  FIFOLevelReg = 0x0A,  // FIFO level register
  ControlReg = 0x0C,    // Control register
  UIDStartReg = 0x20,   // UID start register
  UIDSizeReg = 0x21     // UID length register
};

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

  // Wrapper methods to access the protected PcdRegister
  uint8_t pcd_read_register(MFRC522_LocalRegister reg);
  void pcd_read_register(MFRC522_LocalRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align);
  void pcd_write_register(MFRC522_LocalRegister reg, uint8_t value);
  void pcd_write_register(MFRC522_LocalRegister reg, uint8_t count, uint8_t *values);

 private:
  uint32_t x_;            // Store UID as an integer
  std::string y_;         // Store FIFO data as a string
  uint8_t fifo_data_[MAX_FIFO_SIZE];  // Buffer for FIFO data
  uint8_t fifo_data_length_;          // Length of FIFO data
};

}  // namespace mfrc522_i2c
}  // namespace esphome
