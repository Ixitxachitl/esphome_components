#pragma once

#include "esphome/components/cardkb/cardkb.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace cardkb {

class CardKBTextSensor : public text_sensor::TextSensor, public Component, public CardKBListener {
 public:
  CardKBTextSensor() = default;

  void setup() override {}
  void loop() override {}

  void key_pressed(uint8_t key) override {
    std::string key_str;

    // Map special characters to readable names
    switch (key) {
      case 0x25: key_str = "LEFT_ARROW"; break;
      case 0x26: key_str = "UP_ARROW"; break;
      case 0x27: key_str = "RIGHT_ARROW"; break;
      case 0x28: key_str = "DOWN_ARROW"; break;
      default:
        // Handle printable ASCII characters
        if (key >= 0x20 && key <= 0x7E) {
          key_str = std::string(1, static_cast<char>(key));
        } else {
          // Handle other special characters or unknown values
          key_str = "UNKNOWN";
        }
    }

    // Publish the key string to the text sensor
    this->publish_state(key_str);
  }

  void key_released(uint8_t key) override {
    // Optional: clear the sensor state on key release
    // this->publish_state("");
  }
};

}  // namespace cardkb
}  // namespace esphome
