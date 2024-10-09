#include "console_handlers.h"
#include "eepr.h"
#include "sigma.h"
#include "cs.h"
#include "math.h"
#include "filt.h"
#include "ff.h"
#include "Reverb/reverb.h"
#include "init.h"

extern char __CCM_BSS__ buff[];
char hex[3] = {0,0,0} ;
uint16_t imp_count = 0;
volatile uint8_t rev_en = 0;
volatile uint8_t rev_en1 = 0;

// первый аргумент - имя файла(актуально для второго 0 ) второй - 0 это запись файла импульса , данные идут побайтно с подтверждением ,
// 1 это загрузка импульса - данные идут семплами(по 3 байта начиная с младшева)

volatile uint32_t buff_size = rev_size * sizeof(float) ;
volatile uint32_t stream_pos ;
static void current_cabinet_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	std::emb_string err_str ;
	size_t chunk;
	if ( count == 1 )
	  {
	     console_out_currnt_cab(err_str, rl);
	     return ;
	  }
	if ( count == 3  )
	  {
	   rev_en = 1;
	   if ( args[2][0] == '0' && kgp_sdk_libc::strlen(args[2])== 1 )
	   {
		   while(!rev_en1);
		   stream_pos = 0;
    	   do
    	     {
    	       int c ;
    	       char w   ;
               rl->RecvChar(c);
	    	   if ( c == '\r')
	    	    {
	    		   delete_current_cab(err_str, rl);
	    		   FATFS fs;
	    		   FRESULT res ;
	    		   FIL f;
	    		   UINT bw ;
	    	       UINT f_size;
	    	       res = f_mount ( &fs , "0:",  1);
	    	       if ( res != FR_OK )
	    	         {
	        	       msg_console("error: 'f_mount' result '%s'\n", f_err2str(res) ) ;
	        	       return ;
	    	         }
	    		   emb_string file_name;
	    		   file_name = "0:/Bank_";
	    		   file_name += (size_t)bank_pres[0];
	    		   file_name += "/Preset_";
	    		   file_name += (size_t)bank_pres[1];
	    		   file_name += "/" ;
	    		   file_name += args[1] ;
	               FRESULT fs_res = f_open(&f, file_name.c_str(),  FA_CREATE_ALWAYS | FA_WRITE );
	               if ( fs_res == FR_OK )
	               {
	            	   //msg_console("NX\n") ;
	               }
	               else
	               {
	            	   msg_console("error: file %s not created, 'f_open' result '%s'\n", args[1] , f_err2str(res)) ;
	            	   f_mount(0, "0:", 0);
	            	   return ;
	               }
	    //----------------------------------------------------------------------------------------------
	               // скидывание остатков потока из буффера если они там есть
	               fs_res = f_write(&f, buff , stream_pos , &bw);
	               if ( bw != stream_pos || fs_res != FR_OK )
	    	         {
	    	    	   f_close(&f);
	    	    	   f_unlink(file_name.c_str());
	    	    	   msg_console("file write operation faild\n" ) ;
	       	           f_mount(0, "0:", 0);
	       	           return ;
	                 }
	    //---------------------------------------------------------------------------------------------
	                f_close(&f);
	    	    	f_mount(0, "0:", 0);
	    	    	kgp_sdk_libc::memset(buff,0,buff_size);
	    	    	rev_en = 0;
	    	    	rev_en1 = 0;
	    	    	msg_console("ccEND\n") ;
	    	      return ;
	    	    }
               if(c > 57)c -= 39;
	    	   w =  (c - '0') << 4 ;
	    	   rl->RecvChar(c);
               if ( c == '\r')
	    	    {
	    	      msg_console("SYNC ERROR\n") ;
	    	      return ;
	    	    }
               if(c > 57)c -= 39;
		       w  |=  c - '0' ;
		       buff[stream_pos++] = w ; // запись в буффер очередного байта
	           //msg_console("NX\n") ;
		       }
	       while (1) ;
//--------------------------------------------------------------------------------------
       }
	   else
	   {
		   while(!rev_en1);
           imp_count = 0;
		   chunk = 3 ;
    	   //i2hex(chunk,hex);
    	   //msg_console("RX %s\n",hex) ;
    	   uint8_t buff1[3] = {0,0,0};
    	   do
    	     {
    	       int c ;
    	       char w   ;
	           for (size_t i = 0 ; i < chunk ; i++ )
	              {
                      rl->RecvChar(c);
	    	          if ( c == '\r')
	    	            {
	    	        	  if(imp_count < 983)
	    	        	  {
	    	        		for(; imp_count < 984 ; imp_count++)
	    	        		{
	    	        			buff1[0] = buff1[1] = buff1[2] = 0;
	    	        			cab_data[imp_count] = convert ( (uint8_t*)buff1 ) ;
	    	        		}
                  		  }
	    	        	  sig_load(cab_data , imp_buf_uint);
	    	        	  cab_fl = 1;
	    	        	  prog_data[cab_on] = 1;
	    	        	  imp_count = 0;
	  	    	    	  rev_en = 0;
	  	    	    	  rev_en1 = 0;
	    	    	      msg_console("ccEND\n") ;
	    	    	      return ;
	    	            }
	    	          if(c > 57)c -= 39;
	    	          w =  (c - '0') << 4 ;
	    	          rl->RecvChar(c);
		    	      if(c > 57)c -= 39;
		    	      w  |=  c - '0' ;
		    	      buff1[i] = w;
	               }
	           if(imp_count < 984)cab_data[imp_count++] = convert ( (uint8_t*)buff1 ) ;
    	      }
	       while (1) ;
		   return ;
	   }
	}
  msg_console("invalid args count\n" ) ;
}
static void read_name_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	std::emb_string err_str ;
	console_out_currnt_nam(err_str, rl);
}
static void get_state_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	if ( count == 1 )
	{
		for ( size_t i = 0 ; i < pdCount ; i++ )
		{
			i2hex(prog_data[i],hex);
			msg_console("%s" , hex ) ;
		}
		msg_console("\n") ;
		return ;
	}
	size_t a = 0;
	char w;
	int c ;
	do
	{
         rl->RecvChar(c);
         if (c == '\r')
          {
        	 param_set();
    	     msg_console("gsEND\n") ;
    	     return ;
            }
         if(c > 57)c -= 39;
         w =  (c - '0') << 4 ;
         rl->RecvChar(c);
	     if ( c == '\r')
            {
          	  msg_console("SYNC ERROR\n") ;
          	  return ;
            }
	     if(c > 57)c -= 39;
	     w  |=  c - '0' ;
	     prog_data[a++] = w;
	}
	while(1);
}

