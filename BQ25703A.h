#include "stm32l0xx_hal.h"
//#include "math.h"
//#include "cmsis_os.h"

#define	BQ25703_SERIAL_DEBUG 

#define BQ25703_RESPONSE_OK		1
#define BQ25703_RESPONSE_NONE	0


#define REG_ADC_MULTIPLIER			1000 //1000=mV,1000000=uV

#define VBUS_ADC_SCALE				(uint32_t)( 0.064 * REG_ADC_MULTIPLIER )
#define VBUS_ADC_OFFSET				(uint32_t)( 3.2 * REG_ADC_MULTIPLIER )

#define PSYS_ADC_SCALE				(uint32_t)( 0.012 * REG_ADC_MULTIPLIER )

#define VSYS_ADC_SCALE				(uint32_t)(0.064 * REG_ADC_MULTIPLIER)
#define VSYS_ADC_OFFSET				(uint32_t)(2.88 * REG_ADC_MULTIPLIER)

#define VBAT_ADC_SCALE				(uint32_t)(0.064 * REG_ADC_MULTIPLIER)
#define VBAT_ADC_OFFSET				(uint32_t)(2.88 * REG_ADC_MULTIPLIER)

#define ICHG_ADC_SCALE				(uint32_t)(0.064 * REG_ADC_MULTIPLIER)

#define IIN_ADC_SCALE				(uint32_t)(0.050 * REG_ADC_MULTIPLIER)

#define MAX_CHARGE_CURRENT_MA		6000
#define ASSUME_EFFICIENCY			0.85f
#define BATTERY_DISCONNECT_THRESH	(uint32_t)(4.215 * REG_ADC_MULTIPLIER)
#define MAX_CHARGING_POWER			60000
#define NON_USB_PD_CHARGE_POWER		2500

#define TEMP_THROTTLE_THRESH_C		40


/**
 
 *	@brief	writes a byte to  I2C bus
 *	
 *	@param addr_to_write - the I2C slave device address
 *	
 *	@param pData -the byte to write to an I2C device register
 *
 */
void I2C_Write_Register(uint8_t addr_to_write, uint8_t pData);


/**
 
 *	@brief	reads a byte from  I2C bus
 *	
 *	@param addr_to_read - the I2C slave device address
 *	
 *	@return the byte read from the I2C device register
 *
 */
uint8_t I2C_Read_Register(uint8_t addr_to_read);


/**
 
 *	@brief	checks if the BQ25703A chip in connected
 *	
 *	@param none
 *	
 *	@return 1 if connected, 0 if not found
 *
 */
uint8_t BQ25703A_checkResponse(void);


/**
 
 *	@brief	checks if the BQ25703A chip is charging
 *	
 *	@param none
 *	
 *	@return 1 if charging, 0 if not charging
 *
 */
void BQ25703A_readChargeStatus(void);

/**
 
 *	@brief	sets the BQ25703A chip for one-shot ADC conversion
 *	
 *	@param none
 *	
 *	@return none
 *
 */
void BQ25703A_setADCOption(void);


/**
 
 *	@brief	reads the BQ25703A chip ADC values and stores them to the "regulator" data structure
 *	
 *	@param	none
 *	
 *	@return	none
 *
 */
void BQ25703A_readADC(void);

/**
 
 *	@brief	a custom user function whitch sets the Ilim_HIZ pin state
 *	
 *	@param	hi_z_en - 1 to enable HIZ state (to make the HIZ pin low) 0 to disable HIZ state (to make HIZ pin high)
 *	
 *	@return	none
 *
 */
void BQ25703A_HIZ(uint8_t hi_z_en);


/**
 
 *	@brief	a custom user function whitch sets the Ilim_HIZ pin state
 *	
 *	@param	hi_z_en - 1 to enable HIZ state (to make the HIZ pin low) 0 to disable HIZ state (to make HIZ pin high)
 *	
 *	@return	none
 *
 */
void BQ25703A_OTG_EN(uint8_t otg_en);


/**
 
 *	@brief	sets the charge option register for the dedicated operation mode.
 *	
 *	@param	None
 *	
 *	@return	None
 *
 */
void BQ25703A_setChargeOption0(void);


/**
 
 *	@brief	sets MIN and MAX voltages using standart voltages for LIIon cells. Ex. 1 cell (MinV-MaxV) <> (2.7 - 4.2) V	
 *
 *	@param	number_of_cells - MinV and MaxV depend on cells number. 1,2,3 and 4 available
 *	
 *	@return	None
 *
 */
void BQ25703A_SetLiIonCellsNumber(uint8_t number_of_cells);


/**
 
 *	@brief	sets the MIN battery voltage in mV
 *
 *	@param	minVoltage - min battery voltage to set, mV
 *	
 *	@return	None
 *
 */
void BQ25703A_setMinVoltage(uint32_t minVoltage);


/**
 
 *	@brief	sets the MAX battery voltage in mV
 *
 *	@param	maxVoltage - max battery voltage, mV
 *	
 *	@return	None
 *
 */
void BQ25703A_setMaxVoltage(uint32_t maxVoltage);


/**
 
 *	@brief	sets the MAX charge current in mA
 *
 *	@param	charge_current_limit - max charge current, mA
 *	
 *	@return	None
 *
 */
void BQ25703_setChargeCurrent(uint32_t charge_current_limit);

uint8_t BQ25703A_getConnectionState(void);
uint8_t BQ25703A_getChargingState(void);
uint32_t BQ25703A_getVBAT_ADC_Reading(void);
uint32_t BQ25703A_getVBUS_ADC_Reading(void);
uint32_t BQ25703A_getInputCurrentADCReading(void);
uint32_t BQ25703A_getChargeCurrentADCReading(void);
uint32_t BQ25703A_getMaxChargeCurrent(void);
