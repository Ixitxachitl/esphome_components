#pragma once

#include "esphome/components/cardkb/cardkb.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace cardkb {

class CardKBTextSensor : public text_sensor::TextSensor, public Component, public CardKBListener {
 public:
  CardKBTextSensor() = default;

  void setup() override {
    // Set initial state to "NONE"
    this->publish_state("NONE");
  }

  void loop() override {}

  void key_pressed(uint8_t key) override {
    // Since firmware sends key event on release, ignore this
  }

  void key_released(uint8_t key) override {
    std::string key_str;

    // Map key codes to descriptive names
    switch (key) {
      case 0xB4: key_str = "RIGHT_ARROW"; break;
      case 0xB5: key_str = "UP_ARROW"; break;
      case 0xB6: key_str = "DOWN_ARROW"; break;
      case 0xB7: key_str = "LEFT_ARROW"; break;
      case 27: key_str = "ESC"; break;
      case 9: key_str = "TAB"; break;
      case 8: key_str = "BACKSPACE"; break;
      case 13: key_str = "ENTER"; break;
      case 175: key_str = "SPACE"; break;

      // Printable ASCII range
      default:
        if (key >= 0x20 && key <= 0x7E) {
          key_str = std::string(1, static_cast<char>(key));
        } else {
          key_str = "UNKNOWN";
        }
    }

    // Publish the key string to the text sensor
    this->publish_state(key_str);

    // Optionally, reset to "NONE" after a short delay (handled in loop)
  }
};

}  // namespace cardkb
}  // namespace esphome
