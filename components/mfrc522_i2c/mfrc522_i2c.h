#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/rc522/rc522.h"

namespace esphome {
namespace mfrc522_i2c {

class MFRC522I2C : public rc522::RC522, public i2c::I2CDevice {
 public:
  void dump_config() override;

  uint8_t pcd_read_register(PcdRegister reg) override;
  void pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) override;
  void pcd_write_register(PcdRegister reg, uint8_t value) override;
  void pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) override;

  std::string get_uid();
  std::string get_fifo_data_string();
  bool request_tag(PICC_Command command);
  bool anti_collision(uint8_t *buffer, uint8_t *buffer_size);

 protected:
  std::string uid_;
  std::string fifo_data_;
};

}  // namespace mfrc522_i2c
}  // namespace esphome
