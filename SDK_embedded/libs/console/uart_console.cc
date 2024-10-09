#if 0

#include "console.h"
#include "int2str.h"
#include <math.h>

static int send_char( const int c )
{
	while(USART_GetFlagStatus(CONSOLE_USART, USART_FLAG_TXE) == RESET);
			USART_SendData(CONSOLE_USART,  c);
	return c ;
}

char* send_string( const char* s )
{
	while (*s)
	  send_char( *s++ ) ;
	return (char*)s ;
}

//---------------------------------------------------------------------------

static int recv_char (void)
{
        if (USART_GetFlagStatus(CONSOLE_USART, USART_FLAG_RXNE) == RESET)
          return -1 ;
        else
          return USART_ReceiveData(CONSOLE_USART) ;
}



static size_t recv_string (emb_string& dest)
{
		dest.clear() ;
		if (USART_GetFlagStatus(CONSOLE_USART, USART_FLAG_RXNE) == RESET)
          return (size_t)-1 ;
        int c ;
        do
        {
        	c = USART_ReceiveData(CONSOLE_USART) ;
        	if (c == '\n') return dest.length() ;
        	else dest.push_back(c);
        }while(1) ;
}

static void undefind_command_handler( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	msg_console ("undefind command %s\n" , args[0]) ;
}

// функции враперы синхронизации для readline
extern "C" void console_take()
    {
      ConsoleTask->Take() ;
    }
extern "C" void console_give()
    {
      ConsoleTask->Give() ;
    }

extern "C" void NoSync()
	{
	  ConsoleTask->NoSync();
	}

void TConsoleTask::Code()
{
	while(1)
	{
	    // ожидание данных
	    rx_sem->Take(portMAX_DELAY) ;
	    // быборка данных и обработка
	    read_line->Process() ;
	    // разрешение прерывания модуля USART
	    USART_ITConfig(CONSOLE_USART, USART_IT_RXNE, ENABLE) ;
	}
}

TConsoleTask* ConsoleTask ;

//-----------------------------------------------------------------------
void TConsoleTask::SetSyncMode()
{
	read_line->SetGiveSem( console_give );
	read_line->SetTakeSem( console_take );
}
//-----------------------------------------------------------------------
void TConsoleTask::SetNoSyncMode()
 {
	read_line->SetGiveSem( NoSync );
	read_line->SetTakeSem( NoSync );
 }
//-----------------------------------------------------------------------
TConsoleTask::TConsoleTask(size_t boud)
{
	GPIO_InitTypeDef GPIO_InitStructure ;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(CONSOLE_USART_GPIO_CLOCK, ENABLE);
	/* Enable UART clock */
	CONSOLE_USART_CLOCK_CMD(CONSOLE_USART_CLOCK, ENABLE);


	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(CONSOLE_USART_GPIO, CONSOLE_USART_GPIO_TX_PIN_SOURCE, CONSOLE_USART_GPIO_AF);

	/* Connect PXx to USARTx_Rx*/
	GPIO_PinAFConfig(CONSOLE_USART_GPIO, CONSOLE_USART_GPIO_RX_PIN_SOURCE, CONSOLE_USART_GPIO_AF);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	  GPIO_InitStructure.GPIO_Pin = CONSOLE_USART_TX_PIN | CONSOLE_USART_RX_PIN ;
	  GPIO_Init(CONSOLE_USART_GPIO, &GPIO_InitStructure);

	  USART_InitTypeDef USART_InitStructure ;
	  USART_InitStructure.USART_BaudRate = boud ;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	  /* USART configuration */
	  USART_Init(CONSOLE_USART, &USART_InitStructure);
	  /* Enable USART */
	  USART_Cmd(CONSOLE_USART, ENABLE);

	  USART_ITConfig(CONSOLE_USART, USART_IT_RXNE  , ENABLE);
	  // Enable the USART DMA requests
	  USART_DMACmd(CONSOLE_USART, USART_DMAReq_Tx, ENABLE);


	  NVIC_InitTypeDef NVIC_InitStructure;
	  /* Enable the USARTx Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = CONSOLE_USART_IRQ;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

	  read_line = new TReadLine();
	  read_line -> Init(8 , 128 );
	  read_line->SetSendChar(send_char) ;
	  read_line->SetRecvChar(recv_char) ;
	  read_line->SetSendString(send_string);
	  read_line->SetRecvString(recv_string) ;


	  SetSyncMode();

	  read_line->SetCommandNotFound(undefind_command_handler) ;

	  // call user defined handler setter
	  ConsoleSetCmdHandlers(read_line);

	  read_line->SetPromt("") ;

	  rx_sem = new TSemaphore (TSemaphore::fstBinary) ;
	  tx_sem = new TSemaphore (TSemaphore::fstBinary) ;


}

void TConsoleTask::PrintF(const char* format, ...)
  {
    read_line->PrintF(&format) ;
  }
void TConsoleTask::PrintF(const char** format)
  {
    read_line->PrintF(format) ;
  }
//----------------------------------------------------------------------------------
void __attribute__ ((noinline)) TConsoleTask::UnsafePrintF(const char* format, ...)
 {
	read_line->UnsafePrintF(&format) ;
 }
void __attribute__ ((noinline)) TConsoleTask::UnsafePrintF(const char** format)
 {
	read_line->UnsafePrintF(format) ;
 }
//----------------------------------------------------------------------------------
extern "C" void CONSOLE_USART_IRQ_HANDLER()
{
  if(USART_GetITStatus(CONSOLE_USART, USART_IT_RXNE) != RESET)
   {
      USART_ITConfig(CONSOLE_USART, USART_IT_RXNE, DISABLE) ;

      signed portBASE_TYPE HigherPriorityTaskWoken ;
      ConsoleTask->RxSemaphoreGiveFromISR(&HigherPriorityTaskWoken) ;
      if (HigherPriorityTaskWoken != pdFALSE)
        TSheduler::Yeld();
   }
}

#endif

//----------------------------------------------------------------------------------

