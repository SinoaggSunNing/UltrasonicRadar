
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "delay.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
DMA_HandleTypeDef hdma_tim3_ch1_trig;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void TIM3_CH1_Cap_Init(uint32_t arr,uint16_t psc);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint8_t TIM3CH1_CAPTURE_STA=0;//���벶��״̬
uint32_t TIM3CH1_CAPTURE_VAL=0; //���벶��ֵ
	uint8_t Probe_L=0;// ��̽ͷ����
	uint8_t Probe_LM=0;
	uint8_t Probe_R=0;
	uint8_t Probe_RM=0;
	uint8_t Data_Bit[19]={0};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	uint32_t temp=0;	//�͵�ƽ����
	uint8_t bit=0;
	uint8_t i=0;

	uint8_t beep=0x11;
	uint8_t ChexkBit=0;
	uint32_t Data_Time_Buf[19]={0};
	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	delay_init(72);	
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);//����TIM3�Ĳ���ͨ��1�����ҿ��������ж�
	__HAL_TIM_ENABLE_IT(&htim3,TIM_IT_UPDATE); //ʹ�ܸ����ж�
//	TIM3_CH1_Cap_Init(0XFFFF,72-1);		//��1Mhz��Ƶ�ʼ���
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		HAL_TIM_IC_Start_DMA(&htim3,TIM_CHANNEL_1,Data_Time_Buf,19);
		//Delay_ms(500);
		if(TIM3CH1_CAPTURE_STA&0x80)//�ɹ���׽��������
		{
			temp=TIM3CH1_CAPTURE_STA&0X3F;
			temp*=50; //���ʱ���ܺ�
			temp+=TIM3CH1_CAPTURE_VAL; //�õ��ܵĵ͵�ƽʱ��
			if(temp>1449&&temp<1551)
			{
				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
				TIM3CH1_CAPTURE_STA=0; //������һ�β���
				for(i=0;i<19;)
				{
					if(TIM3CH1_CAPTURE_STA&0x80)
					{
						temp=TIM3CH1_CAPTURE_STA&0X3F;
						temp*=50; //���ʱ���ܺ�
						temp+=TIM3CH1_CAPTURE_VAL; //�õ��ܵĵ͵�ƽʱ��
						Data_Time_Buf[i]=temp;
						i++;
						TIM3CH1_CAPTURE_STA=0; //������һ�β���
					}
				}
				ChexkBit=0;
				for(i=0;i<19;i++)
				{
					if(*(Data_Time_Buf+i)>125)
						Data_Bit[i]=0;
					else
						Data_Bit[i]=1;
					ChexkBit+=Data_Bit[i];
				}
				if(ChexkBit%2==Data_Bit[18])
				{
					Probe_L=(Data_Bit[0]|Data_Bit[1]<<1|Data_Bit[2]<<2|Data_Bit[3]<<3)*20+20;
					Probe_LM=(Data_Bit[4]|Data_Bit[5]<<1|Data_Bit[6]<<2|Data_Bit[7]<<3)*20+20;
					Probe_RM=(Data_Bit[8]|Data_Bit[9]<<1|Data_Bit[10]<<2|Data_Bit[11]<<3)*20+20;
					Probe_R=(Data_Bit[12]|Data_Bit[13]<<1|Data_Bit[14]<<2|Data_Bit[15]<<3)*20+20;
				}
			}
 			TIM3CH1_CAPTURE_STA=0; //������һ�β���
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
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

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 60;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_IC_InitTypeDef sConfigIC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 50;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void TIM3_CH1_Cap_Init(uint32_t arr,uint16_t psc)
{  
    TIM_IC_InitTypeDef TIM2_CH2Config;  
    
    htim3.Instance=TIM2;                          //ͨ�ö�ʱ��2
    htim3.Init.Prescaler=psc;                     //��Ƶϵ��
    htim3.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    htim3.Init.Period=arr;                        //�Զ�װ��ֵ
    htim3.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_IC_Init(&htim3);						//��ʼ�����벶��ʱ������
    
    TIM2_CH2Config.ICPolarity=TIM_ICPOLARITY_RISING;    //�����ز���
    TIM2_CH2Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM2_CH2Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM2_CH2Config.ICFilter=0;                          //���������˲��������˲�
    HAL_TIM_IC_ConfigChannel(&htim3,&TIM2_CH2Config,TIM_CHANNEL_1);//����TIM2ͨ��2
	
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);   //����TIM2�Ĳ���ͨ��2�����ҿ��������ж�
    __HAL_TIM_ENABLE_IT(&htim3,TIM_IT_UPDATE);   //ʹ�ܸ����ж�
	
	HAL_NVIC_SetPriority(TIM2_IRQn,2,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM2_IRQn);          //����ITM2�ж�ͨ��  
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM2_CLK_ENABLE();            //ʹ��TIM2ʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_4;            //PB4
    GPIO_Initure.Mode=GPIO_MODE_AF_INPUT; 	//������������
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM3_IRQn,2,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM2�ж�ͨ��  
}

//��ʱ��5�жϷ�����
//void TIM3_IRQHandler(void)
//{
//	HAL_TIM_IRQHandler(&htim3);				//��ʱ�����ô�����
//}
 
//��ʱ�������жϣ�����������жϴ���ص������� �ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//�����жϣ����������ʱִ��
{
	if((TIM3CH1_CAPTURE_STA&0X80)==0)				//��δ�ɹ�����
	{
		if(TIM3CH1_CAPTURE_STA&0X40)				//�Ѿ����񵽵͵�ƽ��
		{
				TIM3CH1_CAPTURE_STA++;
		}	 
	}		
}

//��ʱ�����벶���жϴ���ص��������ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
	if((TIM3CH1_CAPTURE_STA&0X80)==0)				//��δ�ɹ�����
	{
		if(TIM3CH1_CAPTURE_STA&0X40)				//����һ�������� 		
		{	  			
			TIM3CH1_CAPTURE_STA|=0X80;				//��ǳɹ�����һ�θߵ�ƽ����
      TIM3CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1);//��ȡ��ǰ�Ĳ���ֵ.
			TIM_RESET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1);   //һ��Ҫ�����ԭ�������ã���
      TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//����TIM5ͨ��1�½��ز���
		}
		else  										//��δ��ʼ,��һ�β����½���
		{
			TIM3CH1_CAPTURE_STA=0;					//���
			TIM3CH1_CAPTURE_VAL=0;
			TIM3CH1_CAPTURE_STA|=0X40;				//��ǲ������½���
			__HAL_TIM_DISABLE(&htim3);      	//�رն�ʱ��2
			__HAL_TIM_SET_COUNTER(&htim3,0);
			TIM_RESET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1);   //һ��Ҫ�����ԭ�������ã���
			TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//��ʱ��5ͨ��1����Ϊ�����ز���
			__HAL_TIM_ENABLE(&htim3);		//ʹ�ܶ�ʱ��2
		}		    
	}		
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
