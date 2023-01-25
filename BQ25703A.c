#include "stm32l0xx_hal.h"
#include "BQ25703A.h" 
#include "math.h"
#include "string.h" 
#include "BQ25703A_registers.h"


extern I2C_HandleTypeDef hi2c2;

struct Regulator {
	uint8_t connected;
	uint8_t charging_status;
	uint16_t max_charge_voltage;
	uint8_t input_current_limit;
	uint16_t min_input_voltage_limit;
	uint32_t vbus_voltage;
	uint32_t vbat_voltage;
	uint32_t vsys_voltage;
	uint32_t charge_current;
	uint32_t input_current;
	uint32_t max_charge_current_ma;
};

struct Regulator regulator;

UART_HandleTypeDef huart4;


uint8_t  BQ25703A_getConnectionState() {
	return regulator.connected;
}


uint8_t BQ25703A_getChargingState() {
	return regulator.charging_status;
}

/**
 * @brief Gets VBAT voltage that was read in from the ADC on the regulator
 * @retval VBAT voltage in volts * REG_ADC_MULTIPLIER
 */
uint32_t BQ25703A_getVBAT_ADC_Reading() {
	return regulator.vbat_voltage;
}

/**
 * @brief Gets VBUS voltage that was read in from the ADC on the regulator
 * @retval VBUS voltage in volts * REG_ADC_MULTIPLIER
 */
uint32_t BQ25703A_getVBUS_ADC_Reading() {
	return regulator.vbus_voltage;
}



/**
 * @brief Gets Input Current that was read in from the ADC on the regulator
 * @retval Input Current in amps * REG_ADC_MULTIPLIER
 */
uint32_t BQ25703A_getInputCurrentADCReading() {
	return regulator.input_current;
}

/**
 * @brief Gets Charge Current that was read in from the ADC on the regulator
 * @retval Charge Current in amps * REG_ADC_MULTIPLIER
 */
uint32_t BQ25703A_getChargeCurrentADCReading() {
	return regulator.charge_current;
}

/**
 * @brief Gets the max output current for charging
 * @retval Max Charge Current in miliamps
 */
uint32_t BQ25703A_getMaxChargeCurrent() {
	return regulator.max_charge_current_ma;
}



void I2C_Write_Register(uint8_t addr_to_write, uint8_t pData) 
{
	uint8_t data[2];
	data[0] = addr_to_write;
	data[1] = pData;
	HAL_I2C_Master_Transmit(&hi2c2, BQ26703A_I2C_ADDRESS << 1, data, 2, 1000);
}



