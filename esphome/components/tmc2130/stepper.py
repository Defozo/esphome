import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import spi
from esphome.const import CONF_ID, CONF_CS_PIN
from esphome.core import coroutine

DEPENDENCIES = ['spi']
AUTO_LOAD = ['sensor']

tmc2130_ns = cg.esphome_ns.namespace('tmc2130')
TMC2130Component = tmc2130_ns.class_('TMC2130Component', cg.Component, spi.SPIDevice)

CONF_TOFF = "toff"
CONF_BLANK_TIME = "blank_time"
CONF_RMS_CURRENT = "rms_current"
CONF_MICROSTEPS = "microsteps"
CONF_TCOOLTHRS = "tcoolthrs"
CONF_THIGH = "thigh"
CONF_SEMIN = "semin"
CONF_SEMAX = "semax"
CONF_SEDN = "sedn"
CONF_SGT = "sgt"
CONF_R_SENSE = "r_sense"
CONF_STEP_PIN = "step_pin"
CONF_DIR_PIN = "dir_pin"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(TMC2130Component),
    cv.Required(CONF_CS_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_STEP_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_DIR_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_RMS_CURRENT): cv.positive_int,
    cv.Required(CONF_R_SENSE): cv.float_,
    cv.Optional(CONF_TOFF, default=4): cv.int_range(min=0, max=15),
    cv.Optional(CONF_BLANK_TIME, default=24): cv.int_range(min=16, max=54),
    cv.Optional(CONF_RMS_CURRENT, default=1500): cv.positive_int,
    cv.Optional(CONF_MICROSTEPS, default=16): cv.int_range(min=0, max=256),
    cv.Optional(CONF_TCOOLTHRS, default=0xFFFFF): cv.hex_int,
    cv.Optional(CONF_THIGH, default=0): cv.hex_int,
    cv.Optional(CONF_SEMIN, default=5): cv.int_range(min=0, max=15),
    cv.Optional(CONF_SEMAX, default=2): cv.int_range(min=0, max=15),
    cv.Optional(CONF_SEDN, default=0b01): cv.int_range(min=0, max=0b11),
    cv.Optional(CONF_SGT, default=7): cv.int_range(min=-64, max=63),
}).extend(cv.COMPONENT_SCHEMA).extend(spi.spi_device_schema())

async def to_code(config):
    cs_pin = await cg.gpio_pin_expression(config[CONF_CS_PIN])
    step_pin = await cg.gpio_pin_expression(config[CONF_STEP_PIN])
    dir_pin = await cg.gpio_pin_expression(config[CONF_DIR_PIN])
    r_sense = config[CONF_R_SENSE]
    var = cg.new_Pvariable(config[CONF_ID], cs_pin, r_sense)
    await cg.register_component(var, config)
    # await spi.register_spi_device(var, config)

    cg.add_library('TMCStepper', '0.7.3')
    cg.add(var.set_step_pin(step_pin))
    cg.add(var.set_dir_pin(dir_pin))
    cg.add(var.set_toff(config[CONF_TOFF]))
    cg.add(var.set_blank_time(config[CONF_BLANK_TIME]))
    cg.add(var.set_rms_current(config[CONF_RMS_CURRENT]))
    cg.add(var.set_microsteps(config[CONF_MICROSTEPS]))
    cg.add(var.set_tcoolthrs(config[CONF_TCOOLTHRS]))
    cg.add(var.set_thigh(config[CONF_THIGH]))
    cg.add(var.set_semin(config[CONF_SEMIN]))
    cg.add(var.set_semax(config[CONF_SEMAX]))
    cg.add(var.set_sedn(config[CONF_SEDN]))
    cg.add(var.set_sgt(config[CONF_SGT]))
