#include "mfrc522_i2c.h"

namespace esphome {
namespace mfrc522_i2c {

void MFRC522I2C::dump_config() {
  ESP_LOGCONFIG(TAG, "MFRC522I2C:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
}

uint8_t MFRC522I2C::pcd_read_register(PcdRegister reg) {
  uint8_t value;
  this->read_bytes(reg, &value, 1);
  return value;
}

void MFRC522I2C::pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  this->read_bytes(reg, values, count);
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t value) {
  this->write_bytes(reg, &value, 1);
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) {
  this->write_bytes(reg, values, count);
}

std::string MFRC522I2C::get_uid() {
  uint8_t buffer[10];
  uint8_t buffer_size = sizeof(buffer);
  if (this->request_tag(PiccCommand::PICC_CMD_REQA) && this->anti_collision(buffer, &buffer_size)) {
    this->uid_ = std::string(buffer, buffer + buffer_size);
  } else {
    this->uid_.clear();
  }
  return this->uid_;
}

std::string MFRC522I2C::get_fifo_data_string() {
  uint8_t fifo_size = this->pcd_read_register(PcdRegister::FIFO_LEVEL_REG);
  uint8_t buffer[64];
  this->pcd_read_register(PcdRegister::FIFO_DATA_REG, fifo_size, buffer, 0);
  this->fifo_data_ = std::string(buffer, buffer + fifo_size);
  return this->fifo_data_;
}

bool MFRC522I2C::request_tag(PcdCommand command) {
  this->pcd_write_register(PcdRegister::BIT_FRAMING_REG, 0x07);
  uint8_t command_buffer[] = {static_cast<uint8_t>(command)};
  this->pcd_transceive_data_(command_buffer, sizeof(command_buffer));
  return true;
}

bool MFRC522I2C::anti_collision(uint8_t *buffer, uint8_t *buffer_size) {
  this->pcd_write_register(PcdRegister::BIT_FRAMING_REG, 0x00);
  this->pcd_read_register(PcdRegister::FIFO_DATA_REG, *buffer_size, buffer, 0);
  return true;
}

}  // namespace mfrc522_i2c
}  // namespace esphome
