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
    if (key < 32) {
      key_str = "Keycode: " + std::to_string(key);
    } else {
      key_str = std::string(1, static_cast<char>(key));
    }
    this->publish_state(key_str);
  }

  void key_released(uint8_t key) override {
    // Optional: clear the sensor state on key release
    // this->publish_state("");
  }
};

}  // namespace cardkb
}  // namespace esphome
