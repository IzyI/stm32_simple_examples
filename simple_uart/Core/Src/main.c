/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
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
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define BUFSIZE 3 // здесь указать размер буфера нужного размера
uint8_t buffer[BUFSIZE] = {0,};

// Функция обробатывает IDLE флаг.  Для того чтобы эта функци работала , нужно  добавить  изменения в
// Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_uart.h
// Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c

void HAL_UART_IDLE_Callback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) {
        // Функция вызываетс если буфер не доконца заполнен
        __HAL_UART_DISABLE_IT(&huart1, UART_IT_IDLE);
        uint8_t res = HAL_UART_Transmit_IT(&huart1, (uint8_t *) "ERR_LEN", 7);
        if (res == HAL_ERROR) {
            HAL_UART_Transmit(&huart1, (uint8_t *) "ERR_HAL", 7, 1000);
        } else if (res == HAL_BUSY) {
            HAL_UART_Transmit(&huart1, (uint8_t *) "ERR_BUSY", 8, 1000);
        } else if (res == HAL_OK) {

        } else {
            HAL_UART_Transmit(&huart1, (uint8_t *) "ERR_ALL", 7, 1000);
        }
        HAL_UART_AbortReceive(&huart1);
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
        HAL_UART_Receive_IT(&huart1, (uint8_t *) buffer, BUFSIZE);
    }
}

/////////////////////////////////// полный буфер ///////////////////////////////////////
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) {
        // Функция вызывается если буфер заполнен, здесь мы обрабатываем  и парсим то что пришло
        __HAL_UART_DISABLE_IT(&huart1, UART_IT_IDLE);
        if (strcmp((char *) buffer, "1-0") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
            HAL_UART_Transmit_IT(&huart1, (uint8_t *) "OK", 2);

        } else if (strcmp((char *) buffer, "1-1") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
            HAL_UART_Transmit_IT(&huart1, (uint8_t *) "OK", 2);

        } else if (strcmp((char *) buffer, "2-0") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
            HAL_UART_Transmit_IT(&huart1, (uint8_t *) "OK", 2);

        } else if (strcmp((char *) buffer, "2-1") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
            HAL_UART_Transmit_IT(&huart1, (uint8_t *) "OK", 2);

        } else if (strcmp((char *) buffer, "3-0") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
            HAL_UART_Transmit_IT(&huart1, (uint8_t *) "OK", 2);

        } else if (strcmp((char *) buffer, "3-1") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
            HAL_UART_Transmit_IT(&huart1, (uint8_t *) "OK", 2);

        } else if (strcmp((char *) buffer, "4-0") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
            HAL_UART_Transmit_IT(&huart1, (uint8_t *) "OK", 2);

        } else if (strcmp((char *) buffer, "4-1") == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
            HAL_UART_Transmit_IT(&huart1, (uint8_t *) "OK", 2);

        } else {
            HAL_UART_Transmit_IT(&huart1, (uint8_t *) "NOT", 3);
        }

        HAL_UART_AbortReceive(&huart1);
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
        HAL_UART_Receive_IT(&huart1, (uint8_t *) buffer, BUFSIZE);
    }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    // Здесь мы обрабатываем ошибку  отправки UART
    if (huart == &huart1) {
        __HAL_UART_DISABLE_IT(&huart1, UART_IT_IDLE);
        uint32_t er = HAL_UART_GetError(&huart1);
        HAL_UART_Abort_IT(&huart1);

        switch (er) {
            case HAL_UART_ERROR_PE:
                HAL_UART_Transmit(&huart1, (uint8_t *) "ERR_PARITY", 10, 1000);
                __HAL_UART_CLEAR_PEFLAG(&huart1);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;

            case HAL_UART_ERROR_NE:
                HAL_UART_Transmit(&huart1, (uint8_t *) "ERR_NOISE", 9, 1000);
                __HAL_UART_CLEAR_NEFLAG(&huart1);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;

            case HAL_UART_ERROR_FE:
                HAL_UART_Transmit(&huart1, (uint8_t *) "ERR_FRAME", 9, 1000);
                __HAL_UART_CLEAR_FEFLAG(&huart1);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;

            case HAL_UART_ERROR_ORE:
                HAL_UART_Transmit(&huart1, (uint8_t *) "ERR_OVERRUN", 11, 1000);
                __HAL_UART_CLEAR_OREFLAG(huart);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;

            case HAL_UART_ERROR_DMA:
                HAL_UART_Transmit(&huart1, (uint8_t *) "ERR_DMA", 7, 1000);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
                break;
            default:
                break;
        }
    }

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
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
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */

    // Включаем флаг IDLE
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    // Включаем прерывание
    HAL_UART_Receive_IT(&huart1, (uint8_t *) buffer, BUFSIZE);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        // Здесь просто мигаем светодиодом
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(1000);


    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void) {

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
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : PA1 PA2 PA3 PA4 */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
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
