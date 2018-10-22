/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "queue.h"
#include "keypad.h"
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
RTC_TimeTypeDef ALARM_TIME;

bool Alarm_State = false;
bool Setting_Session = false;
bool Alarm_Session = false;
bool Time_Session = false;

uint8_t password_1 = 0;
uint8_t password_2 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM4_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
}

void GPIO_ResetPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

void Set_Number(uint8_t num, uint8_t dig){	
	switch(dig){
		case 1:
			SSD1306_SetCursor(0, 24);
			SSD1306_WriteChar(num + '0', Font_16x32, White);
	  
			SSD1306_UpdateScreen();
			break;
		case 2:
			SSD1306_SetCursor(16, 24);
			SSD1306_WriteChar(num + '0', Font_16x32, White);
	  
			SSD1306_UpdateScreen();
			break;
		case 3:
			SSD1306_SetCursor(48, 24);
			SSD1306_WriteChar(num + '0', Font_16x32, White);
	  
			SSD1306_UpdateScreen();
			break;
		case 4:
			SSD1306_SetCursor(64, 24);
			SSD1306_WriteChar(num + '0', Font_16x32, White);
	  
			SSD1306_UpdateScreen();
			break;
	}
}

void Set_Minutes(uint8_t min){
	uint8_t quotient = min / 10;
	uint8_t remain = min % 10;
	
	Set_Number(quotient, 3);
	HAL_Delay(1);
	Set_Number(remain, 4);
	HAL_Delay(1);
}

void Set_Hours(uint8_t hou){
	uint8_t quotient = hou / 10;
	uint8_t remain = hou % 10;
	
	Set_Number(quotient, 1);
	HAL_Delay(1);
	Set_Number(remain, 2);
	HAL_Delay(1);
}

void Set_Colon(void){
	SSD1306_SetCursor(32, 24);
	SSD1306_WriteChar(':', Font_16x32, White);
	  
	SSD1306_UpdateScreen();
}

static void RTC_AlarmConfig(uint8_t Hours, uint8_t Minutes)
{
	RTC_AlarmTypeDef salarmstructure;

  /*##-3- Configure the RTC Alarm peripheral #################################*/
  /* Set Alarm to 02:20:30 
     RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
	salarmstructure.Alarm = RTC_ALARM_A;
	salarmstructure.AlarmTime.Hours = Hours;
	salarmstructure.AlarmTime.Minutes = Minutes;
	salarmstructure.AlarmTime.Seconds = 0;
  
	if(HAL_RTC_SetAlarm_IT(&hrtc,&salarmstructure,RTC_FORMAT_BIN) != HAL_OK)
	{
		/* Initialization Error */
		_Error_Handler(__FILE__, __LINE__); 
	}
}

/**
  * @brief  Alarm callback
  * @param  hrtc : RTC handle
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Turn LED_GREEN on: Alarm generation */
  //BSP_LED_On(LED_GREEN);
	RTC_TimeTypeDef now_time;
	
	Alarm_State = true;
	
	GPIO_SetPin(GPIOC, GPIO_PIN_13);											// Alarm LED ON
	
	int random = rand() % 10000;
	
	password_1 = random / 100;
	password_2 = random % 100;
	
	/* Get the RTC current Time */
	HAL_RTC_GetTime(hrtc, &now_time, RTC_FORMAT_BIN);
	
	ALARM_TIME.Hours = now_time.Hours;
	ALARM_TIME.Minutes = now_time.Minutes;
}

