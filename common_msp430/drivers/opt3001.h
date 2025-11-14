#ifndef HAL_OPT3001_H_
#define HAL_OPT3001_H_

// I2C SLAVE ADDRESS
#define OPT3001_SLAVE_ADDRESS 0x44

// REGISTERS
#define RESULT_REG 0x00
#define CONFIG_REG 0x01
#define LOWLIMIT_REG 0x02
#define HIGHLIMIT_REG 0x03
#define MANUFACTUREID_REG 0x7E
#define DEVICEID_REG 0x7F

// CONFIG REGISTER PRESETS
#define DEFAULT_CONFIG 0xCC10 // 800ms
#define DEFAULT_CONFIG_100 0xC410 // 100ms


void OPT3001_config(void);
uint32_t OPT3001_get_lux(void);
uint16_t OPT3001_read_manufacturer_id(void);
uint16_t OPT3001_read_device_id(void);
uint16_t OPT3001_read_config_reg(void);
uint16_t OPT3001_read_low_limit_reg(void);
uint16_t OPT3001_read_high_limit_reg(void);

#endif /* HAL_OPT3001_H_ */
