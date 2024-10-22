#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rc522/rc522.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mfrc522_i2c {

static const uint8_t MAX_FIFO_SIZE = 64;

class MFRC522I2C : public rc522::RC522, public i2c::I2CDevice {
 public:
  MFRC522I2C() : x_("") {}

  void on_scan();

  // Direct access to x_ as a list of strings
  std::vector<std::string> x;

  // Implement pure virtual methods from RC522
  uint8_t pcd_read_register(rc522::RC522::PcdRegister reg) override;
  void pcd_read_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) override;
  void pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t value) override;
  void pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values) override;

 protected:
  uint8_t read_uid(uint8_t *uid);
  void read_fifo_data(uint8_t count);
  std::vector<std::string> convert_to_list(const uint8_t *uid, uint8_t uid_length);

 private:
  uint8_t fifo_data_[MAX_FIFO_SIZE];
  uint8_t fifo_data_length_;
};

}  // namespace mfrc522_i2c
}  // namespace esphome