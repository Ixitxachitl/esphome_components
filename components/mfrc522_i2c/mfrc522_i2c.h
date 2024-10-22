#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rc522/rc522.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mfrc522_i2c {

// Define register addresses as constants
static const uint8_t COMMAND_REG = 0x0D;
static const uint8_t FIFO_DATA_REG = 0x09;
static const uint8_t CONTROL_REG = 0x0C;

class MFRC522I2C : public rc522::RC522, public i2c::I2CDevice {
 public:
  void dump_config() override;

  bool read_full_uid(uint8_t *uid, uint8_t *uid_length);
  bool read_sak(uint8_t *sak);
  bool read_atqa(uint16_t *atqa);

 protected:
  // Remove 'override' since these are not overriding base methods
  uint8_t pcd_read_register(uint8_t reg);
  void pcd_read_register(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rx_align);
  void pcd_write_register(uint8_t reg, uint8_t value);
  void pcd_write_register(uint8_t reg, uint8_t count, uint8_t *values);
};

}  // namespace mfrc522_i2c
}  // namespace esphome
