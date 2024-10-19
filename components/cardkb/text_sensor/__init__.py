import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, text_sensor
from esphome.const import CONF_ID
from .. import CardKB, cardkb_ns, CONF_CARDKB_ID

DEPENDENCIES = ['cardkb']

CardKBTextSensor = cardkb_ns.class_('CardKBTextSensor', text_sensor.TextSensor, cg.Component)

CONFIG_SCHEMA = text_sensor.TEXT_SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(CardKBTextSensor),
        cv.GenerateID(CONF_CARDKB_ID): cv.use_id(CardKB),
    }
)

async def to_code(config):
    # Create the CardKBTextSensor instance
    var = cg.new_Pvariable(config[CONF_ID])
    await text_sensor.register_text_sensor(var, config)
    await cg.register_component(var, config)

    # Link with the CardKB component
    cardkb = await cg.get_variable(config[CONF_CARDKB_ID])
    cg.add(cardkb.register_listener(var))
