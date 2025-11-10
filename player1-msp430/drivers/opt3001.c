#include <hal/hal_i2c.h>
#include <drivers/opt3001.h>

void OPT3001_config()
{
    // Specify slave address for OPT3001
    HAL_I2C_setslave(OPT3001_SLAVE_ADDRESS);

    // Set Default configuration for OPT3001
    HAL_I2C_write16(CONFIG_REG, DEFAULT_CONFIG_100);
}

uint16_t OPT3001_read_manufacturer_id()
{
    // Specify slave address for OPT3001
    HAL_I2C_setslave(OPT3001_SLAVE_ADDRESS);

    // Return content of register
    return HAL_I2C_read16(MANUFACTUREID_REG);
}

uint16_t OPT3001_read_device_id()
{
    // Specify slave address for OPT3001
    HAL_I2C_setslave(OPT3001_SLAVE_ADDRESS);

    // Return content of register
    return HAL_I2C_read16(DEVICEID_REG);
}

uint16_t OPT3001_read_config_reg()
{
    // Specify slave address for OPT3001
    HAL_I2C_setslave(OPT3001_SLAVE_ADDRESS);

    // Return content of register
    return HAL_I2C_read16(CONFIG_REG);
}

uint16_t OPT3001_read_low_limit_reg()
{
    // Specify slave address for OPT3001
    HAL_I2C_setslave(OPT3001_SLAVE_ADDRESS);

    // Return content of register
    return HAL_I2C_read16(LOWLIMIT_REG);
}

uint16_t OPT3001_read_high_limit_reg()
{
    // Specify slave address for OPT3001
    HAL_I2C_setslave(OPT3001_SLAVE_ADDRESS);

    // Return content of register
    return HAL_I2C_read16(HIGHLIMIT_REG);
}

uint32_t OPT3001_get_lux()
{
    // Specify slave address for OPT3001
    HAL_I2C_setslave(OPT3001_SLAVE_ADDRESS);

    uint16_t exponent = 0;
    uint32_t result = 0;
    int16_t raw;
    raw = HAL_I2C_read16(RESULT_REG);
    // Convert to LUX
    // extract result & exponent data from raw readings
    result = raw & 0x0FFF;
    exponent = (raw >> 12) & 0x000F;
    // convert raw readings to LUX
    switch(exponent){
    case 0: //*0.015625
        result = result>>6;
        break;
    case 1: //*0.03125
        result = result>>5;
        break;
    case 2: //*0.0625
        result = result>>4;
        break;
    case 3: //*0.125
        result = result>>3;
        break;
    case 4: //*0.25
        result = result>>2;
        break;
    case 5: //*0.5
        result = result>>1;
        break;
    case 6:
        result = result;
        break;
    case 7: //*2
        result = result<<1;
        break;
    case 8: //*4
        result = result<<2;
        break;
    case 9: //*8
        result = result<<3;
        break;
    case 10: //*16
        result = result<<4;
        break;
    case 11: //*32
        result = result<<5;
        break;
    }
    return result;
}


