import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, text_sensor
from esphome.const import CONF_ID

cardkb_ns = cg.esphome_ns.namespace('cardkb')
CardKB = cardkb_ns.class_('CardKB', cg.Component, i2c.I2CDevice)
CardKBTextSensor = cardkb_ns.class_('CardKBTextSensor', text_sensor.TextSensor, cg.Component)

CONF_CARDKB_ID = 'cardkb_id'

CONFIG_SCHEMA = i2c.i2c_device_schema().extend(
    {
        cv.GenerateID(CONF_CARDKB_ID): cv.declare_id(CardKB),
        cv.Required(CONF_ID): cv.declare_id(CardKBTextSensor),
    }
)

async def to_code(config):
    # Initialize CardKB instance
    cardkb_var = cg.new_Pvariable(config[CONF_CARDKB_ID])
    await cg.register_component(cardkb_var, config)
    await i2c.register_i2c_device(cardkb_var, config)

    # Initialize CardKBTextSensor
    text_sensor_var = cg.new_Pvariable(config[CONF_ID])
    cg.add(cardkb_var.register_listener(text_sensor_var))
    await text_sensor.register_text_sensor(text_sensor_var, config)
    await cg.register_component(text_sensor_var, config)  # Ensure component registration
