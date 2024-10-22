#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rc522/rc522.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mfrc522_i2c {

enum PcdRegister {
  CommandReg = 0x0D,
  FIFODataReg = 0x09,
  ControlReg = 0x0C,
  // Add more registers as needed
};

class MFRC522I2C : public rc522::RC522, public i2c::I2CDevice {
 public:
  void dump_config() override;

  bool read_full_uid(uint8_t *uid, uint8_t *uid_length);
  bool read_sak(uint8_t *sak);
  bool read_atqa(uint16_t *atqa);

 protected:
  uint8_t pcd_read_register(PcdRegister reg) override;
  void pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) override;
  void pcd_write_register(PcdRegister reg, uint8_t value) override;
  void pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) override;
};

}  // namespace mfrc522_i2c
}  // namespace esphome
