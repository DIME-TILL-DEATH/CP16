
#if 0
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "appdefs.h"
#include "readline.h"

#include "format.h"

//extern "C" int send_char( const int c );

class TConsoleTask : public TTask
{
  public:
     TConsoleTask (size_t boud) ;
     virtual ~TConsoleTask() {} ;
     inline signed portBASE_TYPE RxSemaphoreGiveFromISR(signed portBASE_TYPE* HigherPriorityTaskWoken)
       {
         return rx_sem->GiveFromISR(HigherPriorityTaskWoken) ;
       } ;

     // нельзя заинланить изза необходимости формирования стека
     void __attribute__ ((noinline)) PrintF(const char* format, ...) ;
     void __attribute__ ((noinline)) PrintF(const char** format) ;

     void __attribute__ ((noinline)) UnsafePrintF(const char* format, ...) ;
     void __attribute__ ((noinline)) UnsafePrintF(const char** format) ;


     inline void Clear(){ read_line->Clear() ; }
     inline void Take() { tx_sem->Take(portMAX_DELAY) ; }
     inline void Give() { tx_sem->Give() ; } ;
     static inline void NoSync() { NOP(); } ;

     void SetSyncMode();
     void SetNoSyncMode();


  private:
     void Code() ;
     TSemaphore* rx_sem ;
     TSemaphore* tx_sem ;
     TReadLine* read_line ;
};

extern TConsoleTask* ConsoleTask ;

void ConsoleSetCmdHandlers(TReadLine* rl) ;

#define rmsg(...) \
		ConsoleTask->PrintF( __VA_ARGS__ );

#define msg(...) \
		ConsoleTask->PrintF( "%u: " , TTaskUtilities::GetTickCount() ); \
		ConsoleTask->PrintF( __VA_ARGS__ );

#define err(...) \
		ConsoleTask->PrintF( "%u ERROR :" , TTaskUtilities::GetTickCount() ); \
		ConsoleTask->PrintF( __VA_ARGS__ );

#define rmsg_unsafe(...) \
		ConsoleTask->UnsafePrintF( __VA_ARGS__ );

#define msg_unsafe(...) \
		ConsoleTask->UnsafePrintF( "%u: " , TTaskUtilities::GetTickCount() ); \
		ConsoleTask->UnsafePrintF( __VA_ARGS__ );

#define err_unsafe(...) \
		ConsoleTask->UnsafePrintF( "%u ERROR :" , TTaskUtilities::GetTickCount() ); \
		ConsoleTask->UnsafePrintF( __VA_ARGS__ );


// umsg(...) для использования внутри обработчиков
// команда TReadLine (выполняется в контексте TConsoleTask )
#define msg_console(...) rl->UnsafePrintF( __VA_ARGS__ )


#define msg_file(file,...) 																			\
{																									\
	if( (file==LUA_CONSOLE_STDIN) || (file==LUA_CONSOLE_STDOUT) || (file==LUA_CONSOLE_STDERR)  ) 	\
			{ rmsg_unsafe(__VA_ARGS__); } 															\
	if( (file==LUA_STDIN) || (file==LUA_STDOUT) || (file==LUA_STDERR) ) 							\
			{ msg(__VA_ARGS__); } 																	\
		}


#endif /*__CONSOLE_H__*/
#endif