static void cabinet_enable_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		i2hex( prog_data[cab_on],hex);
		msg_console("%s\n" , hex ) ;
		return ;
	 }
	 char* end ;
	 uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
	 prog_data[cab_on] = val ;
}
static void master_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[pres_lev],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[pres_lev] = val ;
     if(prog_data[cab_on] && cab_fl)
        {
           sig_volume(powf(prog_data[pres_lev],2.0f)*(1.0f/powf(31.0f,2.0f)));
           p_vol = 1.0f;
        }
     else p_vol = powf(prog_data[pres_lev],2.0f)*(1.0f/powf(31.0f,2.0f));
}
static void early_on_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[er_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[er_on] = val ;
}
static void early_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[e_vol],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[e_vol] = val ;
     ear_vol = prog_data[e_vol]*(1.0f/31.0f);
}
static void early_type_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[e_t],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[e_t] = val ;
}
static void presence_on_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[pr_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[pr_on] = val ;
}
static void presence_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[pr_vol],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[pr_vol] = val ;
     set_shelf(prog_data[pr_vol]*(25.0f/31.0f));
}
static void lpf_on_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[lop_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[lop_on] = val ;
}
static void lpf_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[lop],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[lop] = val ;
     float lopas = powf(195 - prog_data[lop],2.0f)*(19000.0f/powf(195.0f,2.0f))+1000.0f;
     SetLPF(lopas);
}
static void hpf_on_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[hip_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[hip_on] = val ;
}
static void hpf_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[hip],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[hip] = val ;
     float hipas = prog_data[hip]*(980.0f/255.0f) + 20.0f;
     SetHPF(hipas);
}
static void eq_on_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[eq_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[eq_on] = val ;
}
static void eq_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count)
{
	 uint8_t a = args[1][0] - 48;
	 if ( count == 2 )
	 {
		 i2hex( prog_data[a],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 if ( count == 3  )
	 {
		 char* end ;
	     uint32_t val = kgp_sdk_libc::strtol ( args[2] , &end, 16 );
	     prog_data[a] = val ;
	     filt_ini(a , prog_data + fr1 , prog_data + q1);
	     set_filt(a,prog_data[a]);
	 }
}
static void eq_frec_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count)
{
	 uint8_t a = args[1][0] - 48;
	 uint8_t aa = args[1][0] - 48 + fr1;
	 int8_t b = prog_data[aa];
	 uint8_t c = abs(b);
	 if ( count == 2 )
	 {
		 char hex[4] = {0,0,0,0} ;
		 i2hex( c ,hex + 1);
		 if(b < 0)hex[0] = '-';
		 else hex[0] = ' ';
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 if ( count == 3  )
	 {
		 char* end ;
		 int32_t val = kgp_sdk_libc::strtol ( args[2] , &end, 16 );
	     prog_data[aa] = val ;
	     filt_ini(a , prog_data + fr1 , prog_data + q1);
	     set_filt(a,prog_data[a]);
	 }
}
static void eq_q_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count)
{
	 uint8_t a = args[1][0] - 48;
	 uint8_t aa = args[1][0] - 48 + q1;
	 int8_t b = prog_data[aa];
	 uint8_t c = abs(b);
	 if ( count == 2 )
	 {
		 char hex[4] = {0,0,0,0} ;
		 i2hex( c ,hex + 1);
		 if(b < 0)hex[0] = '-';
		 else hex[0] = ' ';
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 if ( count == 3  )
	 {
		 char* end ;
		 int32_t val = kgp_sdk_libc::strtol ( args[2] , &end, 16 );
	     prog_data[aa] = val ;
	     filt_ini(a , prog_data + fr1 , prog_data + q1);
	     set_filt(a,prog_data[a]);
	 }
}
static void preamp_on_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[preamp_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[preamp_on] = val ;
     msg_console("%s\n" , hex ) ;
}
static void preamp_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[preamp_vol],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[preamp_vol] = val ;
     pream_vol = powf(prog_data[preamp_vol],2.0f)*(1.0f/powf(31.0f,2.0f));
     msg_console("%s\n" , hex ) ;
}
static void preamp_low_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[preamp_lo],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint8_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[preamp_lo] = val ;
     pre_param(0,val);
     msg_console("%s\n" , hex ) ;
}
static void preamp_mid_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[preamp_mi],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[preamp_mi] = val ;
     pre_param(1,val);
     msg_console("%s\n" , hex ) ;
}
static void preamp_high_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[preamp_hi] ,hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     prog_data[preamp_hi] = val ;
     pre_param(2,val);
     msg_console("%s\n" , hex ) ;
}