int fputc(int ch, FILE *f){
  uint8_t temp[1] = {ch};
  HAL_UART_Transmit(&huart1, temp, 1, 2);
  return ch;
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	RTC_TimeTypeDef time_instructure;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_TIM4_Init();

  /* USER CODE BEGIN 2 */
	Keypad_Init();																								// Init Keypad
	
	SSD1306_Init();																								// Init SSD1306
	
	SSD1306_Fill(Black);																					// Fill Background of Display by Black
	SSD1306_UpdateScreen();																				// Refresh Display
	
	init_queue();																									// Init Queue
	
	uint8_t old_Seconds = 0;																			
	
	uint8_t set_hour = 0;																					// Hour Parameter
	uint8_t set_minu = 0;																					// Minute Parameter
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Set_Colon();																								// Display Colon 
		
		/* Get the RTC current Time */
		HAL_RTC_GetTime(&hrtc, &time_instructure, RTC_FORMAT_BIN);
		
		// 1 Second
		if(old_Seconds != time_instructure.Seconds){
			old_Seconds = time_instructure.Seconds;
			printf("%02d:%02d:%02d\r\n", time_instructure.Hours, time_instructure.Minutes, old_Seconds);
		}
		
		// Setting Session Button Input Pull Down
		if(HAL_GPIO_ReadPin(BT1_GPIO_Port, BT1_Pin) && !HAL_GPIO_ReadPin(BT2_GPIO_Port, BT2_Pin)){
			while(HAL_GPIO_ReadPin(BT1_GPIO_Port, BT1_Pin) && !HAL_GPIO_ReadPin(BT2_GPIO_Port, BT2_Pin));					// Release Button1
			
			if(Setting_Session == false){															// If Current State wasn't Setting Session
				printf("Set Button 1\r\n");
				
				SSD1306_SetCursor(32, 0);
				SSD1306_WriteString("Setting", Font_7x10, White);				// Display "Setting"
	  
				SSD1306_UpdateScreen();																	// Refresh Display
				
				GPIO_SetPin(LED1_GPIO_Port, LED1_Pin);	 								// LED1 ON
				GPIO_ResetPin(LED2_GPIO_Port, LED2_Pin);								// LED2 OFF
				
				Setting_Session = true;																	// Enter Setting Session
				Alarm_Session = false;																	// Not a Alarm Session
			}
			
			else if(Setting_Session == true){													// If Current State was Setting Session
				printf("Set Button 0\r\n");
				
				SSD1306_SetCursor(32, 0);
				SSD1306_WriteString("       ", Font_7x10, White);				// Clear Diplay Line
	  
				SSD1306_UpdateScreen();																	// Refresh Display
				
				GPIO_ResetPin(LED1_GPIO_Port, LED1_Pin);								// LED1 OFF
				GPIO_ResetPin(LED2_GPIO_Port, LED2_Pin);								// LED2 OFF
				
				Setting_Session = false;																// Leave Setting Session
				Alarm_Session = false;																	// Not a Alarm Session
			}
		}
		
		// Alarm Session Button Input Pull Down
		if(!HAL_GPIO_ReadPin(BT1_GPIO_Port, BT1_Pin) && HAL_GPIO_ReadPin(BT2_GPIO_Port, BT2_Pin)){
			while(!HAL_GPIO_ReadPin(BT1_GPIO_Port, BT1_Pin) && HAL_GPIO_ReadPin(BT2_GPIO_Port, BT2_Pin));					// Release Button2
			
			if(Alarm_Session == false){																// If Current State wasn't Alarm Session
				printf("Set Button 1\r\n");
				
				SSD1306_SetCursor(32, 0);																
				SSD1306_WriteString("Password", Font_7x10, White);			// Display "Password"
	  
				SSD1306_UpdateScreen();																	// Refresh Display
				
				GPIO_ResetPin(LED1_GPIO_Port, LED1_Pin);								// LED1 OFF
				GPIO_SetPin(LED2_GPIO_Port, LED2_Pin);									// LED2 ON
				
				Alarm_Session = true;																		// Enter Alarm Session
				Setting_Session = false;																// Not a Setting Session
			}
			
			else if(Alarm_Session == true){														// If Current State was Alarm Session
				printf("Set Button 0\r\n");
				
				GPIO_ResetPin(LED1_GPIO_Port, LED1_Pin);								// LED1 OFF
				GPIO_SetPin(LED2_GPIO_Port, LED2_Pin);									// LED2 ON
				
				SSD1306_SetCursor(32, 0);
				SSD1306_WriteString("        ", Font_7x10, White);			// Clear Diplay Line
	  
				SSD1306_UpdateScreen();																	// Refresh Display
				
				Alarm_Session = false;																	// Leave Alarm Session
				Setting_Session = false;																// Not a Setting Session
			}
		}
		
		// 
		if(HAL_GPIO_ReadPin(BT1_GPIO_Port, BT1_Pin) && HAL_GPIO_ReadPin(BT2_GPIO_Port, BT2_Pin)){
			while(HAL_GPIO_ReadPin(BT1_GPIO_Port, BT1_Pin) && HAL_GPIO_ReadPin(BT2_GPIO_Port, BT2_Pin));
			
			if(Time_Session == false){
				SSD1306_SetCursor(32, 0);																
				SSD1306_WriteString("Time Setting", Font_7x10, White);	// Display "Time Setting"
				
				SSD1306_UpdateScreen();																	// Refresh Display
					
				// Get Total Data What Inside Of Queue And Put '0'
				for(int i = 0; i < 4; i ++){
					get();																								// Pop
					put('0');																							// Push
				}
				
				Time_Session = true;																		// 
				Alarm_Session = false;																	// Leave Alarm Session
				Setting_Session = false;																// Not a Setting Session
			}
			else if(Time_Session == true){
				SSD1306_SetCursor(32, 0);
				SSD1306_WriteString("            ", Font_7x10, White);			// Clear Diplay Line
	  
				SSD1306_UpdateScreen();																	// Refresh Display
				
				Time_Session = false;																		// 
				Alarm_Session = false;																	// Leave Alarm Session
				Setting_Session = false;																// Not a Setting Session
			}
		}
		
		// Common Clock State
		if(!Setting_Session && !Alarm_State){
			//printf("Set Time\r\n");
			Set_Minutes(time_instructure.Minutes);										// Set Present Minutes
			Set_Hours(time_instructure.Hours);												// Set Present Hours
		}
		
		// Alarm Session. Will Display Password
		else if(Alarm_State){
			SSD1306_SetCursor(32, 0);
			SSD1306_WriteString("Alarm", Font_7x10, White);						// Display "Alarm"
	  
			SSD1306_UpdateScreen();																		// Refresh Display
			
			Set_Hours(password_1);																		// Set Password_1
			Set_Minutes(password_2);																	// Set Password_2
		}
		
		// If Setting Session
		if(Setting_Session && !Alarm_State){
			// Setting Session
			//printf("Setting Session IN\r\n");
			
			// Display Number
			Set_Minutes(set_minu);																		// Set Present Minutes
			Set_Hours(set_hour);																			// Set Present Hours
			
			// Read Input By Keypad
			char Key = GetKey();
			
			if(isdigit(Key)){																					// If Input Number by Keypad
					get();																								// Pop
					put(Key);																							// Push
				
					print_queue();																				// Print Queue
			}
			
			// Reset Number to 00:00
			if(Key == '*'){
				
				// Get Total Data What Inside Of Queue And Put '0'
				for(int i = 0; i < 4; i ++){
					get();																								// Pop
					put('0');																							// Push
				}
				
				// Reset Setting Value
				set_minu = 0;
				set_hour = 0;
				
				print_queue();																					// Print Queue
			}
			
			// Set Alarm Time And Come Back To Clock
			if(Key == '#'){
				// Setting Alarm
				RTC_AlarmConfig(set_hour, set_minu);
				
				printf("Alarm A Set in %02d:%02d\r\n", set_hour, set_minu);
				
				// Get Total Data What Inside Of Queue And Put '0'
				for(int i = 0; i < 4; i ++){
					get();																								// Pop
					put('0');																							// Push
				}
				
				// Reset Setting Value
				set_minu = 0;
				set_hour = 0;
				
				print_queue();																					// Print Queue
				
				GPIO_ResetPin(LED1_GPIO_Port, LED1_Pin);								// LED1 OFF
				GPIO_ResetPin(LED2_GPIO_Port, LED2_Pin);								// LED2 OFF
				
				SSD1306_SetCursor(32, 0);
				SSD1306_WriteString("        ", Font_7x10, White);			// Clear Diplay Line
	  
				SSD1306_UpdateScreen();																	// Refresh Display
				
				Setting_Session = false;																// Leave Setting Session
			}		
			// Change Display Number			
			else {
				// ex) Queue [ 0 | 5 | 3 | 0 ] means 05:30 => set_hour = 05, set_minu = 30 
				set_hour = ((get_queue()[0] - '0') * 10) + (get_queue()[1] - '0');
				set_minu = ((get_queue()[2] - '0') * 10) + (get_queue()[3] - '0');
			}
		}
		
		// When Alarm Callback Event On And Enter the Alarm Session
		if(Alarm_Session && Alarm_State){
			// Alarm Session
			//printf("Alarm Session IN\r\n");
			
			// Read Input By Keypad
			char Key = GetKey();
			
			if(isdigit(Key)){																					// If Input Number by Keypad
					get();																								// Pop
					put(Key);																							// Push
				
					print_queue();																				// Print Queue
			}
			
			// If Password Correct
			if(password_1 == ( ((get_queue()[0] - '0') * 10) + (get_queue()[1] - '0') ) 
				&& password_2 == ( ((get_queue()[2] - '0') * 10) + (get_queue()[3] - '0') )){
				// Alarm Off : All Led Off
				GPIO_ResetPin(GPIOC, GPIO_PIN_13);											// Alarm LED OFF
				
				// Get Total Data What Inside Of Queue And Put '0'
				for(int i = 0; i < 4; i ++){
					get();																								// Pop
					put('0');																							// Push
				}
				
				print_queue();																					// Print Queue	
				
				SSD1306_SetCursor(32, 0);
				SSD1306_WriteString("        ", Font_7x10, White);			// Clear Diplay Line
	  
				SSD1306_UpdateScreen();																	// Refresh Display
				
				Alarm_State = false;																		// Alarm Off
				Alarm_Session = false;																	// Leave Alarm Session
			}
		}
	  
		if(Time_Session){
			char Key = GetKey();
			
			if(isdigit(Key)){																					// If Input Number by Keypad
					get();																								// Pop
					put(Key);																							// Push
				
					print_queue();																				// Print Queue
			}
			
			// Reset Number to 00:00
			if(Key == '*'){
				
				// Get Total Data What Inside Of Queue And Put '0'
				for(int i = 0; i < 4; i ++){
					get();																								// Pop
					put('0');																							// Push
				}
				
				// Reset Setting Value
				set_minu = 0;
				set_hour = 0;
				
				print_queue();																					// Print Queue
			}
			
			// Set Alarm Time And Come Back To Clock
			if(Key == '#'){
				// Setting Time
				RTC_TimeTypeDef set_time_instructure;
		
				time_instructure.Hours = set_hour;  
				time_instructure.Minutes = set_minu;
				time_instructure.Seconds = 0;
	
				HAL_RTC_SetTime(&hrtc, &time_instructure, RTC_FORMAT_BIN);
				
				printf("Time Set in %02d:%02d\r\n", set_hour, set_minu);
				
				// Get Total Data What Inside Of Queue And Put '0'
				for(int i = 0; i < 4; i ++){
					get();																								// Pop
					put('0');																							// Push
				}
				
				// Reset Setting Value
				set_minu = 0;
				set_hour = 0;
				
				print_queue();																					// Print Queue
				
				GPIO_ResetPin(LED1_GPIO_Port, LED1_Pin);								// LED1 OFF
				GPIO_ResetPin(LED2_GPIO_Port, LED2_Pin);								// LED2 OFF
				
				SSD1306_SetCursor(32, 0);
				SSD1306_WriteString("            ", Font_7x10, White);	// Clear Diplay Line
	  
				SSD1306_UpdateScreen();																	// Refresh Display
				
				Time_Session = false;																// Leave Setting Session
			}		
			// Change Display Number			
			else {
				// ex) Queue [ 0 | 5 | 3 | 0 ] means 05:30 => set_hour = 05, set_minu = 30 
				set_hour = ((get_queue()[0] - '0') * 10) + (get_queue()[1] - '0');
				set_minu = ((get_queue()[2] - '0') * 10) + (get_queue()[3] - '0');
			}
		}
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* RTC init function */
static void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef DateToUpdate;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initialize RTC and set the Time and Date 
    */
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2){
  sTime.Hours = 0x1;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x32F2);
  }

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim4);

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED1_Pin|LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin LED2_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BT1_Pin BT2_Pin */
  GPIO_InitStruct.Pin = BT1_Pin|BT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
