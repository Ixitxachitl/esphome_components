# CardKB Component for ESPHome

This ESPHome component supports the [CardKB mini keyboard](https://shop.m5stack.com/products/cardkb-mini-keyboard). It provides integration for key detection using a `text_sensor` to display the most recent key pressed, including special handling for arrow keys and other non-character keys like ESC, TAB, BACKSPACE, and ENTER.

## Features
- **Text Sensor**: Captures and displays the last key pressed on the CardKB, with support for special keys.
- **I2C Configuration**: Easily configurable over I2C, supporting custom I2C settings.

## Setup

1. **Declare the `cardkb` component** in your YAML configuration to set up the keyboard interface.
2. **Add a `text_sensor` component** to display the last key pressed.

## Example Configuration

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/Ixitxachitl/esphome_components
    components: [cardkb]

i2c:
  sda: GPIO26 # For Atom Lite
  scl: GPIO32 # -^
  scan: true
  id: i2c_bus

# CardKB component declaration
cardkb:
  address: 0x5F

# Text Sensor to display the last key pressed
text_sensor:
  - platform: cardkb
    id: cardkb_text
    name: Key
```
## Functionality
# Text Sensor
- The text_sensor captures and displays the most recent key pressed on the CardKB, including special keys:
  - ESC → "ESC"
  - TAB → "TAB"
  - BACKSPACE → "BACKSPACE"
  - ENTER → "ENTER"
  - Arrow keys: LEFT_ARROW, RIGHT_ARROW, UP_ARROW, DOWN_ARROW
# Usage Notes
- I2C Settings: Ensure that the I2C settings match your hardware configuration.
- Special Key Handling: The component automatically converts non-character keys to readable names for display.
This streamlined setup allows you to monitor key presses on the CardKB in ESPHome with a single text_sensor.
