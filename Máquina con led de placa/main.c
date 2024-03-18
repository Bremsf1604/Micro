/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ESTADO_ERROR        0
#define ESTADO_ABIERTO      1
#define ESTADO_CERRADO      2
#define ESTADO_ABRIENDO     3
#define ESTADO_CERRANDO     4
#define ESTADO_INTERMEDIO   5
#define ESTADO_INIT         6
#define FALSE 				0
#define TRUE 				1
#define TIME_Ca 			60
#define PUERTO 				GPIOA
#define BTN_ABRIR 			GPIO_PIN_0
#define BTN_CERRAR 			GPIO_PIN_1
#define SW_ABRIR 			GPIO_PIN_4
#define SW_CERRAR 			GPIO_PIN_8
#define MOTOR_a 			GPIO_PIN_6
#define MOTOR_c 			GPIO_PIN_7
#define Led_Pin				GPIO_PIN_5

//Definiciones de los tiempos del led

#define LED_ON          1
#define LED_OFF         0
#define Inter_ON        1
#define Inter_OFF       0
#define Inter_RAPIDO    20//Definicion del tiempo rapido multiplo de 10ms
#define Inter_LENTO     40 //Definicion del tiempo lento multiplo de 10ms

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
int Func_ESTADO_ERROR(void);
int Func_ESTADO_ABIERTO(void);
int Func_ESTADO_CERRADO(void);
int Func_ESTADO_ABRIENDO(void);
int Func_ESTADO_CERRANDO(void);
int Func_ESTADO_INTERMEDIO(void);
int Func_ESTADO_INIT(void);
int Func_SetLed(int, int);

volatile int ESTADO_ANTERIOR = ESTADO_INIT;
volatile int ESTADO_ACTUAL = ESTADO_INIT;
volatile int ESTADO_SIGUIENTE = ESTADO_INIT;
volatile int CntTimeCa = 0; //Contador de un segundo

volatile struct INOUT
{
    unsigned int Sc:1;
    unsigned int Sa:1;
    unsigned int Mc:1;
    unsigned int Ma:1;
    unsigned int Bc:1;
    unsigned int Ba:1;
    unsigned int Led:1;
} inout;

volatile struct LED
{
    int unsigned StatusLED1:1;
    int unsigned InterLED1;
}LedStatus;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      if(ESTADO_SIGUIENTE == ESTADO_INIT)
      {
          ESTADO_SIGUIENTE = Func_ESTADO_INIT();
      }
      if(ESTADO_SIGUIENTE == ESTADO_ABIERTO)
      {
          ESTADO_SIGUIENTE = Func_ESTADO_ABIERTO();
      }
      if(ESTADO_SIGUIENTE == ESTADO_CERRADO)
      {
          ESTADO_SIGUIENTE = Func_ESTADO_CERRADO();
      }
      if(ESTADO_SIGUIENTE == ESTADO_ABRIENDO)
      {
          ESTADO_SIGUIENTE = Func_ESTADO_ABRIENDO();
      }
      if(ESTADO_SIGUIENTE == ESTADO_CERRANDO)
      {
          ESTADO_SIGUIENTE = Func_ESTADO_CERRANDO();
      }
      if(ESTADO_SIGUIENTE == ESTADO_INTERMEDIO)
      {
          ESTADO_SIGUIENTE = Func_ESTADO_INTERMEDIO();
      }
      if(ESTADO_SIGUIENTE == ESTADO_ERROR)
      {
          ESTADO_SIGUIENTE = Func_ESTADO_ERROR();
      }
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 80;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5_Pin|GPIO_PIN_6_Pin|GPIO_PIN_7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = GPIO_PIN_0_Pin|GPIO_PIN_1_Pin|GPIO_PIN_4_Pin|GPIO_PIN_8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = GPIO_PIN_5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO_PIN_5_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = GPIO_PIN_6_Pin|GPIO_PIN_7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

int Func_SetLed(int St, int Inter)
{
    LedStatus.StatusLED1 = St;
    LedStatus.InterLED1 = Inter;
    return 0;
}

int Func_ESTADO_ERROR(void)
{
    for(;;)
    {
        inout.Led = TRUE;
    }
}

int Func_ESTADO_ABIERTO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ABIERTO;
    inout.Ma = FALSE;
    inout.Mc = FALSE;
    CntTimeCa = 0;//reset de contador
    Func_SetLed(LED_ON,Inter_RAPIDO);

    for(;;)
    {
        if(CntTimeCa > TIME_Ca)
        {
            return ESTADO_CERRANDO;
        }

        if(inout.Bc == TRUE)
        {
            return ESTADO_CERRANDO;
        }
    }
}

