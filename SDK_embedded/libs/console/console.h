#ifndef __USB_CONSOLE_H__
#define __USB_CONSOLE_H__

#include "appdefs.h"
#include "readline.h"

#include "format.h"

class TConsoleTask : public TTask
{
  public:

    typedef void (*console_task_hw_init_t)() ;

    typedef void (*console_state_t)(int& ret) ;

    typedef const struct
     {
       console_task_hw_init_t  console_task_hw_init ;

       TReadLine::state_fnc enable ;
       TReadLine::state_fnc disable ;

       TReadLine::send_char_fnc send_char ;
       TReadLine::recv_char_fnc recv_char ;

       TReadLine::send_string_fnc send_string ;
       TReadLine::recv_string_fnc recv_string ;
       TReadLine::recv_string_fnc recv_line ;

       TReadLine::send_buf_fnc send_buf ;
       TReadLine::recv_buf_fnc recv_buf ;
     } readline_io_t ;


     TConsoleTask (/*readline_io_t *io,*/ const size_t rx_queue_size);
     virtual ~TConsoleTask() {} ;

        // ----------  readline IO stream interface ------------------

        // Note! befor use TConsoleTask

 	inline void Enable( TReadLine::state_fnc  enable ) { read_line->Enable(enable) ; }
 	inline TReadLine::state_fnc Enable() { return read_line->Enable() ; }
 	inline void Enable(int& ret) { return read_line->Enable(ret); }

 	inline void Disable( TReadLine::state_fnc  disable ) { read_line->Disable(disable) ; }
 	inline TReadLine::state_fnc Disable() { return read_line->Disable() ; }
 	inline void Disable(int& ret) { return read_line->Disable(ret); }

	inline void SendChar( TReadLine::send_char_fnc  send_char ) { read_line->SendChar(send_char) ; }
	inline TReadLine::send_char_fnc SendChar() { return read_line->SendChar() ; }
	inline int SendChar(const int c) { return read_line->SendChar(c); }

	inline void RecvChar( TReadLine::recv_char_fnc recv_char ) { read_line->RecvChar(recv_char) ; }
	inline TReadLine::recv_char_fnc RecvChar() { return read_line->RecvChar() ; }
	inline int RecvChar(int& c) { return read_line->RecvChar(c); }

	inline void RecvString( TReadLine::recv_string_fnc recv_string ) { read_line->RecvString(recv_string) ; }
	inline TReadLine::recv_string_fnc RecvString() { return read_line->RecvString() ; }
	inline size_t RecvString( emb_string& dest ) { return read_line->RecvString(dest); } ;

	inline void RecvLine( TReadLine::recv_string_fnc recv_string ) { read_line->RecvLine(recv_string) ; }
	inline TReadLine::recv_string_fnc RecvLine() { return read_line->RecvLine() ; }
	inline size_t RecvLine( emb_string& dest ) { return read_line->RecvLine(dest); } ;

	void inline SendString( TReadLine::send_string_fnc send_string ) { read_line->SendString(send_string) ; }
	inline TReadLine::send_string_fnc SendString() { return read_line->SendString() ; }
	inline TReadLine::symbol_type_ptr_t SendString( TReadLine::const_symbol_type_ptr_t dest ) { return read_line->SendString(dest); } ;
	inline TReadLine::symbol_type_ptr_t SendString( emb_string& dest ) { return read_line->SendString(dest.c_str()); } ;

	inline void SendBuf( TReadLine::send_buf_fnc send_buf ) { read_line->SendBuf(send_buf) ; }
	inline TReadLine::send_buf_fnc SendBuf() { return read_line->SendBuf() ; }
	inline int SendBuf( TReadLine::const_symbol_type_ptr_t buf , size_t size ) { return read_line->SendBuf(buf , size); }

	inline void RecvBuf( TReadLine::recv_buf_fnc recv_buf ) { read_line->RecvBuf(recv_buf) ; }
	inline TReadLine::recv_buf_fnc RecvBuf() { return read_line->RecvBuf() ; }
	inline int RecvBuf( TReadLine::symbol_type_ptr_t buf , size_t size ) { return read_line->RecvBuf(buf,size) ; }

	//------------------------------------------------------------------------------------



     // нельзя заинланить изза необходимости формирования стека
     void  PrintF(const char* format, ...) __attribute__ ((noinline));
     void __attribute__ ((noinline)) PrintF(const char** format) ;

     void  UnsafePrintF(const char* format, ...) __attribute__ ((noinline));
     void __attribute__ ((noinline)) UnsafePrintF(const char** format) ;


     inline void Clear(){ read_line->Clear() ; }
     inline void Take() { tx_sem->Take(portMAX_DELAY) ; }
     inline void Give() { tx_sem->Give() ; } ;
     static inline void NoSync() { NOP(); } ;

     void SetSyncMode();
     void SetNoSyncMode();

     inline void Echo(bool state) { read_line->Echo(state); }
     inline bool Echo() { return read_line->Echo(); }

     inline TQueue::TQueueSendResult WriteToInputBuffFromISR( const char* symbol,  BaseType_t*  HigherPriorityTaskWoken ) {  return rx_queue->SendToBackFromISR( symbol , HigherPriorityTaskWoken) ; }
     inline TQueue::TQueueSendResult WriteToInputBuff( const char* symbol ) {  return rx_queue->SendToBack( symbol , 0) ; }

     inline TQueue::TQueueReceiveResult ReadFromInputBuff( char* symbol ) {  return rx_queue->Receive( symbol , portMAX_DELAY) ; }

     void SetIo( readline_io_t *io ) ;

  private:
     void Code() ;
     TSemaphore* rx_sem ;
     TSemaphore* tx_sem ;
     TReadLine* read_line ;

     TQueue* rx_queue ;
};

extern TConsoleTask* ConsoleTask ;

void consoleSetCmdHandlers(TReadLine* rl);

// команда TReadLine (выполняется в контексте TConsoleTask ) !!!!!!!!!!!!!!!!
#define msg_console(...) rl->UnsafePrintF( __VA_ARGS__ )

#define rmsg(...) \
		if (TTaskUtilities::GetCurrentTaskHandle()!=ConsoleTask->GetHandle()) ConsoleTask->PrintF( __VA_ARGS__ ); \
		else  ConsoleTask->UnsafePrintF( __VA_ARGS__ );



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

#define rmsg_buf(msg,size) { ConsoleTask->SendBuf(msg,size); }


extern "C" int  condole_printf(const char* format, ...) __attribute__ ((noinline,used));

#endif /*__USB_CONSOLE_H__*/
