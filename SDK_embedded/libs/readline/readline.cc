#include "readline.h"

#include <algorithm>

#include "format.h"



TReadLine::symbol_type_ptr_t TReadLine::TrimStr(symbol_type_ptr_t s)
{
	//trim string left
	while ( (*s == ' ') ) s++ ;
	//trim string right
	char* tmp = s + strlen(s) -1 ;
	while ( (*tmp == ' ') ) *(tmp--) = 0 ;
	return s ;
}

//-----------------------------------------------------
size_t __attribute__ ((noinline)) TReadLine::StrWordCount(const_symbol_type_ptr_t s)
{
	size_t count = 0 ;
	if ( !*s ) return 0 ;
	count++ ;
	while(*s)
		{
			if ( (*s == ' ') &&  (*(s+1) != ' ') && *s )
				count++ ;
			s++ ;
		}
	return count ;
}
//-----------------------------------------------------
void __attribute__ ((noinline)) TReadLine::MakeArgv( symbol_type_ptr_t s, const_symbol_type_ptr_t* argv )
{
	size_t argc = 0 ;
	argv[argc] = s ;

	while ( *s )
			{
				while ( (*s != ' ') && *s) s++ ;
				if ( !(*s) ) break ;
				// terminate current arg sring
				*s = 0 ;
				s++ ;
				// serch next alfabetical symbol
				while ( *s == ' ' ) s++ ;
				argc++ ;
				argv[argc] = s++ ;
			}
}
//-----------------------------------------------------

void TReadLine::enter(TReadLine* rl )
{
	if(rl->echo) rl->send_char('\r');

	symbol_type_ptr_t command_line = *(rl->current) ;

        // обновление списка истории
        if ( (++rl->current) == rl->history.end())
                rl->current = rl->history.begin() ;

        // в качестве буфера используется обновленная текущая строка списка истории
        strcpy( *(rl->current), command_line) ;
        command_line = *(rl->current) ;

	if ( (*command_line) )
		{
			command_line = TrimStr( command_line) ;

			// ������ �� ������ ��� ��������� ����� ����������
			size_t argc = StrWordCount(command_line);
			const_symbol_type_ptr_t argv[argc] ;
			MakeArgv( command_line , argv) ;
			command_handler_map_t::iterator it = rl->command_handler_map.find( argv[0]) ;

			if ( it != rl->command_handler_map.end())
				(*it).second( rl , argv , argc ) ;
			else
				if ( rl->command_not_found ) rl->command_not_found( rl, (const_symbol_type_ptr_t*)argv , argc ) ;

		}

//	rl->send_string ( "\033[33m" ) ;
	rl->send_string (rl->promt) ;
//	rl->send_string ("\033[32m") ;

	(*rl->current)[0] = 0 ;
	rl->pos = 0 ;
}
//-------------------------------------------------------------------------
void TReadLine::tab( TReadLine* rl)
{
	rl->send_string ( "call tab\r" ) ;
}

//-------------------------------------------------------------------------
void TReadLine::backspace( TReadLine* rl)
{
     rl->send_string("\b\033[K");
     if (rl->pos)
       (*rl->current)[--rl->pos] = 0 ;
}

//-------------------------------------------------------------------------
void TReadLine::escape( TReadLine* rl  )
{
	/*unsigned char*/ int  c ;
	rl->recv_char(c) ;
	if ( c == 'O' )
	{
		switch ( rl->recv_char(c))
		{
			case 'F':
				rl->send_string ("end\r") ;
				break ;
			case 'H':
				rl->send_string ("home\r") ;
				break ;
			default :
				break ;
		}

		return ;
	}
	if ( c == '[')
	{
		switch(  rl->recv_char(c) )
			{
				case 'A' :  //history up

				        if ( rl->current == rl->history.begin())
				          rl->current = rl->history.end();

				        rl->current -- ;

				        // стирание строки
				        // установка курсора в начало строки
				        //вывод (*rl->current);

				        rl->UnsafePrintF ("%s%d%c%s" , "\033\[2K\033\[", rl->length,  'D' , *rl->current) ;
					break ;
				case 'B' :  //history up
					rl->send_string ("send history down\r") ;

					break ;
				case 'D' :
					rl->send_string ("move cursor left\r") ;
					break ;
				case 'C' :
					rl->send_string ("move cursor right\r") ;
					break ;
				case '2' :
					rl->recv_char(c) ;
					rl->send_string ("insert\r") ;
					break ;
				case '3' :
					rl->recv_char(c) ;
					rl->send_string ("delete\r") ;
					break ;
				case '5' :
					rl->recv_char(c) ;
					rl->send_string ("page up\r") ;
					break ;
				case '6' :
					rl->recv_char(c) ;
					rl->send_string ("page down\r") ;
					break ;
				case '7':
					rl->send_string ("home\r") ;
					break ;
				case '8':
					rl->send_string ("end\r") ;
					break ;
				default :
					break ;
			}
	}
}
//-------------------------------------------------------------------------
int TReadLine::Init(size_t history_depth , size_t length )
{
	//send_string = 0 ;

	this->length = length ;
	pos = 0 ;

	// init history list
	for ( size_t hi = 0 ; hi < history_depth ; hi++ )
	  {
	    symbol_type_ptr_t history_item = new  symbol_type_t [length+1];
	    if ( !history_item )
	        abort();
	    // init to zero string

	    history_item[0] = 0 ;

	    memset( history_item , 0 , length+1 ) ;



	    history.push_front(history_item) ;
	  }

	current = history.begin() ;
	(*current)[pos] = 0 ;


	//init symbol handler
	symbol_handler_map.insert(make_pair('\r',enter)) ;
	symbol_handler_map.insert(make_pair('\t',tab));
	symbol_handler_map.insert(make_pair('\b',backspace));
	symbol_handler_map.insert(make_pair('\033',escape));

	return 1 ;
}
//--------------------------------------------------------------------------
void TReadLine::Process()
{
	int c;
	while ( recv_char(c) != -1 )
	{
		// call handler for symbol c
		symbol_handler_map_t::iterator it = symbol_handler_map.find(c) ;
		if ( it != symbol_handler_map.end())
		{
			take();
			(*it).second( this ) ;
			give();
		}

		if ( c < 32 ) return ;
		if (echo)
			send_char (c) ;

		if ( pos < length )
		{
			(*current)[pos++] = c ;
			(*current)[pos] = 0 ;
		}
	}
}
//--------------------------------------------------------------------------
int TReadLine::SendBuf( const_symbol_type_ptr_t buf , size_t size )
{
        take();
  	int result = UnsafeSendBuf ( buf , size ) ;
  	give();
  	return result ;
}

int TReadLine::RecvBuf( symbol_type_ptr_t buf , size_t size )
{
	take();
	int result = UnsafeRecvBuf ( buf , size ) ;
	give();
	return result ;
}
//--------------------------------------------------------------------------
void TReadLine::PrintF(const_symbol_type_ptr_t format, ... )
{
	take();
	UnsafePrintF ( format ) ;
	give();
}
//--------------------------------------------------------------------------
void TReadLine::PrintF(const_symbol_type_ptr_t* format )
{
	take();
	UnsafePrintF ( format ) ;
	give();
}
//--------------------------------------------------------------------------
void TReadLine::UnsafePrintF (const_symbol_type_ptr_t format, ... )
{
    kprint( send_char, 0 , format, (int*)&format + 1);
}
//--------------------------------------------------------------------------
void TReadLine::UnsafePrintF (const_symbol_type_ptr_t* format)
{
    kprint( send_char, 0 , *format, (int*)format + 1);
}

