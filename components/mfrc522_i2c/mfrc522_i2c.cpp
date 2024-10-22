#include "mfrc522_i2c.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mfrc522_i2c {

static const char *const TAG = "mfrc522_i2c";

void MFRC522I2C::dump_config() {
  RC522::dump_config();
  LOG_I2C_DEVICE(this);
}

uint8_t MFRC522I2C::pcd_read_register(PcdRegister reg) {
  uint8_t value;
  if (!read_byte(reg >> 1, &value))
    return 0;
  ESP_LOGVV(TAG, "read_register_(%x) -> %u", reg, value);
  return value;
}

void MFRC522I2C::pcd_read_register(PcdRegister reg, uint8_t count, uint8_t *values, uint8_t rx_align) {
  if (count == 0) {
    return;
  }

  uint8_t b = values[0];
  read_bytes(reg >> 1, values, count);

  if (rx_align) {
    uint8_t mask = 0xFF << rx_align;
    values[0] = (b & ~mask) | (values[0] & mask);
  }
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t value) {
  this->write_byte(reg >> 1, value);
}

void MFRC522I2C::pcd_write_register(PcdRegister reg, uint8_t count, uint8_t *values) {
  write_bytes(reg >> 1, values, count);
}

std::string MFRC522I2C::get_uid() {
  uint8_t buffer[10];
  uint8_t buffer_size = sizeof(buffer);

  // Use the RC522 method to read the card UID
  if (this->request_tag(MFRC522::PICC_CMD_REQA) && this->anti_collision(buffer, &buffer_size)) {
    this->uid_ = "";
    for (uint8_t i = 0; i < buffer_size; i++) {
      char hex[3];
      snprintf(hex, sizeof(hex), "%02X", buffer[i]);
      this->uid_ += hex;
    }
  } else {
    ESP_LOGW(TAG, "Failed to read UID");
    this->uid_ = "UNKNOWN";
  }

  return this->uid_;
}

std::string MFRC522I2C::get_fifo_data() {
  uint8_t fifo_size = this->pcd_read_register(MFRC522::FIFOLevelReg);
  uint8_t buffer[fifo_size];

  // Read FIFO data
  this->pcd_read_register(MFRC522::FIFODataReg, fifo_size, buffer, 0);

  this->fifo_data_ = "";
  for (uint8_t i = 0; i < fifo_size; i++) {
    if (buffer[i] >= 32 && buffer[i] <= 126) {
      this->fifo_data_ += static_cast<char>(buffer[i]);
    } else {
      this->fifo_data_ += '.';
    }
  }

  return this->fifo_data_;
}

}  // namespace mfrc522_i2c
}  // namespace esphome
