/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdint.h>
#include"gpio.h"
#include"timer.h"
#include"adc.h"
#include"Interrupt.h"
#include"oled.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEMP_ALARM_X10     500                     /* 50.0 C, in tenths of a degree      */
#define LDR_DIM_PCT        30                      /* below this  -> DIM    -> light ON  */
#define LDR_BRIGHT_PCT     40                      /* above this  -> BRIGHT -> light OFF */
#define LOOP_PERIOD_MS     200                     /* one pass of the while loop         */
#define EMERGENCY_TICKS    (3000 / LOOP_PERIOD_MS) /* buzzer stays on 3 s after button   */
#define SCREEN_TICKS       (3000 / LOOP_PERIOD_MS) /* OLED switches screen every 3 s     */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ADC_CHANNEL_LM35D       0U
#define ADC_CHANNEL_LDR       	1U

#define ADC_VREF                3.3f
#define ADC_MAX_VALUE           4095.0f

/*
 * LM35D sensitivity:
 *
 * 10 mV / °C
 * 0.010 V / °C
 */
#define LM35D_SENSITIVITY       0.010f
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
 * Temperature to motor speed (percent):
 *   <= 50.0 C : 0 %          (motor off)
 *   51 C      : about 32 %   (low)
 *   60 C      : about 53 %   (medium)
 *   70 C      : about 76 %   (higher)
 *   >= 80 C   : 100 %        (maximum)
 * Linear between 50 C and 80 C, starting at 30 %.
 */
static uint8_t Motor_Duty_From_Temp(int16_t t10)
{
    if (t10 <= 500) return 0;
    if (t10 >= 800) return 100;
    return (uint8_t)(30 + ((t10 - 500) * 70) / 300);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  GPIO_Init();
  TIMER2_Init();
  PWM_Init();
  ADC_Init();
  INTERRUPT_Init();
  Buzzer_Init();
  I2C1_Init();                 /* must come before OLED_Init(): sets up PB8/PB9 and I2C1 */
  TIMER2_Delay_ms(100);        /* let the OLED power up */
  OLED_Init();
  OLED_Clear();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  SystemStatus st = {0};              /* all fields start at 0 */
  uint8_t screen = 0;
  uint8_t screen_count = 0;
  uint8_t emergency_ticks = 0;

  st.light = 2;                       /* start as BRIGHT so the light is OFF at power-up */

  while (1)
  {
      /* ---- 1. Read both sensors (average 8 samples to reduce noise) ---- */
      /* LM35D OUT -> PA0, LDR divider -> PA1 */
      uint32_t lm35_sum = 0;
      uint32_t ldr_sum  = 0;
      for (uint8_t i = 0; i < 8; i++)
      {
          lm35_sum += ADC_Read(ADC_CHANNEL_LM35D);
          ldr_sum  += ADC_Read(ADC_CHANNEL_LDR);
      }
      uint16_t lm35_raw = (uint16_t)(lm35_sum / 8);
      uint16_t ldr_raw  = (uint16_t)(ldr_sum  / 8);

      /* ---- 2. Temperature: ADC -> voltage -> degrees C ----
       *     Voltage     = ADC_Value / 4095 x VREF
       *     Temperature = Voltage / 0.010   (LM35D: 10 mV = 1 C)
       */
      float voltage     = ((float)lm35_raw * ADC_VREF) / ADC_MAX_VALUE;
      float temperature = voltage / LM35D_SENSITIVITY;
      st.temp_x10 = (int16_t)(temperature * 10.0f + 0.5f);     /* 52.4 C -> 524 */

      /* ---- 3. Light: dim -> home light ON (with hysteresis) ---- */
      uint32_t light_pct = (ldr_raw * 100u) / 4095u;
      if      (light_pct < LDR_DIM_PCT)    st.light = 1;       /* DIM    */
      else if (light_pct > LDR_BRIGHT_PCT) st.light = 2;       /* BRIGHT */
      /* between the two thresholds: keep the previous state */

      st.home_light = (st.light == 1);
      GPIO_LED_ON_OFF(st.home_light);

      /* ---- 4. Over-temperature: motor + PWM speed ---- */
      uint8_t over_temp = (st.temp_x10 > TEMP_ALARM_X10);
      st.motor_speed = Motor_Duty_From_Temp(st.temp_x10);
      st.motor       = over_temp;
      st.cooling     = over_temp;
      PWM_Motor_Speed_Control(st.motor_speed * 10u);           /* function takes 0 to 1000 */

      /* ---- 5. Emergency button (the ISR already switched the buzzer on) ---- */
      if (button_flag)
      {
    	  TIMER2_Delay_ms(20);                    // debounce
		  Buzzer_On();
		  TIMER2_Delay_ms(2000);               // beep for 0.5 s
		  Buzzer_Off();
          emergency_ticks = EMERGENCY_TICKS;
          button_flag = 0;
      }
      else if (emergency_ticks > 0)
      {
          emergency_ticks--;
      }

      /* ---- 6. Buzzer: on if over-temperature OR emergency ---- */
      st.buzzer = (over_temp || (emergency_ticks > 0));
      if (st.buzzer) Buzzer_On();
      else           Buzzer_Off();

      /* ---- 7. OLED: alternate the two screens every 3 s ---- */
      if (++screen_count >= SCREEN_TICKS)
      {
          screen_count = 0;
          screen ^= 1;
      }
      OLED_Update(&st, screen);

      /* ---- 8. UART status (add when your UART driver is ready) ---- */
      /* UART_Send_Status(&st); */

      TIMER2_Delay_ms(LOOP_PERIOD_MS);

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