static void change_bank_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 char* end ;
     uint32_t dir = kgp_sdk_libc::strtol ( args[1] , &end, 16 );

     next_preset(dir , bpf_bank);
     prog_ch();

     i2hex( bank_pres[0],hex);
     if(cab_fl)hex[0] = 49;
     msg_console("%s\n" , hex ) ;
}
static void change_pres_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 char* end ;
     uint32_t dir = kgp_sdk_libc::strtol ( args[1] , &end, 16 );

     next_preset(dir , bpf_preset);
     prog_ch();

	 i2hex( bank_pres[1],hex);
	 if(cab_fl)hex[0] = 49;
	 msg_console("%s\n" , hex ) ;
}
static void gate_on_off_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[gate_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
    uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
    prog_data[gate_on] = val ;
    msg_console("%s\n" , hex ) ;
}
static void gate_threshold_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[gate_th],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
    uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
    prog_data[gate_th] = val ;
    gate_par(prog_data[gate_th] << 8);
    msg_console("%s\n" , hex ) ;
}
static void gate_decay_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[gate_att],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
    uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
    prog_data[gate_att] = val ;
    gate_par(1 | (prog_data[gate_att] << 8));
    msg_console("%s\n" , hex ) ;
}
static void compreccor_on_off_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[compr_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
    uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
    prog_data[compr_on] = val ;
    msg_console("%s\n" , hex ) ;
}
static void compreccor_sustein_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[sustein],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
    uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
    prog_data[sustein] = val ;
    comp_par(0 | (prog_data[sustein] << 8));
    msg_console("%s\n" , hex ) ;
}
static void compreccor_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( prog_data[compr_vol],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
    uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
    prog_data[compr_vol] = val ;
    comp_par(2 | (prog_data[compr_vol] << 8));
    msg_console("%s\n" , hex ) ;
}
static void save_pres_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	save_pres();
	msg_console("END\n") ;
}
static void get_mode_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 i2hex( sys_para[2],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
     sys_para[2] = val ;
     save_sys();
}
static void get_bp_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	for ( size_t i = 0 ; i < 2 ; i++ )
	{
		i2hex(bank_pres[i],hex);
		msg_console("%s" , hex ) ;
	}
	msg_console("\n") ;
}
static void get_amtid_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
#ifdef __LA3_MOD__
	i2hex(5,hex); //PangaeaCP16_RV LA3
