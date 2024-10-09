#ifndef __READLINE_H__
#define __READLINE_H__

	/* Result too large */

#include "supc++.h"
#include "supstl.h"
#include "string.h"

#include <map>
//#include <unordered_map>
#include <list>

using namespace std ;


class TReadLine
{
	public:
		typedef char symbol_type_t ;
		typedef const symbol_type_t const_symbol_type_t ;

		typedef symbol_type_t* 	symbol_type_ptr_t ;
		typedef const_symbol_type_t* const_symbol_type_ptr_t ;

		typedef void (*command_handler_t)( TReadLine* rl, const_symbol_type_ptr_t* argv , const size_t argc ) ;
		typedef pair< const char*, command_handler_t>  command_map_pair_t ;
		typedef map < const char*, command_handler_t, KgpCompare /*, KgpAllocator<command_map_pair_t> */> command_handler_map_t ;
		typedef command_handler_map_t::iterator command_handler_map_iterator_t ;

		typedef void (*state_fnc) ( int& ret ) ;
		typedef int (*send_char_fnc) (const int c) ;
		typedef int (*recv_char_fnc) (int& c) ;

		typedef char* (*send_string_fnc) (const_symbol_type_ptr_t s) ;
		typedef size_t (*recv_string_fnc) (emb_string& s) ;


		typedef size_t (*send_buf_fnc) (const_symbol_type_ptr_t buf , size_t len ) ;
		typedef size_t (*recv_buf_fnc) (symbol_type_ptr_t buf , size_t len ) ;

	protected:

		typedef void (*sem_fnc) ();

		typedef void (*symbol_handler)( TReadLine* rl ) ;
		typedef pair< const_symbol_type_t,symbol_handler>  sym_proces_pair_t ;
		typedef map < const_symbol_type_t,symbol_handler, KgpCompare, KgpAllocator<sym_proces_pair_t> > symbol_handler_map_t ;
		//typedef unordered_map<const char, symbol_process, std::hash<const char> , compareT , KgpAllocator<sym_proces_pair_t> > symbol_process_map_t ;


		typedef list<symbol_type_ptr_t, KgpAllocator<symbol_type_ptr_t> > history_t ;

		static void enter(TReadLine* rl) ;
		static void tab(TReadLine* rl) ;
		static void backspace(TReadLine* rl);
		static void escape(TReadLine* rl) ;

		static symbol_type_ptr_t TrimStr(symbol_type_ptr_t s);
		static size_t StrWordCount  (const_symbol_type_ptr_t s) ;
		static void MakeArgv( symbol_type_ptr_t s, const_symbol_type_ptr_t* argv );

	private:

		symbol_handler_map_t symbol_handler_map ; // ���� ������������ �������� ��������
		command_handler_map_t command_handler_map ; // ���� ������������ ���������������� ������

		history_t history ;   // ��������� �������
		history_t::iterator current ; // �������� ���������� �������
		size_t pos ;
		size_t length ;

		state_fnc enable ;
		state_fnc disable ;

		send_char_fnc send_char ;
		recv_char_fnc recv_char ;


        send_string_fnc send_string ;
		recv_string_fnc recv_string ;
		recv_string_fnc recv_line ;

		send_buf_fnc send_buf ;
		recv_buf_fnc recv_buf ;

		command_handler_t command_not_found ;

		const_symbol_type_ptr_t promt ;

		sem_fnc take ;
		sem_fnc give ;

		bool echo ;

	public:
			inline TReadLine () { echo = true; } ;
			inline ~TReadLine() { symbol_handler_map.clear(); } ;
			int Init(size_t history_depth , size_t line_len );
			void Process();
			void inline AddCommandHandler( const_symbol_type_ptr_t command , command_handler_t handler )
				{
					command_handler_map.insert ( command_map_pair_t(command,handler) ) ;
				}
			void inline RemoveCommandHandler( const_symbol_type_ptr_t command )
				{
					command_handler_map.erase ( command_handler_map.find(command)) ;
				}
			void inline GetCommandHandlerMapIterator( command_handler_map_iterator_t& begin, command_handler_map_iterator_t& end )
				{
					begin = command_handler_map.begin() ;
					end = command_handler_map.end();
				}

			inline void Enable( state_fnc enable ) { this->enable = enable ; }
			inline state_fnc Enable() {  return enable ; }
			inline void Enable(int& ret) { return enable(ret); }