void BQ25703_debugMessageAsString(char message[])
{
	char msg[64];
	sprintf(msg, message);
	HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void BQ25703_debugMessageAsInteger(uint32_t stringBuf)
	
{
	char msg[64];
	snprintf(msg, sizeof(msg), "%+08i", stringBuf);
	HAL_UART_Transmit(&huart4, (uint32_t*)msg, strlen(msg), HAL_MAX_DELAY);
	snprintf(msg, sizeof(msg), "\r\n");	
	HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	
}

void BQ25703_debugMessageAsHEX(int8_t message)
{
	char msg[64];
	snprintf(msg, sizeof(msg), "0x%02X", message);	
	HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}
void BQ25703_debugMessageAsHEX32(int8_t message)
{
	char msg[64];
	snprintf(msg, sizeof(msg), "0x%08X", message);	
	HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}


uint8_t I2C_Read_Register(uint8_t addr_to_read) 
{
	//I2C_Transfer((uint8_t *)&addr_to_read, 1);
	//I2C_Receive(pData, size);
	
	//uint8_t buf[2];
	//buf[0] = addr_to_read;
	//HAL_I2C_Mem_Read(&hi2c2, BQ26703A_I2C_ADDRESS << 1, buf[0], I2C_MEMADD_SIZE_8BIT, (uint8_t *)&buf[0], 1, 1000) ;
	//pData = buf;
	
	uint8_t buf;
	
	buf = addr_to_read;
	
	if (HAL_I2C_Mem_Read(&hi2c2, BQ26703A_I2C_ADDRESS << 1, buf, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&buf, 1, 1000) == HAL_OK)
	{
		
	}
	return buf;
	//BQ25703_debugMessageAsHEX(pData);
	
	
	

	
}


void BQ25703A_OTG_EN(uint8_t otg_en)
{
	//fullfill this if you plan to drive OTG pin from uC
	//if (OTG_en)
	//{
	//make_OTG_pin_HIGH
	//}
	//else
	//{
	//make_OTG_PIN_LOW
	//}
	//	
}

void BQ25703A_HIZ(uint8_t hi_z_en)
{
	//fullfill this if you plan to drive HIZ pin from uC
	//if (hi_z_en)
	//{
	//make_HIZ_pin_LOW
	//}
	//else
	//{
	//make_HIZ_PIN_HIGH
	//}
	//
}
uint8_t BQ25703A_checkResponse() {
	
	/* Get the manufacturer id */
	uint8_t manufacturer_id;
	
	manufacturer_id = I2C_Read_Register(MANUFACTURER_ID_ADDR);
	//BQ25703_debugMessageAsHEX(manufacturer_id);

	/* Get the device id */
	uint8_t device_id;
	device_id = I2C_Read_Register(DEVICE_ID_ADDR);
	

	if ((device_id == BQ26703A_DEVICE_ID) && (manufacturer_id == BQ26703A_MANUFACTURER_ID)) {
		return BQ25703_RESPONSE_OK;
	}
	else {
		return BQ25703_RESPONSE_NONE;
	}

}




void BQ25703A_readChargeStatus() 
{
	uint8_t data[2];
	data[0] = I2C_Read_Register(CHARGE_STATUS_ADDR);
	data[1] = I2C_Read_Register(CHARGE_STATUS_ADDR+1);
	if (data[1] & CHARGING_ENABLED_MASK) {
		regulator.charging_status = 1;
	}
	else {
		regulator.charging_status = 0;
	}
	//BQ25703_debugMessageAsHEX(data[0]);
	//BQ25703_debugMessageAsHEX(data[1]);

}


void BQ25703A_setADCOption() 
{

	uint8_t ADC_lsb_3A = ADC_ENABLED_BITMASK;

	I2C_Write_Register(ADC_OPTION_ADDR, ADC_lsb_3A);
}

void BQ25703A_readADC() 
{

	uint8_t ADC_msb_3B = ADC_START_CONVERSION_MASK;

	I2C_Write_Register((ADC_OPTION_ADDR + 1), ADC_msb_3B);

	/* Wait for the conversion to finish */
	while (ADC_msb_3B & (1 << 6)) {
		HAL_Delay(100);
		ADC_msb_3B = I2C_Read_Register(ADC_OPTION_ADDR + 1);
	}

	uint8_t temp = 0;

	temp = I2C_Read_Register(VBAT_ADC_ADDR);
	regulator.vbat_voltage = (temp * VBAT_ADC_SCALE) + VBAT_ADC_OFFSET;
	
	
	temp = I2C_Read_Register(VSYS_ADC_ADDR);
	regulator.vsys_voltage = (temp * VSYS_ADC_SCALE) + VSYS_ADC_OFFSET;

	temp = I2C_Read_Register(ICHG_ADC_ADDR);
	regulator.charge_current = temp * ICHG_ADC_SCALE;

	temp = I2C_Read_Register(IIN_ADC_ADDR);
	regulator.input_current = temp * IIN_ADC_SCALE;

	temp = I2C_Read_Register(VBUS_ADC_ADDR);
	regulator.vbus_voltage = (temp * VBUS_ADC_SCALE) + VBUS_ADC_OFFSET;
	

}


void BQ25703A_setChargeOption0() 
{

	//uint8_t charge_option_0_register_1_value = 0b00100110;
	//uint8_t charge_option_0_register_2_value = 0b00001110;
	uint8_t charge_option_0_register_1_value = 0b00100010;//
	uint8_t charge_option_0_register_2_value = 0b00001000;
	
	I2C_Write_Register(CHARGE_OPTION_0_ADDR, charge_option_0_register_2_value);
	I2C_Write_Register(CHARGE_OPTION_0_ADDR+1, charge_option_0_register_1_value);
	
}


void BQ25703_setChargeCurrent(uint32_t charge_current_limit) 
{

	uint32_t charge_current = 0;

	if (charge_current_limit > MAX_CHARGE_CURRENT_MA) {
		charge_current_limit = MAX_CHARGE_CURRENT_MA;
	}

	regulator.max_charge_current_ma = charge_current_limit;

	if (charge_current_limit != 0) {
		charge_current = charge_current_limit / 64;
	}

	if (charge_current > 128) {
		charge_current = 128;
	}

	//0-128 which remaps from 64mA-8.128A. 7 bit value.
	uint8_t charge_current_register_1_value = 0;
	uint8_t charge_current_register_2_value = 0;

	if ((charge_current >= 0) || (charge_current <= 128)) {
		charge_current_register_1_value = (charge_current >> 2);
		charge_current_register_2_value = (charge_current << 6);
	}
	
	//charge_current_register_2_value = 0b01000000;
	//charge_current_register_1_value = 0b00000000;
	//BQ25703_debugMessageAsHEX(charge_current_register_2_value);
	//BQ25703_debugMessageAsHEX(charge_current_register_1_value);

	
	I2C_Write_Register(CHARGE_CURRENT_ADDR, charge_current_register_2_value);
	I2C_Write_Register(CHARGE_CURRENT_ADDR+1, charge_current_register_1_value);
	return;
}





void BQ25703A_setMinVoltage(uint32_t minVoltage)
{
	uint32_t	minimum_system_voltage_value = 0;
	uint8_t		minimum_system_voltage_register = 0;
	minimum_system_voltage_value = minVoltage / 256;
	//if (minimum_system_voltage_value > 0b00111111)
	//{
	//	minimum_system_voltage_value = 0b00111111;
		
	//}
	minimum_system_voltage_register = (uint8_t)minimum_system_voltage_value;
	//minimum_system_voltage_register = 0b00100011;
	
	I2C_Write_Register(MINIMUM_SYSTEM_VOLTAGE_ADDR, minimum_system_voltage_value);
}


void BQ25703A_setMaxVoltage(uint32_t maxVoltage)
{
	uint8_t		maximum_system_voltage_register1 = 0;
	uint8_t		maximum_system_voltage_register2 = 0;
	uint32_t	maximum_system_voltage_value = 0;
	maximum_system_voltage_value = maxVoltage;
	maximum_system_voltage_value = maximum_system_voltage_value / 16;
	
	//if (maximum_system_voltage_value > 0b111111111111)
	//{
	//	maximum_system_voltage_value = 0b111111111111;
	//}
	maximum_system_voltage_register1 = (uint8_t)(0b11111111 & maximum_system_voltage_value) << 4;
	
	maximum_system_voltage_register2 = (uint8_t)(0b11111111 & (maximum_system_voltage_value >> 4));
	
	//maximum_system_voltage_register1 =0b00010000;//0x10
	//maximum_system_voltage_register2 =0b00101110;//0x2E
	//BQ25703_debugMessageAsHEX(maximum_system_voltage_register1);
	//BQ25703_debugMessageAsHEX(maximum_system_voltage_register2);
	
	I2C_Write_Register(MAX_CHARGE_VOLTAGE_ADDR, maximum_system_voltage_register1);
	I2C_Write_Register(MAX_CHARGE_VOLTAGE_ADDR+1, maximum_system_voltage_register2);
	
}

void BQ25703A_SetLiIonCellsNumber(uint8_t number_of_cells) {

	uint8_t max_charge_register_1_value = 0;
	uint8_t max_charge_register_2_value = 0;

	uint8_t	minimum_system_voltage_value = MIN_VOLT_ADD_1024_MV;

	if ((number_of_cells > 0) || (number_of_cells < 5)) {
		switch (number_of_cells) {
		case 1:
			max_charge_register_1_value = MAX_VOLT_ADD_4096_MV;
			max_charge_register_2_value = MAX_VOLT_ADD_64_MV | MAX_VOLT_ADD_32_MV;
			minimum_system_voltage_value = MIN_VOLT_ADD_2048_MV | MIN_VOLT_ADD_512_MV | MIN_VOLT_ADD_256_MV;
			break;
		case 2:
			max_charge_register_1_value = MAX_VOLT_ADD_8192_MV;
			max_charge_register_2_value = MAX_VOLT_ADD_128_MV | MAX_VOLT_ADD_64_MV | MAX_VOLT_ADD_16_MV;
			minimum_system_voltage_value = MIN_VOLT_ADD_4096_MV | MIN_VOLT_ADD_1024_MV | MIN_VOLT_ADD_512_MV;
			break;
		case 3:
			max_charge_register_1_value = MAX_VOLT_ADD_8192_MV | MAX_VOLT_ADD_4096_MV | MAX_VOLT_ADD_256_MV;
			max_charge_register_2_value = MAX_VOLT_ADD_32_MV | MAX_VOLT_ADD_16_MV;
			minimum_system_voltage_value = MIN_VOLT_ADD_8192_MV |  MIN_VOLT_ADD_256_MV;
			break;
		case 4:
			max_charge_register_1_value = MAX_VOLT_ADD_16384_MV | MAX_VOLT_ADD_256_MV;
			max_charge_register_2_value = MAX_VOLT_ADD_128_MV | MAX_VOLT_ADD_32_MV;
			minimum_system_voltage_value = MIN_VOLT_ADD_8192_MV | MIN_VOLT_ADD_2048_MV | MIN_VOLT_ADD_1024_MV;
			break;
		default:
			max_charge_register_1_value = 0;
			max_charge_register_2_value = 0;
			minimum_system_voltage_value = MIN_VOLT_ADD_1024_MV;
			break;
		}
	}

	
	I2C_Write_Register(MINIMUM_SYSTEM_VOLTAGE_ADDR, minimum_system_voltage_value);
	I2C_Write_Register(MAX_CHARGE_VOLTAGE_ADDR, max_charge_register_2_value);
	I2C_Write_Register(MAX_CHARGE_VOLTAGE_ADDR+1, max_charge_register_1_value);
	//I2C_Write_Two_Byte_Register(MAX_CHARGE_VOLTAGE_ADDR, max_charge_register_2_value, max_charge_register_1_value);

	return;
}

