#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rc522/rc522.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mfrc522_i2c {

static const uint8_t MAX_FIFO_SIZE = 64;  // Maximum size of the FIFO buffer

enum MFRC522_LocalRegister : uint8_t {
  FIFODataReg = 0x09,
  FIFOLevelReg = 0x0A,
  ControlReg = 0x0C,
  UIDStartReg = 0x20,
  UIDSizeReg = 0x21
};

class MFRC522I2C : public rc522::RC522, public i2c::I2CDevice {
 public:
  MFRC522I2C() : x_(0), y_("") {}

  // Directly access x_ and y_ as global variables
  uint32_t x_;
  std::string y_;

  void on_scan();

  // Implement pure virtual methods from RC522
  uint8_t pcd_read_register(PcdRegister reg) override;
  void pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) override;
  void pcd_write_register(PcdRegister reg, uint8_t value) override;
  void pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) override;

 protected:
  uint8_t read_uid(uint8_t *uid);
  void read_fifo_data(uint8_t count);
  std::string get_fifo_data_as_string();

 private:
  uint8_t fifo_data_[MAX_FIFO_SIZE];
  uint8_t fifo_data_length_;
};

}  // namespace mfrc522_i2c
}  // namespace esphome