int Func_ESTADO_CERRADO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_CERRADO;
    inout.Mc = FALSE;
    inout.Ma = FALSE;
    Func_SetLed(LED_OFF,Inter_OFF);

    for(;;)
    {
        if(inout.Ba == TRUE)
        {
            return ESTADO_ABRIENDO;
        }
    }
}

int Func_ESTADO_ABRIENDO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ABRIENDO;
    inout.Mc = FALSE;
    inout.Ma = TRUE;
    Func_SetLed(LED_ON,Inter_LENTO);

    for(;;)
    {
        if(inout.Sa == TRUE)
        {
            return ESTADO_ABIERTO;
        }
    }
}

int Func_ESTADO_CERRANDO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_CERRANDO;
    inout.Mc = TRUE;
    inout.Ma = FALSE;
    Func_SetLed(LED_ON,Inter_LENTO);

    for(;;)
    {
        if(inout.Sc == TRUE)
        {
            return ESTADO_CERRADO;
        }
    }
}

int Func_ESTADO_INTERMEDIO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_INTERMEDIO;
    inout.Mc = FALSE;
    inout.Ma = FALSE;

    for(;;)
    {
        if(inout.Bc == TRUE)
        {
           return ESTADO_CERRANDO;
        }
        if(inout.Ba == TRUE)
        {
           return ESTADO_ABRIENDO;
        }
    }
}

int Func_ESTADO_INIT(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_INIT;

    inout.Ma = FALSE;
    inout.Mc = FALSE;

    if((inout.Sa == TRUE) && (inout.Sc == TRUE))
    {
        return ESTADO_ERROR;
    }
    if(inout.Sa == TRUE)
    {
        return ESTADO_ABIERTO;
    }
    if(inout.Sc == TRUE)
    {
        return ESTADO_CERRADO;
    }
    if((inout.Sa == FALSE) && (inout.Sc == FALSE))
    {
        return ESTADO_INTERMEDIO;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim2)
{
	static int cont_int = 0;
	    cont_int++;

	    if(cont_int == 9)
	    {
	        CntTimeCa++;
	        cont_int = 0;
	    }

	    if(HAL_GPIO_ReadPin(PUERTO, BTN_CERRAR)== TRUE)//leg
	    {
	        inout.Bc = TRUE;
	    }
	    else
	    {
	        inout.Bc = FALSE;
	    }

	    if(HAL_GPIO_ReadPin(PUERTO, BTN_ABRIR)== TRUE)//leg
	    {
	        inout.Ba = TRUE;
	    }
	    else
	    {
	        inout.Ba = FALSE;
	    }

	    if(HAL_GPIO_ReadPin(PUERTO, SW_CERRAR)== TRUE)//leg
	    {
	        inout.Sc = TRUE;
	    }
	    else
	    {
	        inout.Sc = FALSE;
	    }

	    if(HAL_GPIO_ReadPin(PUERTO, SW_ABRIR)== TRUE)//leg
	    {
	        inout.Sa = TRUE;
	    }
	    else
	    {
	        inout.Sa = FALSE;
	    }

	    if(inout.Ma == TRUE)//leg
	    {
	        HAL_GPIO_WritePin(PUERTO, MOTOR_a,TRUE);
	    }
	    else
	    {
	        HAL_GPIO_WritePin(PUERTO, MOTOR_a,FALSE);
	    }

	    if(inout.Mc == TRUE)//leg
	    {
	        HAL_GPIO_WritePin(PUERTO, MOTOR_c,TRUE);
	    }
	    else
	    {
	        HAL_GPIO_WritePin(PUERTO, MOTOR_c,FALSE);
	    }

	    static unsigned int cont_led = 0;
	    cont_led ++;

	    if (LedStatus.StatusLED1 == LED_ON)
	    {
	        if(cont_led >= LedStatus.InterLED1)
	        {
	            if(HAL_GPIO_ReadPin(PUERTO,Led_Pin) == LED_ON)
	            {
	                HAL_GPIO_WritePin(PUERTO,Led_Pin,FALSE);//Negacion del led
	            }
	            else
	            {
	            	HAL_GPIO_WritePin(PUERTO,Led_Pin,TRUE);
	            }
	            cont_led = 0;
	        }
	    }
	    else
	    {
	        HAL_GPIO_WritePin(PUERTO,Led_Pin, FALSE);
	        cont_led = 0;
	    }
}

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