#else
	i2hex(2,hex); //PangaeaCP16_RV
#endif

	msg_console("%s" , hex ) ;
	msg_console("\n") ;
	msg_console("END\n") ;
}
static void get_amtver_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	msg_console("%s\n" , ver ) ;
	msg_console("END\n") ;
}
static void esc_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	prog_ch();
	msg_console("END\n") ;
}
static void load_current_cab_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	emb_string err_msg;
	if(load_pres(cab_data,err_msg,0) != true)cab_fl = 0;
	else {
		sig_load(cab_data , imp_buf_uint);
		cab_fl = 1;
	}
	msg_console("END\n") ;
}

#if 0
static void delete_current_cab_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	std::emb_string err_str ;
	delete_current_cab(err_str, rl);
	msg_console("END\n") ;
}
#endif

static void direct_program_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 char* end ;

	 if ( count > 1 )
	  {
       uint32_t val = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
       if ( end != args[1] )
         {
    	    decode_preset( bank_pres, val);
    	    prog_ch();
    	    TTask::Delay(50);
 	        msg_console("END\n") ;
 	        return ;
         }
	  }
     msg_console("PARAM_ERROR\n") ;
}

static void read_full_name_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	std::emb_string err_str ;
	if ( count == 1)
	   console_out_full_nam(err_str, rl, false);
	else
	   console_out_full_nam(err_str, rl, true);
}

//------------------------------------------------------------------------------
static void sw4_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 msg_console("%s\n" , sw4_state ? "enable" : "disable") ;
		 return ;
	 }
	if ( !kgp_sdk_libc::strcmp (args[1], "enable"))
		sw4_state = true ;
	else
		sw4_state = false ;

	msg_console("END\n") ;
}
//------------------------------------------------------------------------------
static void fs_format_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	std::emb_string err_str ;
	console_fs_format(err_str, rl);
}
//------------------------------------------------------------------------------
//static void fs_delete_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
//{
//	std::emb_string err_str ;
//	console_fs_delete_file(err_str, rl);
//}
//------------------------------------------------------------------------------
static void fw_update_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{

	std::emb_string err_msg  ;
	if ( console_fs_write_file(err_msg, rl , "0:/firmware"))
	{
	    TTask::Delay(100); // задержка, необходимая для сброса данных в TUsbTask по интерфейсу usb
		NVIC_SystemReset();
	}

}
//------------------------------------------------------------------------------


static void preset_wav_copy_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
   std::emb_string err_msg  ;

   char* end ;
   uint32_t preset_src  = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
   uint32_t preset_dst = kgp_sdk_libc::strtol ( args[2] , &end, 16 );

   console_fs_preset_copy_wav_file(err_msg, rl , preset_src, preset_dst) ;
   msg_console("END\n") ;
}
//------------------------------------------------------------------------------
static void preset_wav_store_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
   std::emb_string err_msg  ;

   char* end ;
   uint32_t preset_src  = kgp_sdk_libc::strtol ( args[1] , &end, 16 );

   console_fs_preset_store_wav_file_tmp(err_msg, rl , preset_src) ;
   msg_console("END\n") ;
}
static void preset_wav_load_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
   std::emb_string err_msg  ;

   char* end ;
   uint32_t preset_dst = kgp_sdk_libc::strtol ( args[1] , &end, 16 );

   if ( !console_fs_preset_load_wav_file_tmp(err_msg, rl , preset_dst))
   {
	   msg_console("TMP is empty\n") ;
   }
   msg_console("END\n") ;
}

