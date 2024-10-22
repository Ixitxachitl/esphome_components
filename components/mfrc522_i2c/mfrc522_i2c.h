#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rc522/rc522.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mfrc522_i2c {

static const uint8_t MAX_FIFO_SIZE = 64;

class MFRC522I2C : public rc522::RC522, public i2c::I2CDevice {
 public:
  void on_scan();  // New method to trigger scan actions

  // Directly accessible variables for x and y
  uint32_t x_;  // Holds the tag ID
  std::string y_;  // Holds the FIFO data as a string

 protected:
  uint8_t pcd_read_register(PcdRegister reg) override;
  void pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) override;
  void pcd_write_register(PcdRegister reg, uint8_t value) override;
  void pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) override;

  uint32_t get_tag_id();
  void read_fifo_data(uint8_t count);
  std::string get_fifo_data_as_string();

 private:
  uint8_t fifo_data_[MAX_FIFO_SIZE] = {0};
  uint8_t fifo_data_length_ = 0;
};

}  // namespace mfrc522_i2c
}  // namespace esphome
