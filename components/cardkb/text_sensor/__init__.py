import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, text_sensor
from esphome.const import CONF_ID

cardkb_ns = cg.esphome_ns.namespace('cardkb')
CardKB = cardkb_ns.class_('CardKB', cg.Component, i2c.I2CDevice)
CardKBTextSensor = cardkb_ns.class_('CardKBTextSensor', text_sensor.TextSensor)

CONF_CARDKB_ID = 'cardkb_id'

CONFIG_SCHEMA = i2c.i2c_device_schema(None).extend(
    {
        cv.GenerateID(): cv.declare_id(CardKB),
        cv.Optional(CONF_ID): cv.declare_id(CardKBTextSensor),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    text_sensor_var = cg.new_Pvariable(config[CONF_ID])
    await text_sensor.register_text_sensor(text_sensor_var, config)
    cg.add(var.register_listener(text_sensor_var))