			inline void Disable( state_fnc disable ) { this->disable = disable ; }
			inline state_fnc Disable() {  return disable ; }
			inline void Disable(int& ret) { return disable(ret); }

			inline void SendChar( send_char_fnc send_char ) { this->send_char = send_char ; }
			inline send_char_fnc SendChar() {  return send_char ; }
			inline int SendChar(const int c) { return send_char(c); }

			inline void RecvChar( recv_char_fnc recv_char ) { this->recv_char = recv_char ; }
			inline recv_char_fnc RecvChar() { return recv_char ; }
			inline int RecvChar( int& c ) { return recv_char(c); } ;

                        inline void SendBuf( send_buf_fnc send_buf ) { this->send_buf = send_buf ; }
                        inline send_buf_fnc SendBuf() { return send_buf ; }
                        inline int UnsafeSendBuf( const_symbol_type_ptr_t buf , size_t size ) { return send_buf(buf , size); }
                        int SendBuf( const_symbol_type_ptr_t buf , size_t size );

			inline void RecvBuf( recv_buf_fnc recv_buf ) { this->recv_buf = recv_buf ; }
                        inline recv_buf_fnc RecvBuf() { return recv_buf ; }
                        inline int UnsafeRecvBuf( symbol_type_ptr_t buf , size_t size ) { return recv_buf(buf,size) ; }
                        int RecvBuf( symbol_type_ptr_t buf , size_t size );

                        inline void RecvString( recv_string_fnc recv_string ) { this->recv_string = recv_string ; }
			inline recv_string_fnc RecvString() { return recv_string ; }
                        inline size_t RecvString( emb_string& dest ) { return recv_string(dest); } ;

                        void inline SendString( send_string_fnc send_string ) { this->send_string = send_string ; }
			inline send_string_fnc SendString() { return send_string ; }
			inline symbol_type_ptr_t SendString( const_symbol_type_ptr_t dest ) { return send_string(dest); } ;
			inline symbol_type_ptr_t SendString( emb_string& dest ) { return send_string(dest.c_str()); } ;

			inline void RecvLine( recv_string_fnc recv_line ) { this->recv_line = recv_line ; }
                        inline recv_string_fnc RecvLine() { return recv_line ; }
                        inline size_t RecvLine( emb_string& dest ) { return recv_line(dest); } ; ;



			void inline SetCommandNotFound( command_handler_t command_not_found ) { this->command_not_found = command_not_found ; }
			inline void SetPromt(const_symbol_type_ptr_t promt) { this->promt = promt ; } ;

			inline void SetTakeSem( sem_fnc take ) { this->take = take ; }
			inline sem_fnc GetTakeSem() { return take ; }
			inline void SetGiveSem( sem_fnc give ) { this->give = give ; }
			inline sem_fnc GetGiveSem() { return give ; }

			inline command_handler_map_t& CommandHahdlerMap() { return command_handler_map ; } ;

			void __attribute__ ((noinline)) PrintF(const_symbol_type_ptr_t* format) ;
			void __attribute__ ((noinline)) PrintF(const_symbol_type_ptr_t  format, ... ) ;
			inline void Print (const emb_string& str ) { take(); send_string(str.c_str()) ; give(); } ;
			inline void Print (const_symbol_type_ptr_t  str ) { take();  send_string(str); give(); } ;
			inline void Print (const_symbol_type_t c ) { take();  send_char(c); give(); } ;


			void __attribute__ ((noinline)) UnsafePrintF (const_symbol_type_ptr_t* format) ;
			void __attribute__ ((noinline)) UnsafePrintF (const_symbol_type_ptr_t format, ... )  ;
			inline void UnsafePrint (const emb_string& str ) { send_string(str.c_str()) ; } ;
			inline void UnsafePrint (const_symbol_type_ptr_t  str ) { send_string(str); } ;
			inline void UnsafePrint (const_symbol_type_t c ) { send_char(c); } ;



			inline void Clear () {  Print("\033[2J\033[H"); } ;
			inline void CursorUp () {  Print("\033[1A"); } ;
			inline void CursorLeft () {  Print("\033[200D"); } ;


			inline void UnsafeSetColorRed() { UnsafePrint("\033[31m") ; } ;
			inline void UnsafeSetColorGreen() { UnsafePrint("\033[32m") ; } ;

			inline void Echo(bool state) { echo = state ; }
			inline bool Echo() { return echo ; }

};

#endif /*__READLINE_H__*/