//------------------------------------------------------------------------------
static void preset_wavs_delete_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{

	emb_string preset_dir_name;
	if ( count==1 ) // удаление в текщем пресете
    {
	   uint8_t bpenc = encode_preset( bank_pres[bpf_bank], bank_pres[bpf_preset] );
	   decode_preset_dir(bpenc, preset_dir_name);
    }
	else
	{
  	   char* end ;
  	   uint8_t arg1 = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
  	   decode_preset_dir( arg1, preset_dir_name);
	}

	console_dir_remove_wavs(preset_dir_name) ;

   msg_console("END\n") ;
}

#if 0

static void preset_wavs_delete_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{

	emb_string preset_dir_name;
	if ( count==1 ) // удаление в текщем пресете
    {
	   uint8_t bpenc = encode_preset( bank_pres[bpf_bank], bank_pres[bpf_preset] );
	   decode_preset_dir(bpenc, preset_dir_name);
    }
	else
	{
  	   char* end ;
  	   uint8_t arg1 = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
  	   decode_preset_dir( arg1, preset_dir_name);
	}

	console_dir_wavs_remove(preset_dir_name) ;

   msg_console("END\n") ;
}

#endif


static void preset_map0_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	if ( count==1 ) // вывод map0
    {
		uint8_t bp ;
		load_map0(bp);
		i2hex( bp,hex);
		msg_console("%s\n" , hex ) ;
    }
	else // запись map0
	{
  	   char* end ;
  	   uint8_t arg1 = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
  	   save_map0(arg1);
	}
   msg_console("END\n") ;
}

static void preset_map1_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	if ( count==1 ) // вывод map0
    {
		uint8_t bp ;
		load_map1(bp);
		i2hex( bp,hex);
		msg_console("%s\n" , hex ) ;
    }
	else // запись map1
	{
  	   char* end ;
  	   uint8_t arg1 = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
  	   save_map1(arg1);
	}
   msg_console("END\n") ;
}

static void use_map0_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	uint8_t bp ;
	load_map0(bp);
	i2hex( bp,hex);
	const char* a[2] = {0,hex} ;
	direct_program_command_handler( rl , a , 2 ) ;
}

static void use_map1_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	uint8_t bp ;
	load_map1(bp);
	i2hex( bp,hex);
	const char* a[2] = {0,hex} ;
	direct_program_command_handler( rl , a , 2 ) ;
}

static void sw1_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{

   msg_console("%s\n" , sw1_state() ? "high" : "low") ;
   msg_console("END\n") ;
}

/*
static void preset_wavs_info_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{

	emb_string preset_dir_name;
	if ( count==1 ) // в текщем пресете
    {
	   uint8_t bpenc = encode_preset( bank_pres[bpf_bank], bank_pres[bpf_preset] );
	   decode_preset_dir(bpenc, preset_dir_name);
    }
	else
	{
  	   char* end ;
  	   uint8_t arg1 = strtol ( args[1] , &end, 16 );
  	   decode_preset_dir( arg1, preset_dir_name);
	}

	console_dir_wavs_info(preset_dir_name, rl) ;

   msg_console("END\n") ;
}
*/

static void ind_in_out_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	msg_console("%u\n" , (uint32_t)vsqrt(vol_ind_vector[0])) ;
	msg_console("%u\n" , (uint32_t)vsqrt(vol_ind_vector[1])) ;
//	msg_console("%u\n" , (uint32_t)vsqrt(vol_ind_vector[2])) ;
	msg_console("END\n") ;
}

