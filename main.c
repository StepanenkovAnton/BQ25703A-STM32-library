/* USER CODE BEGIN Header */
/*
 *how to use:
 *connect port monitor to UART4@19200 and the BQ25703A chip to I2C#2
 *if you have another port numbers modify the following functions:
 *--BQ25703_debugMessageAsString
 *--BQ25703_debugMessageAsInteger
 *--I2C_Write_Register
 *--I2C_Read_Register
 *
 *run I2C scan to figure out your chip address. Check BQ26703A_I2C_ADDRESS
 *
 *before loop:
 *-check if chip is connected: (BQ25703A_checkResponse() == BQ25703_RESPONSE_OK)
 *-set ADC for one-shot conversion mode: BQ25703A_setADCOption();
 *-set charger working mode:BQ25703A_setChargeOption0(); See datasheet for possible options.
 *
 *inside the RTOS loop:
 *-Check CHARGE_OK signal: (BQ25703A_readChargeStatus() == 1);
 *-Set the min and max voltages: BQ25703A_SetLiIonCellsNumber(1); or BQ25703A_setMinVoltage(); BQ25703A_setMaxVoltage();
 *-Set the maximum current in mA: BQ25703_setChargeCurrent(charging_current_ma);
 *-Read the ADC values: BQ25703A_readADC();
 *!!!!!!be sure the RTOS loop is running at least 1 time per 5 seconds. 
 *!!!!!!The WatchDog timer is set to 5 sec by the BQ25703A_setChargeOption0() function
 *
 *
 *somwhere outside the RTOS loop you can quickly check the following without I2C interaction:
 *-BQ25703A_getVBAT_ADC_Reading()
 *-BQ25703A_getVBUS_ADC_Reading()
 *-BQ25703A_getInputCurrentADCReading()
 *-BQ25703A_getChargeCurrentADCReading()
 *-BQ25703A_getMaxChargeCurrent()
 *
 *

 *
 **/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "BQ25703A.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART4_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void I2C_Scan()
{
	// the variable holding status creation
        HAL_StatusTypeDef res;
	// the message about the routine starts
        char info[] = "Scanning I2C bus...\r\n";
	// sending message over UART
	HAL_UART_Transmit(&huart4, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);
	/* &huart4 - address of the UART used
	 * (uint8_t*)info - ponyer to the data to be sent
	 * strlen(info) - sended message length
	 * HAL_MAX_DELAY - delay
	 */
	// going through all possible addresses
for(uint16_t i = 0 ; i < 127 ; i++)
	{
		HAL_Delay(10);
		// checking wheither the device with the address "i" is ready
		res = HAL_I2C_IsDeviceReady(&hi2c2, i << 1, 1, HAL_MAX_DELAY);
		// if yes then
			if(res == HAL_OK)
		{
			//HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
		char msg[64];
			// writing the responsive address (i) to the string
				// as a 16-bit value:
			snprintf(msg, sizeof(msg), "0x%02X", i);
			// sending the responsive address
			HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
			// new string
			HAL_UART_Transmit(&huart4, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
		}
		else HAL_UART_Transmit(&huart4, (uint8_t*)".", 1, HAL_MAX_DELAY);
	}
	HAL_UART_Transmit(&huart4, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */
	char msg_buf[64];
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C2_Init();
	MX_USART1_UART_Init();
	MX_USART4_UART_Init();
	/* USER CODE BEGIN 2 */
#ifdef BQ25703_SERIAL_DEBUG
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET);
#endif // BQ25703_SERIAL_DEBUG
	HAL_Delay(500);
	I2C_Scan();
	//BQ25703A_HIZ(1);//turn the chip off for safety
	//BQ25703A_OTG_EN(0);
	if(BQ25703A_checkResponse() == BQ25703_RESPONSE_OK)
	{
#ifdef BQ25703_SERIAL_DEBUG
		char info[] = "The BQ chip is online!";
		HAL_UART_Transmit(&huart4, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);
#endif // BQ25703_SERIAL_DEBUG

	}
	else
	{
#ifdef BQ25703_SERIAL_DEBUG
		char info[] = "The BQ chip is oFFline!";
		HAL_UART_Transmit(&huart4, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);
#endif // BQ25703_SERIAL_DEBUG
	}
	
	BQ25703A_setADCOption();
	BQ25703A_setChargeOption0();
	HAL_Delay(1000);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	
	while (1)
	{
		uint32_t charging_voltage_max_mV = 13800;
		uint32_t charging_voltage_min_mV = 11200;
		uint32_t charging_current_ma = 200;
		
		HAL_Delay(500);
		BQ25703A_readChargeStatus();
		//BQ25703A_setMaxVoltage(charging_voltage_max_mV);
		//BQ25703A_setMinVoltage(charging_voltage_min_mV);
		BQ25703A_SetLiIonCellsNumber(3);
		
		
		BQ25703_setChargeCurrent(charging_current_ma);
		BQ25703A_readChargeStatus();
		BQ25703A_readADC();
		//BQ25703A_HIZ(0);//turn the chip on
		HAL_Delay(500);
		
#ifdef BQ25703_SERIAL_DEBUG
		
		HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET);
		
		BQ25703_debugMessageAsString("Battery voltage is: ");
		BQ25703_debugMessageAsInteger(BQ25703A_getVBAT_ADC_Reading());
		
		BQ25703_debugMessageAsString("VBUS voltage is: ");
		BQ25703_debugMessageAsInteger(BQ25703A_getVBUS_ADC_Reading());
		
		BQ25703_debugMessageAsString("Input current is: ");
		BQ25703_debugMessageAsInteger(BQ25703A_getInputCurrentADCReading());
		
		BQ25703_debugMessageAsString("Charge current is: ");
		BQ25703_debugMessageAsInteger(BQ25703A_getChargeCurrentADCReading());
		
		BQ25703_debugMessageAsString("MAX charge current is: ");
		BQ25703_debugMessageAsInteger(BQ25703A_getMaxChargeCurrent());
		
#endif // BQ25703_SERIAL_DEBUG
		
		/* USER CODE END WHILE */
		
		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Configure the main internal regulator output voltage
	*/
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_DIV4;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_24;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

	/* USER CODE BEGIN I2C2_Init 0 */

	/* USER CODE END I2C2_Init 0 */

	/* USER CODE BEGIN I2C2_Init 1 */

	/* USER CODE END I2C2_Init 1 */
	hi2c2.Instance = I2C2;
	hi2c2.Init.Timing = 0x00707CBB;
	hi2c2.Init.OwnAddress1 = 0;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c2.Init.OwnAddress2 = 0;
	hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c2) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure Analogue filter
	*/
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure Digital filter
	*/
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C2_Init 2 */

	/* USER CODE END I2C2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART4_UART_Init(void)
{

	/* USER CODE BEGIN USART4_Init 0 */

	/* USER CODE END USART4_Init 0 */

	/* USER CODE BEGIN USART4_Init 1 */

	/* USER CODE END USART4_Init 1 */
	huart4.Instance = USART4;
	huart4.Init.BaudRate = 19200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART4_Init 2 */

	/* USER CODE END USART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : RS485_RE_Pin */
	GPIO_InitStruct.Pin = RS485_RE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(RS485_RE_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : RS485_DE_Pin */
	GPIO_InitStruct.Pin = RS485_DE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(RS485_DE_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/