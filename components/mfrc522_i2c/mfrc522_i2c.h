#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rc522/rc522.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mfrc522_i2c {

static const uint8_t MAX_FIFO_SIZE = 64;

class MFRC522I2C : public rc522::RC522, public i2c::I2CDevice {
 public:
  void dump_config() override;
  void on_scan();  // Handles scanning events and processes UID/FIFO data
  uint8_t read_uid(uint8_t *uid);  // Reads UID from the MFRC522

  // Global variables to be used in the YAML configuration
  uint32_t x_;
  std::string y_;

  void read_fifo_data(uint8_t count);  // Reads data from the FIFO buffer
  std::string get_fifo_data_as_string();  // Converts FIFO data to string

 protected:
  uint8_t pcd_read_register(PcdRegister reg) override;
  void pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) override;
  void pcd_write_register(PcdRegister reg, uint8_t value) override;
  void pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) override;

 private:
  uint8_t fifo_data_[MAX_FIFO_SIZE] = {0};
  uint8_t fifo_data_length_ = 0;
};

}  // namespace mfrc522_i2c
}  // namespace esphome
