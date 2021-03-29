#include "main.h"
#include "stm32f1xx.h"


void delay(uint32_t time_delay) {
    uint32_t i;
    for (i = 0; i < time_delay; i++);
}


//Настраиваем тактирование системы от внешнего кварца
//через PLL на саксимально возможных частотах.
//Внешний кварц должен быть на 8МГц
//Возвращает:
//  0 - завершено успешно
//  1 - не запустился кварцевый генератор
//  2 - не запустился PLL
int SetSysClockTo72(void) {
    __IO int StartUpCounter;

    ////////////////////////////////////////////////////////////
    //Запускаем кварцевый генератор
    ////////////////////////////////////////////////////////////

    RCC->CR |= RCC_CR_HSEON; //Запускаем генератор HSE

    //Ждем успешного запуска или окончания тайм-аута
    for (StartUpCounter = 0;; StartUpCounter++) {
        //Если успешно запустилось, то
        //выходим из цикла
        if (RCC->CR & RCC_CR_HSERDY)
            break;

        //Если не запустилось, то
        //отключаем все, что включили
        //и возвращаем ошибку
        if (StartUpCounter > 0x1000) {
            RCC->CR &= ~RCC_CR_HSEON; //Останавливаем HSE
            return 1;
        }
    }

    ////////////////////////////////////////////////////////////
    ////////////////// Настраиваем FLASH  //////////////////////
    ////////////////////////////////////////////////////////////

    //Устанавливаем 2 цикла ожидания для Flash
    //так как частота ядра у нас промежутке 48 MHz < SYSCLK <= 72 MHz
//    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 2 wait state */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;// сбрасываем настройки
    FLASH->ACR |= FLASH_ACR_LATENCY_2;



    ////////////////////////////////////////////////////////////
    ////////////////// Настраиваем Делители ////////////////////
    ////////////////////////////////////////////////////////////

    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; //Делитель шины APB2 отключен
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; //Делитель нишы APB1 равен 2
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1; //Делитель AHB отключен


    ////////////////////////////////////////////////////////////
    /////////////  Настраиваем и запускаем PLL /////////////////
    ////////////////////////////////////////////////////////////

    //Настраиваем PLL
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL); // сбрасываем настройки
    RCC->CFGR |= RCC_CFGR_PLLMULL9; //PLL множитель равен 9 поступающая частота 8  умножаем на 9 того 72 МГц
    RCC->CFGR |= RCC_CFGR_PLLSRC; // Включаем Тактирование PLL от HSE


    RCC->CR |= RCC_CR_PLLON; //Запускаем PLL

    //Ждем успешного запуска или окончания тайм-аута
    for (StartUpCounter = 0;; StartUpCounter++) {
        //Если успешно запустилось, то
        //выходим из цикла
        if (RCC->CR & RCC_CR_PLLRDY)
            break;

        //Если по каким-то причинам не запустился PLL, то
        //отключаем все, что включили
        //и возвращаем ошибку
        if (StartUpCounter > 0x1000) {
            RCC->CR &= ~RCC_CR_HSEON; //Останавливаем HSE
            RCC->CR &= ~RCC_CR_PLLON; //Останавливаем PLL
            return 2;
        }
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= ~(RCC_CFGR_SW);// сбрасываем настройки
    RCC->CFGR |= RCC_CFGR_SW_PLL; //Переключаемся на работу от PLL

    //Ждем, пока переключимся
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL) {
    }

    // После того, как переключились на внешний источник такирования
    // можем отключаем внутренний RC-генератор для экономии энерги
    // но flach работает  через HSI поэтому  прошивать нужн только передав --reset
    // например: st-flash   --reset write  ./build/main.bin  0x08000000
    // RCC->CR &= ~RCC_CR_HSION;

    //Настройка и переклбючение сисемы
    //на внешний кварцевый генератор
    //и PLL запершилось успехом.
    //Выходим
    return 0;
}

void PortInit(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; //Включаем тактирование порта GPIOС

    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13 | GPIO_CRH_MODE14 |
                    GPIO_CRH_CNF14 | GPIO_CRH_MODE15 | GPIO_CRH_CNF15); //для начала все сбрасываем в ноль MODE и  CNF

    //MODE: выход с максимальной частотой 2 МГц
    //CNF: режим push-pull
    GPIOC->CRH |= (0x02 << GPIO_CRH_MODE13_Pos) | (0x00 << GPIO_CRH_CNF13_Pos)
                  | (0x02 << GPIO_CRH_MODE14_Pos) | (0x00 << GPIO_CRH_CNF14_Pos)
                  | (0x02 << GPIO_CRH_MODE15_Pos) | (0x00 << GPIO_CRH_CNF14_Pos);

}


