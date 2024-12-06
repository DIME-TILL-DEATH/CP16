
#include "console.h"
#include "int2str.h"
#include <math.h>

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
        read_line->Process() ;
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

TConsoleTask::TConsoleTask(/*readline_io_t* readline_io,*/ const size_t rx_queue_size)
{
	  read_line = new TReadLine();
	  read_line -> Init(1 , 256 );

	  SetSyncMode();

	  read_line->SetCommandNotFound(undefind_command_handler) ;

	  // call user defined handler setter
	  consoleSetCmdHandlers(read_line);

	  read_line->SetPromt("") ;

	  rx_sem = new TSemaphore (TSemaphore::fstBinary) ;
	  tx_sem = new TSemaphore (TSemaphore::fstBinary) ;


	  rx_queue = new TQueue(rx_queue_size , sizeof(char)) ;
	  rx_queue->AddToRegistry("ConsoleTask rx_queue");

}

void TConsoleTask::SetIo( readline_io_t *readline_io )
 {
	int ret ;

	if (read_line->Disable())
	{
	  // выключение перефении текущего канала связи
	  Disable(ret);
	}




	//Suspend(Handle);


	// установка обработчиков нового канала связи
	SendChar(readline_io->send_char) ;
	RecvChar(readline_io->recv_char) ;
	SendString(readline_io->send_string);
	RecvString(readline_io->recv_string) ;
	RecvLine(readline_io->recv_line) ;
	SendBuf(readline_io->send_buf);
	RecvBuf(readline_io->recv_buf) ;
	Enable(readline_io->enable);
	Disable(readline_io->disable) ;



	char c ;
	// очистка очереди от данных старого канала
	while (  rx_queue->Receive( &c , 0)  == TQueue::TQueueReceiveResult::qrrPass ) {}

	// инициализация HW нового канала связи
	if (readline_io->console_task_hw_init)
	      readline_io->console_task_hw_init();

	//Resume(Handle);

	// включение перефении нового канала связи
	Enable(ret);

 }

void TConsoleTask::PrintF(const char* format, ...)
 {
        read_line->PrintF(&format) ;
 }
//----------------------------------------------------------------------------------
void TConsoleTask::PrintF(const char** format)
 {
        read_line->PrintF(format) ;
 }
//----------------------------------------------------------------------------------
void __attribute__ ((noinline)) TConsoleTask::UnsafePrintF(const char* format, ...)
 {
	read_line->UnsafePrintF(&format) ;
 }
//----------------------------------------------------------------------------------
void __attribute__ ((noinline)) TConsoleTask::UnsafePrintF(const char** format)
 {
	read_line->UnsafePrintF(format) ;
 }
//----------------------------------------------------------------------------------
int console_printf(const char* format, ...)
 {
        if (TTaskUtilities::GetCurrentTaskHandle()!=ConsoleTask->GetHandle())
          ConsoleTask->PrintF(&format);
        else
          ConsoleTask->UnsafePrintF(&format);
        return 0 ;
 }