void ConsoleSetCmdHandlers(TReadLine* rl)
{
  SetConsoleCmdDefaultHandlers(rl);

  rl->AddCommandHandler("cc", current_cabinet_command_handler);
  rl->AddCommandHandler("ce", cabinet_enable_command_handler);
  rl->AddCommandHandler("mv", master_volume_command_handler);
  rl->AddCommandHandler("gs", get_state_command_handler);
  rl->AddCommandHandler("eo", early_on_command_handler);
  rl->AddCommandHandler("ev", early_volume_command_handler);
  rl->AddCommandHandler("et", early_type_command_handler);
  rl->AddCommandHandler("po", presence_on_command_handler);
  rl->AddCommandHandler("pv", presence_volume_command_handler);
  rl->AddCommandHandler("lo", lpf_on_command_handler);
  rl->AddCommandHandler("lv", lpf_volume_command_handler);
  rl->AddCommandHandler("ho", hpf_on_command_handler);
  rl->AddCommandHandler("hv", hpf_volume_command_handler);
  rl->AddCommandHandler("eqo", eq_on_command_handler);
  rl->AddCommandHandler("eqv", eq_volume_command_handler);
  rl->AddCommandHandler("eqf", eq_frec_command_handler);
  rl->AddCommandHandler("eqq", eq_q_command_handler);
  rl->AddCommandHandler("pro", preamp_on_command_handler);
  rl->AddCommandHandler("prv", preamp_volume_command_handler);
  rl->AddCommandHandler("prl", preamp_low_command_handler);
  rl->AddCommandHandler("prm", preamp_mid_command_handler);
  rl->AddCommandHandler("prh", preamp_high_command_handler);
  rl->AddCommandHandler("chb", change_bank_command_handler);
  rl->AddCommandHandler("chp", change_pres_command_handler);
  rl->AddCommandHandler("sp", save_pres_command_handler);
  rl->AddCommandHandler("gm", get_mode_command_handler);
  rl->AddCommandHandler("gb", get_bp_command_handler);
  rl->AddCommandHandler("rn", read_name_command_handler);
  rl->AddCommandHandler("esc", esc_command_handler);
  rl->AddCommandHandler("lcc", load_current_cab_command_handler);
//  rl->AddCommandHandler("dcc", delete_current_cab_command_handler);
  rl->AddCommandHandler("pc", direct_program_command_handler);
  rl->AddCommandHandler("go", gate_on_off_command_handler);
  rl->AddCommandHandler("gt", gate_threshold_command_handler);
  rl->AddCommandHandler("gd", gate_decay_command_handler);
  rl->AddCommandHandler("co", compreccor_on_off_command_handler);
  rl->AddCommandHandler("cs", compreccor_sustein_command_handler);
  rl->AddCommandHandler("cv", compreccor_volume_command_handler);
  rl->AddCommandHandler("amtdev", get_amtid_command_handler);
  rl->AddCommandHandler("amtver", get_amtver_command_handler);
  rl->AddCommandHandler("rns", read_full_name_command_handler);
  rl->AddCommandHandler("sw4", sw4_command_handler);

  rl->AddCommandHandler("pwc", preset_wav_copy_command_handler);
  rl->AddCommandHandler("pws", preset_wav_store_command_handler);
  rl->AddCommandHandler("pwl", preset_wav_load_command_handler);
  rl->AddCommandHandler("pwsd", preset_wavs_delete_command_handler); // удаляет все *.wav в директории текущего пресета или в том на который указывает параметр

  rl->AddCommandHandler("sm0", preset_map0_command_handler);
  rl->AddCommandHandler("sm1", preset_map1_command_handler);
  rl->AddCommandHandler("um0", use_map0_command_handler);
  rl->AddCommandHandler("um1", use_map1_command_handler);
  rl->AddCommandHandler("sw1", sw1_command_handler);

  rl->AddCommandHandler("fsf", fs_format_command_handler);
//  rl->AddCommandHandler("fsd", fs_delete_command_handler);
  rl->AddCommandHandler("fwu", fw_update_command_handler);

#if 0
  rl->AddCommandHandler("preset_wavs_delete", preset_wavs_delete_command_handler); // удаляет все *.wav в директории текущего пресета или в том на который указывает параметр
#endif

  //rl->AddCommandHandler("preset_wavs_info",   preset_wavs_info_command_handler);   //

  rl->AddCommandHandler("iio", ind_in_out_command_handler);

}

//------------------------------------------------------------------------------