void PortSetHi(void) {
    GPIOC->BSRR = GPIO_ODR_ODR13;
}

void PortSetLow(void) {
    GPIOC->BRR = GPIO_ODR_ODR13;
}


void MCO_Init(int sourse) {
    //Настройка вывода тактового сигнала
    //через MCO-пин
    //sourse - источник тактирования
    // 0 - отключено
    // 4 - SYSCLK
    // 5 - HSI
    // 6 - HSE
    // 7 - PLL/2
    if (sourse != 0 && !(sourse >= 4 && sourse <= 7))
        return;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Включаем тактирование порта PA

    //Настраиваем порт в режим альтернативной функции
    GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8); //Сбрасываем биты в ноль
    GPIOA->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_CNF8_1; //устанавливаем нужные биты

    RCC->CFGR &= ~RCC_CFGR_MCO; //Сначала устанавливаем все в ноль
    RCC->CFGR |= (sourse << RCC_CFGR_MCO_Pos); //Устанавливаем источник тактирования
}


void SysTick_Init() {
//     вместо SysTick_Init можно использовать дефолтную функцию SysTick_Config(72000000UL/1000);

    uint32_t F_CPU = 72000000UL;    // Тактовая у нас 72МГЦ
    uint32_t TimerTick = F_CPU / 2000 - 1;    // Нам нужен килогерц

    SysTick->LOAD = TimerTick;        // Загрузка значения
    SysTick->VAL = TimerTick;        // Обнуляем таймеры и флаги.

    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
void SysTick_Handler(void) {
    GPIOC->ODR ^= GPIO_ODR_ODR15;

}


void ButtonEXTI_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOA->ODR |= GPIO_ODR_ODR0;
    GPIOA->CRL |= GPIO_CRL_MODE0_1;

    GPIOA->CRL &= ~GPIO_CRL_CNF0_0;

    GPIOA->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOA->CRL |= GPIO_CRL_CNF7_1; //включение Pull Up (Подтяжка вверх)/Pull Down (Подтяжка вниз)
    GPIOA->ODR |= GPIO_ODR_ODR7; //Подтяжка вверх



    EXTI->RTSR |= EXTI_RTSR_TR7; //Прерывание по нарастанию импульса
    EXTI->FTSR |= EXTI_FTSR_TR7; //Прерывание по спаду импульса
    EXTI->IMR |= EXTI_IMR_MR7;
    NVIC_EnableIRQ(EXTI9_5_IRQn);

}

void EXTI9_5_IRQHandler(void) {
    //перехватываем   нажатие кнопки и выключаем GPIOA0
    EXTI->PR |= EXTI_PR_PR7;
    asm("nop");
    asm("nop");
    asm("nop");

    static volatile uint8_t flag = 0;
    if (flag) {
        GPIOA->ODR |= GPIO_ODR_ODR0; // led on
        flag = 0;
    } else {
        GPIOA->ODR &= ~GPIO_ODR_ODR0; // led off
        flag = 1;
    }


}

void uart_simple_echo_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // включаем тактирование UART1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // разрешаем тактирование порта GPIOA

// настройка вывода PA9 (TX1) на режим альтернативной функции с активным выходом
// Биты CNF = 10, ,биты MODE = X1
    GPIOA->CRH &= (~GPIO_CRH_CNF9_0);
    GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9);

// настройка вывода PA10 (RX1) на режим входа с подтягивающим резистором
// Биты CNF = 10, ,биты MODE = 00, ODR = 1
    GPIOA->CRH &= (~GPIO_CRH_CNF10_0);
    GPIOA->CRH |= GPIO_CRH_CNF10_1;
    GPIOA->CRH &= (~(GPIO_CRH_MODE10));
    GPIOA->BSRR |= GPIO_ODR_ODR10;

// конфигурация UART1
    USART1->CR1 = USART_CR1_UE; // разрешаем USART1, сбрасываем остальные биты

    USART1->BRR = 7500; // скорость 9600 бод


    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // разрешаем приемник и передатчик
    USART1->CR2 = 0;
    USART1->CR3 = 0;


}


int main(void) {
    int d = 0;
    d = SetSysClockTo72();
    SystemCoreClockUpdate();

    PortInit();
    SysTick_Init();
    ButtonEXTI_Init();
    uart_simple_echo_Init();
    if (d == 0) {
        GPIOC->BSRR = GPIO_ODR_ODR14;
    }
    MCO_Init(6);
    while (1) {
        while ((USART1->SR & USART_SR_RXNE) == 0) {}
        uint32_t d = USART1->DR;
        while ((USART1->SR & USART_SR_TXE) == 0) {}
        USART1->DR = d;
    }
}
//----------------------------------------------------------