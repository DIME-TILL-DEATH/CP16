#include "bt.h"

int uart_send_char( const int c )
{
	 while(!USART_GetFlagStatus(UART4,USART_FLAG_TXE));
	 USART_SendData(UART4,c);
	 return c ;
}

size_t uart_send_buf( const char* buf , size_t size )
{
	for ( size_t i = 0 ; i < size ; i++)
	{
		int c = buf[i] ;
		uart_send_char(c);
	}
    return size ;
}

char* uart_send_string( const char* s )
{
        size_t length = kgp_sdk_libc::strlen(s)  ;

        if (!length)
          return (char*)s ;
        uart_send_buf( s , length ) ;
        return (char*)s ;
}



//---------------------------------------------------------------------------
int uart_recv_char (int& c)
{
	    char tmp ;
        ConsoleTask->ReadFromInputBuff( &tmp ) ;
        c = tmp ;
        return c;
}


size_t uart_recv_string (emb_string& dest)
{
        dest.clear() ;
        int c ;
        do
        {
        	char tmp ;
        	ConsoleTask->ReadFromInputBuff(&tmp) ;
                c = tmp ;
                if ( !c ) return dest.length() ;
                else dest.push_back(c);
        }while(1) ;
}

size_t uart_recv_line (emb_string& dest)
{
        dest.clear() ;
        int c ;
        do
        {
        	char tmp ;
        	ConsoleTask->ReadFromInputBuff(&tmp) ;
                c = tmp ;
                if ( (c == '\r') || (c == '\n') ) return dest.length() ;
                else dest.push_back(c);
        }while(1) ;
}

size_t uart_recv_buf (char* buf , size_t size)
{
        size_t c = 0 ;
        while ( c < size )
        {
        	char tmp ;
            ConsoleTask->ReadFromInputBuff(&tmp) ;
            buf[c++] = tmp ;
        }
        return c ;
}

void uart_console_task_hw_init()
		{
	      GPIO_InitTypeDef  GPIO_InitStructure;
	      USART_InitTypeDef USART_InitStructure;
	      NVIC_InitTypeDef NVIC_InitStructure;

	      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	      RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	      GPIO_StructInit(&GPIO_InitStructure);
	      GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF ;
	      GPIO_InitStructure.GPIO_Speed=GPIO_Low_Speed;
	      GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 ;
	      GPIO_Init(GPIOC, &GPIO_InitStructure);

	      GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	      GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

	      RCC_APB1PeriphClockCmd( RCC_APB1Periph_UART4, ENABLE );

	      USART_Cmd(UART4,DISABLE);

	      USART_StructInit(&USART_InitStructure);
	      USART_InitStructure.USART_BaudRate = 9600;
	      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	      USART_InitStructure.USART_StopBits = USART_StopBits_1;
	      USART_InitStructure.USART_Parity = USART_Parity_No;
	      USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	      USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	      USART_Init(UART4, &USART_InitStructure);
	      USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);


	      NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn ;
	      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	      NVIC_Init( &NVIC_InitStructure );

	      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

		}

void uart_enable(int& ret )
		{
	      USART_Cmd(UART4,ENABLE);
	      ret = 0 ;
		}

void uart_disable(int& ret )
		{
	      USART_Cmd(UART4,DISABLE);
	      ret = 0 ;
		}

TConsoleTask::readline_io_t bt_io
{
    .console_task_hw_init = uart_console_task_hw_init,

    .enable = uart_enable,
    .disable = uart_disable,

    .send_char = uart_send_char,
    .recv_char = uart_recv_char,

    .send_string = uart_send_string,
    .recv_string = uart_recv_string,
    .recv_line = uart_recv_line,

    .send_buf = uart_send_buf,
    .recv_buf = uart_recv_buf,
} ;

extern "C" void UART4_IRQHandler()
{
	USART_ClearITPendingBit(UART4, USART_IT_RXNE);
	char c = USART_ReceiveData(UART4);
    BaseType_t HigherPriorityTaskWoken ;
    ConsoleTask->WriteToInputBuffFromISR( &c,  &HigherPriorityTaskWoken );
    portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}
