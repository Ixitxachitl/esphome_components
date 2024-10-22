#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

void MFRC522I2C::dump_config() {
  RC522::dump_config();
  LOG_I2C_DEVICE(this);
}

uint8_t MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg) {
  uint8_t value = 0;
  this->read_byte(static_cast<uint8_t>(reg), &value);
  ESP_LOGVV(TAG, "Read register 0x%02X: 0x%02X", reg, value);
  return value;
}

void MFRC522I2C::pcd_read_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  this->read_bytes(static_cast<uint8_t>(reg), values, count);
  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] &= mask;
  }
  ESP_LOGVV(TAG, "Read %d bytes from register 0x%02X", count, reg);
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t value) {
  this->write_byte(static_cast<uint8_t>(reg), value);
  ESP_LOGVV(TAG, "Wrote 0x%02X to register 0x%02X", value, reg);
}

void MFRC522I2C::pcd_write_register(rc522::RC522::PcdRegister reg, uint8_t count, uint8_t *values) {
  this->write_bytes(static_cast<uint8_t>(reg), values, count);
  ESP_LOGVV(TAG, "Wrote %d bytes to register 0x%02X", count, reg);
}

bool MFRC522I2C::read_full_uid(uint8_t *uid, uint8_t *uid_length) {
  this->pcd_write_register(rc522::RC522::PcdRegister::COMMAND_REG, 0x93);  // PCD_Anticoll
  *uid_length = 5;
  this->pcd_read_register(rc522::RC522::PcdRegister::FIFO_DATA_REG, *uid_length, uid, 0);
  return *uid_length != 0;
}

bool MFRC522I2C::read_sak(uint8_t *sak) {
  this->pcd_write_register(rc522::RC522::PcdRegister::COMMAND_REG, 0x70);  // PCD_Select
  *sak = this->pcd_read_register(rc522::RC522::PcdRegister::FIFO_DATA_REG);
  return *sak != 0;
}

bool MFRC522I2C::read_atqa(uint16_t *atqa) {
  this->pcd_write_register(rc522::RC522::PcdRegister::COMMAND_REG, 0x26);  // PCD_REQA
  uint8_t atqa_buf[2] = {0};
  this->pcd_read_register(rc522::RC522::PcdRegister::FIFO_DATA_REG, 2, atqa_buf, 0);
  *atqa = (atqa_buf[0] << 8) | atqa_buf[1];
  return *atqa != 0;
}

}  // namespace mfrc522_i2c
}  // namespace esphome
