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
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
//#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "queue.h"
#include "mp3.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define STX	0x02
#define ETX	0x03

#define BASE_VOLUME 10

#define SEG_PORT GPIOE

uint32_t Number_Pin[7] = { 
	SEG_A_Pin,
	SEG_B_Pin,
	SEG_C_Pin,
	SEG_D_Pin,
	SEG_E_Pin,
	SEG_F_Pin,
	SEG_G_Pin
};

#define _0 { 0, 0, 0, 0, 0, 0, 1 }
#define _1 { 1, 0, 0, 1, 1, 1, 1 }
#define _2 { 0, 0, 1, 0, 0, 1, 0 }
#define _3 { 0, 0, 0, 0, 1, 1, 0 }
#define _4 { 1, 0, 0, 1, 1, 0, 0 }
#define _5 { 0, 1, 0, 0, 1, 0, 0 }
#define _6 { 0, 1, 0, 0, 0, 0, 0 }
#define _7 { 0, 0, 0, 1, 1, 1, 1 }
#define _8 { 0, 0, 0, 0, 0, 0, 0 }
#define _9 { 0, 0, 0, 0, 1, 0, 0 }

uint8_t Number[10][7] = { _0, _1, _2, _3, _4, _5, _6, _7, _8, _9 };

uint8_t aShowTime[50] = {0};

uint8_t second_count = 0;
uint8_t minute_count = 0;
uint8_t hour_count = 0;

uint8_t Buffer[6];

bool Alarm_State = false;
bool Setting_Session = false;
bool Alarm_Session = false;

uint8_t password_1 = 0;
uint8_t password_2 = 0;

uint8_t Keys[4][4] = {
	'1', '2', '3', 'A',
	'4', '5', '6', 'B',
	'7', '8', '9', 'C',
	'*', '0', '#', 'D'
};

uint8_t Key = 0;

#define ROW_PORT GPIOD
#define COL_PORT GPIOD

#define ROWS 4
#define COLS 4

// OUTPUT
uint8_t Row_Pin[4] = {
	R1_Pin,
	R2_Pin,
	R3_Pin,
	R4_Pin
};

// INPUT
uint8_t Column_Pin[4] = {
	C1_Pin,
	C2_Pin,
	C3_Pin,
	C4_Pin
};

