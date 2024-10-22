#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rc522/rc522.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mfrc522_i2c {

class MFRC522I2C : public rc522::RC522, public i2c::I2CDevice {
 public:
  void dump_config() override;

  // Getter methods to access UID and FIFO data
  std::string get_uid();
  std::string get_fifo_data_string();

 protected:
  uint8_t pcd_read_register(rc522::PcdRegister reg) override;
  void pcd_read_register(rc522::PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) override;
  void pcd_write_register(rc522::PcdRegister reg, uint8_t value) override;
  void pcd_write_register(rc522::PcdRegister reg, uint8_t count, uint8_t *values) override;

 private:
  bool request_tag(rc522::PICC_Command command);
  bool anti_collision(uint8_t *buffer, uint8_t *buffer_size);
  std::string uid_;
  std::string fifo_data_;
};

}  // namespace mfrc522_i2c
}  // namespace esphome
