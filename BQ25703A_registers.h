#define BQ26703A_I2C_ADDRESS		0x6B // datasheets states the 0xD6 address but my chip was  detected with 0x6B address
#define BQ26703A_MANUFACTURER_ID	0x40
#define BQ26703A_DEVICE_ID			0x78

#define MANUFACTURER_ID_ADDR		0x2E
#define DEVICE_ID_ADDR				0x2F
#define MAX_CHARGE_VOLTAGE_ADDR		0x04
#define CHARGE_CURRENT_ADDR			0x02
#define CHARGE_OPTION_0_ADDR		0x00
#define CHARGE_OPTION_3_ADDR		0x35
#define MINIMUM_SYSTEM_VOLTAGE_ADDR	0x0D
#define CHARGE_STATUS_ADDR			0x20
#define ADC_OPTION_ADDR				0x3A
#define VBUS_ADC_ADDR				0x27
#define PSYS_ADC_ADDR				0x26
#define VSYS_ADC_ADDR				0x2D
#define VBAT_ADC_ADDR				0x2C
#define ICHG_ADC_ADDR				0x29
#define IDCHG_ADC_ADDR				0x28
#define IIN_ADC_ADDR				0x2B
#define OTG_VOLTAGE_ADDR			0x07
#define OTG_CURRENT_ADDR			0x09

#define EN_LWPWR					0b0
#define EN_OOA						0b1


#define CHARGING_ENABLED_MASK		0b00000100	//the bitmap for the case when charging process is OK
#define ADC_ENABLED_BITMASK			0b01010111	// setup the ADC for 1-short conversion
#define ADC_START_CONVERSION_MASK	0b01100000	//the command to start ADC

//Max voltage register 1 values
#define MAX_VOLT_ADD_16384_MV		0b01000000
#define MAX_VOLT_ADD_8192_MV		0b00100000
#define MAX_VOLT_ADD_4096_MV		0b00010000
#define MAX_VOLT_ADD_2048_MV		0b00001000
#define MAX_VOLT_ADD_1024_MV		0b00000100
#define MAX_VOLT_ADD_512_MV			0b00000010
#define MAX_VOLT_ADD_256_MV			0b00000001

//Max voltage register 2 values
#define MAX_VOLT_ADD_128_MV			0b10000000
#define MAX_VOLT_ADD_64_MV			0b01000000
#define MAX_VOLT_ADD_32_MV			0b00100000
#define MAX_VOLT_ADD_16_MV			0b00010000

//Minimum system voltage register values
#define MIN_VOLT_ADD_8192_MV		0b00100000
#define MIN_VOLT_ADD_4096_MV		0b00010000
#define MIN_VOLT_ADD_2048_MV		0b00001000
#define MIN_VOLT_ADD_1024_MV		0b00000100
#define MIN_VOLT_ADD_512_MV			0b00000010
#define MIN_VOLT_ADD_256_MV			0b00000001