RTC_TimeTypeDef ALARM_TIME;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void Set_Minutes(uint8_t min);
void Set_Hours(uint8_t hou);
void Set_Number(uint8_t num, uint8_t dig);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void Set_Number(uint8_t num, uint8_t dig){	
	switch(dig){
		case 1:
			HAL_GPIO_WritePin(DIGIT_1_GPIO_Port, DIGIT_1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(DIGIT_2_GPIO_Port, DIGIT_2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DIGIT_3_GPIO_Port, DIGIT_3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DIGIT_4_GPIO_Port, DIGIT_4_Pin, GPIO_PIN_RESET);
			for(int i = 0; i < 7; i++)
				HAL_GPIO_WritePin(SEG_PORT, Number_Pin[i], Number[num][i]);
			break;
		case 2:
			HAL_GPIO_WritePin(DIGIT_1_GPIO_Port, DIGIT_1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DIGIT_2_GPIO_Port, DIGIT_2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(DIGIT_3_GPIO_Port, DIGIT_3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DIGIT_4_GPIO_Port, DIGIT_4_Pin, GPIO_PIN_RESET);
			for(int i = 0; i < 7; i++)
				HAL_GPIO_WritePin(SEG_PORT, Number_Pin[i], Number[num][i]);
			break;
		case 3:
			HAL_GPIO_WritePin(DIGIT_1_GPIO_Port, DIGIT_1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DIGIT_2_GPIO_Port, DIGIT_2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DIGIT_3_GPIO_Port, DIGIT_3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(DIGIT_4_GPIO_Port, DIGIT_4_Pin, GPIO_PIN_RESET);
			for(int i = 0; i < 7; i++)
				HAL_GPIO_WritePin(SEG_PORT, Number_Pin[i], Number[num][i]);
			break;
		case 4:
			HAL_GPIO_WritePin(DIGIT_1_GPIO_Port, DIGIT_1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DIGIT_2_GPIO_Port, DIGIT_2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DIGIT_3_GPIO_Port, DIGIT_3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DIGIT_4_GPIO_Port, DIGIT_4_Pin, GPIO_PIN_SET);
			for(int i = 0; i < 7; i++)
				HAL_GPIO_WritePin(SEG_PORT, Number_Pin[i], Number[num][i]);
			break;
	}
}

void Set_Minutes(uint8_t min){
	uint8_t quotient = min / 10;
	uint8_t remain = min % 10;
	
	//printf("Minute_1 : %d\r\n", quotient);
	//printf("Minute_2 : %d\r\n", remain);
	
    Set_Number(quotient, 3);
	HAL_Delay(1);
    Set_Number(remain, 4);
	HAL_Delay(1);
}

void Set_Hours(uint8_t hou){
	uint8_t quotient = hou / 10;
	uint8_t remain = hou % 10;
	
	//printf("Hour_1 : %d\r\n", quotient);
	//printf("Hour_2 : %d\r\n", remain);
	
    Set_Number(quotient, 1);
	HAL_Delay(1);
    Set_Number(remain, 2);
	HAL_Delay(1);
}

void DP_Blink(){
	//HAL_GPIO_WritePin(DIGIT_DP_GPIO_Port, DIGIT_DP_Pin, 1);
	HAL_GPIO_TogglePin(DIGIT_DP_GPIO_Port, DIGIT_DP_Pin);
}

void Calculate_Time(){
	minute_count = minute_count + second_count / 60;
	hour_count = hour_count + minute_count / 60;
	
	if(hour_count == 24)
		hour_count = 0;
	if(minute_count == 60)
		minute_count = 0;
	if(second_count == 60)
		second_count = 0;
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
	
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	
	int random = rand() % 10000;
	
	password_1 = random / 100;
	password_2 = random % 100;
	
	/* Get the RTC current Time */
	HAL_RTC_GetTime(hrtc, &now_time, RTC_FORMAT_BIN);
	
	ALARM_TIME.Hours = now_time.Hours;
	ALARM_TIME.Minutes = now_time.Minutes;
}

void getKey(){
	for(uint8_t i = 0; i < ROWS; i++){
		HAL_GPIO_WritePin(ROW_PORT, Row_Pin[i], GPIO_PIN_RESET);
		for(uint8_t j = 0; j < COLS; j++){
			if(!HAL_GPIO_ReadPin(COL_PORT, Column_Pin[j]))
			{
				//printf("C%d, R%d IN\r\n", (j + 1), (i + 1));
				while(!HAL_GPIO_ReadPin(COL_PORT, Column_Pin[j]));
				Key = Keys[i][j];
				
				if(isdigit(Key)){
					get();
					put(Key);
				
					print_queue();
				}
				//printf("%s\r\n", get_queue());
			}
		}
		HAL_GPIO_WritePin(ROW_PORT, Row_Pin[i], GPIO_PIN_SET);
	}
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();

  /* USER CODE BEGIN 2 */
	uint8_t old_Seconds = 0;
	uint8_t count = 0;
	
	uint8_t old_Key = 0;
	
	uint8_t set_hour = 0;
	uint8_t set_minu = 0;
	
	//delete uint8_t alarm_count = 0;
	
	uint8_t Volume = BASE_VOLUME;
	
	for(uint8_t i = 0; i < ROWS; i++){
		HAL_GPIO_WritePin(ROW_PORT, Row_Pin[i], GPIO_PIN_SET);
	}
	
	init_queue();	// Init Queue
	
	mp3_set_volume(Volume);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		/* Get the RTC current Time */
		HAL_RTC_GetTime(&hrtc, &time_instructure, RTC_FORMAT_BIN);
		
		count++;	// time count for DP Blink. 1 Second => 124 or 123, 500 MilliSeconds is APPROX 62.
		
		// 1 Second
		if(old_Seconds != time_instructure.Seconds){
			old_Seconds = time_instructure.Seconds;
			printf("%02d:%02d:%02d\r\n", time_instructure.Hours, time_instructure.Minutes, old_Seconds);
			//printf("Count : %d\r\n", count);
			DP_Blink();		// DP Blink
			count = 0;		// time count reset
		}
		
		// 500 MilliSeconds
		if(count == 62){
			DP_Blink();		// DP Blink
		}
		
		if(time_instructure.Hours == ALARM_TIME.Hours && time_instructure.Minutes - ALARM_TIME.Minutes > 0){
			// Volume 10Points UP  
			if(Volume == BASE_VOLUME){
				Volume += 10;
				
				mp3_set_volume(Volume);
			}
		}
		
		// Setting Session Button Input
		if(!HAL_GPIO_ReadPin(Touch_1_GPIO_Port, Touch_1_Pin)){
			while(!HAL_GPIO_ReadPin(Touch_1_GPIO_Port, Touch_1_Pin));
			if(Setting_Session == false){
				printf("Set Button 1\r\n");
				
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
				
				Setting_Session = true;
				Alarm_Session = false;
			}
			else if(Setting_Session == true){
				printf("Set Button 0\r\n");
				
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
				
				Setting_Session = false;
			}
		}
		
		// Alarm Session Button Input
		if(!HAL_GPIO_ReadPin(Touch_2_GPIO_Port, Touch_2_Pin)){
			while(!HAL_GPIO_ReadPin(Touch_1_GPIO_Port, Touch_1_Pin));
			if(Alarm_Session == false){
				printf("Set Button 1\r\n");
				
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
				
				Alarm_Session = true;
				Setting_Session = false;
			}
		}
		
		// Common Clock State
		if(!Setting_Session && !Alarm_State){
			//printf("Set Time\r\n");
			Set_Minutes(time_instructure.Minutes);
			Set_Hours(time_instructure.Hours);
		}
		
		// Alarm Session. Will Display Password
		else if(Alarm_State){
			Set_Minutes(password_2);
			Set_Hours(password_1);
		}
		
		// If Setting Session
		if(Setting_Session && !Alarm_State){
			// Setting Session
			//printf("Setting Session IN\r\n");
			
			// Display Number
			Set_Minutes(set_minu);
			Set_Hours(set_hour);
			
			// Read Input By Keypad
			getKey();
		
			if(old_Key != Key){
				old_Key = Key;
				printf("%c IN\r\n", Key);
			}
			
			// Reset Number to 00:00
			if(old_Key == '*'){
				
				// Get Total Data What Inside Of Queue And Put '0'
				for(int i = 0; i < 4; i ++){
					get();
					put('0');	
				}
				
				// Reset Setting Value
				set_minu = 0;
				set_hour = 0;
				
				print_queue();
				
				Key = '0';
			}
			
			// Set Alarm Time And Come Back To Clock
			if(old_Key == '#'){
				// Setting Alarm
				RTC_AlarmConfig(set_hour, set_minu);
				
				printf("Alarm A Set in %02d:%02d\r\n", set_hour, set_minu);
				
				// Get Total Data What Inside Of Queue And Put '0'
				for(int i = 0; i < 4; i ++){
					get();
					put('0');
				}
				
				// Reset Setting Value
				set_minu = 0;
				set_hour = 0;
				
				print_queue();
				
				Key = '0';
				
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
				
				Setting_Session = false;
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
			getKey();
		
			if(old_Key != Key){
				old_Key = Key;
				printf("%c IN\r\n", Key);
			}
			
			if(password_1 == ( ((get_queue()[0] - '0') * 10) + (get_queue()[1] - '0') ) && password_2 == ( ((get_queue()[2] - '0') * 10) + (get_queue()[3] - '0') )){
				// Alarm Off
				HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
				
				// Get Total Data What Inside Of Queue And Put '0'
				for(int i = 0; i < 4; i ++){
					get();
					put('0');
				}
				
				print_queue();
			
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
				
				Alarm_State = false;
				Alarm_Session = false;
			}
		}
		
		// ENTER ALARM SETTING PAGE

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		//HAL_UART_Receive_IT(&huart1, Buffer, 6);
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

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	if(huart->Instance == USART1){
//		uint8_t hours = ((Buffer[0] - 0x30) * 10) + (Buffer[1] - 0x30);
//		uint8_t minutes = ((Buffer[2] - 0x30) * 10) + (Buffer[3] - 0x30);
//		uint8_t seconds = ((Buffer[4] - 0x30) * 10) + (Buffer[5] - 0x30);
//		
//		RTC_TimeTypeDef time_instructure;
//		
//		time_instructure.Hours = hours;
//		time_instructure.Minutes = minutes;
//		time_instructure.Seconds = seconds;
//	
//		HAL_RTC_SetTime(&hrtc, &time_instructure, RTC_FORMAT_BIN);
//	}
}
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
