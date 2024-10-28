#include "appdefs.h"
#include "eeprom.h"
#include "ADAU/adau1701.h"
#include "ff.h"
#include "console.h"

#include "preset.h"


float cab_data[1024] ;
uint8_t impulse_avaliable;

volatile uint8_t prog;
volatile uint8_t prog1;
volatile uint32_t flash_adr;

system_parameters_t system_parameters;

char name_buf [512];

uint8_t preset_data[128];

uint8_t dir_use[10];
volatile uint32_t fl_st;

uint8_t bank_pres[2] = {0, 0};

constexpr char volume_label[] = FIRMWARE_NAME;

float convert ( uint8_t* in )
{
    int32_t d = 0;
    float out ;
    d |=(in[0])<<8;
    d |=(in[1])<<16;
    d |=(in[2])<<24;

    d = d / 256 ;
    out = d/8388607.0f ;
    return out ;
}

static inline bool dir_get_wav( const emb_string& dir_name,  std::emb_string& wav_file_name )
{
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function assumes non-Unicode configuration */
  #if _USE_LFN
    const size_t sz = _MAX_LFN + 1 ;
    char* lfn = new char[sz];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sz ;
  #endif

  bool result = false ;

  FRESULT res = f_opendir(&dir, dir_name.c_str());                       /* Open the directory */
  if (res == FR_OK)
	  {
		  for (;;)
		  {
			  res = f_readdir(&dir, &fno);                   /* Read a directory item */
			  if (res != FR_OK || fno.fname[0] == 0)
			  {
				  result = false ;  /* Break on error or end of dir */
				  break ;
			  }

       	      #if _USE_LFN
                 fn = *fno.lfname ? fno.lfname : fno.fname;
                 uint32_t i = 0;
                 while(fn[i])name_buf[i] = fn[i++];
                 name_buf[i] = 0;
       	      #else
                 fn = fno.fname;
       	      #endif
              if ( fno.fattrib & AM_DIR)
                   continue ;
              else
                {                    /* It is a directory */
                   emb_string fn_name = fn ;
                   reverse(fn_name.begin(),fn_name.end()) ;
                   emb_string tmp = "vaw." ;
                   if ( fn_name.find(tmp) == 0 )
                     {
                        fn_name = dir_name;
                        fn_name+= "/" ;
                        fn_name+= fn ;

                        // current cab file name output

                        wav_file_name = fn ;
                        result = true ;
                        break ;
                    }
               }
           }
	  }

	  delete [] lfn ;
	  f_closedir(&dir);

      return result ;
}

static inline void dir_remove_wavs_raw(const emb_string& dir_name )
{
	  emb_string  wav_to_delete ;
	  FRESULT res ;
	  while ( dir_get_wav(dir_name, wav_to_delete ))
	  {
		  res = f_unlink((dir_name + "/" + wav_to_delete).c_str());
	  }
}

#if 0
static inline void dir_info_wavs_raw(const emb_string& dir_name, TReadLine* rl )
{

	emb_string  wav_to_info ;
	  FRESULT res ;

	  DIR dir;         /* Directory object */
	  FILINFO fno;    /* File information */

	  res = f_opendir(&dir, dir_name.c_str());


	  res = f_readdir (&dir, &fno );

	  while (res == FR_OK && fno.fname[0])
	  {
		      msg_console("%s ", fno.fname);
		      char size_str[16] ;
		      kgp_sdk_libc::uitoa(fno.fsize, size_str, 10);
		      msg_console("%s\n", size_str);
		      res = f_readdir (&dir, &fno );               /* Search for next item */
	  }
	  f_closedir(&dir);
      msg_console("END\n") ;
}
#endif

void delete_current_cab(std::emb_string& err_msg, TReadLine* rl)
{
	FATFS fs;
	FRESULT res ;
	FIL file;
	UINT f_size;
	f_mount ( &fs , "0:",  1);
		        emb_string dir_name;
		        emb_string file_name;
		        dir_name = "/Bank_";
		        dir_name += (size_t)bank_pres[0];;
		        dir_name += "/Preset_";
		        dir_name += (size_t)bank_pres[1];

		        // load WAV file
		        FILINFO fno;
		        DIR dir;
		        char *fn;   /* This function assumes non-Unicode configuration */
		        #if _USE_LFN
		           const size_t sz = _MAX_LFN + 1 ;
		           char* lfn = new char[sz];   /* Buffer to store the LFN */
		           fno.lfname = lfn;
		           fno.lfsize = sz ;
		        #endif

		       bool result = false ;

		       res = f_opendir(&dir, dir_name.c_str());                       /* Open the directory */
		        if (res == FR_OK)
		          {
		            for (;;)
		              {
		                 res = f_readdir(&dir, &fno);                   /* Read a directory item */
		                 if (res != FR_OK || fno.fname[0] == 0)
		                   {
		                     err_msg = "no *.wav file in the dir" ;
		                     break;  /* Break on error or end of dir */
		                   }

		                 #if _USE_LFN
		                     fn = *fno.lfname ? fno.lfname : fno.fname;
		                     uint8_t i = 0;
		                     while(fn[i])name_buf[i] = fn[i++];
		                     name_buf[i] = 0;
		                 #else
		                     fn = fno.fname;
		                 #endif
		                 if ( fno.fattrib & AM_DIR)
		                       continue ;
		                 else
		                     {                    /* It is a directory */
		                          emb_string fn_name = fn ;
		                          reverse(fn_name.begin(),fn_name.end()) ;
		                          emb_string tmp = "vaw." ;
		                          if ( fn_name.find(tmp) == 0 )
		                          {
		                                  FIL f ;
		                                  fn_name = dir_name;
		                                  fn_name+= "/" ;
		                                  fn_name+= fn ;

		                                  FRESULT fs_res = f_unlink(fn_name.c_str());
		                          }

		                     }
		                }
		          }
  f_mount(0, "0:", 0);
}

bool console_out_currnt_cab(std::emb_string& err_msg, TReadLine* rl )
{
	bool result = false ;

	FATFS fs;
	FRESULT res ;
	FIL file;
	UINT f_size;
	res = f_mount ( &fs , "0:",  1);

    emb_string dir_name;
    emb_string file_name;
    dir_name = "/Bank_";
    dir_name += (size_t)bank_pres[0];;
    dir_name += "/Preset_";
    dir_name += (size_t)bank_pres[1];

   emb_string ret_msg = "\nFILE_NOT_FIND\n" ;
   result = dir_get_wav( dir_name,  file_name ) ;

   if ( res == FR_OK )
    {
	   emb_string file_path = dir_name + "/" + file_name ;

       res = f_open(&file, file_path.c_str(),  FA_READ );
       if ( res == FR_OK )
          {
        	 char hex[3] = {0,0,0};
        	 UINT br = 0 ;
        	 char byte ;

        	 msg_console( "%s %d\n", file_name.c_str(), f_size(&file) ) ;
        	 TTask::Delay(1);

        	 while(1)
               {
        	   	   res = f_read (&file, &byte, 1 , &br ) ;

                   if ( res != FR_OK) { result = false ; ret_msg = "\nERROR\n" ; break ;}
                   if ( br != 1 )        { result = true  ; ret_msg = "\nEND\n"   ; break ;}

                   i2hex( byte,hex);
                   msg_console( "%s",  hex) ;
                   TTask::Delay(1);
                }
        	  f_close(&file);
            }
      }

	 msg_console("%s" , ret_msg.c_str() ) ;

	 f_mount(0, "0:", 0);
	 return result ;
}


bool console_out_currnt_nam(std::emb_string& err_msg, TReadLine* rl )
{
	FATFS fs;
	        FRESULT res ;
	        bool result;
	        FIL file;
	        UINT f_size;
	        f_mount ( &fs , "0:",  1);
	        emb_string dir_name;
	        dir_name = "/Bank_";
	        dir_name += (size_t)bank_pres[0];
	        dir_name += "/Preset_";
	        dir_name += (size_t)bank_pres[1];

	        FILINFO fno;
	        DIR dir;
	        const size_t sz = _MAX_LFN + 1 ;
	        char* lfn = new char[sz];   /* Buffer to store the LFN */
	        fno.lfname = lfn;
	        fno.lfsize = sz ;

	        f_opendir(&dir, dir_name.c_str());                       /* Open the directory */
	        res = f_findfirst(&dir, &fno, dir_name.c_str() , "*.wav");
	        if(res == FR_OK)
	        {
	        	if(!fno.lfname[0])msg_console("%s\n" , fno.fname) ;
	        	else msg_console("%s\n" , fno.lfname) ;
	        	result = true ;
	        }
	        else {
	        	msg_console("\n") ;
	        	msg_console("\n") ;
	        }
	        f_closedir(&dir);
	        f_mount(0, "0:", 0);
	        return result ;
}

bool console_out_full_nam(std::emb_string& err_msg, TReadLine* rl, bool interface2 )
{
	FATFS fs;
	extern char hex[3];
	        FIL file;
	        UINT f_size;
	        f_mount ( &fs , "0:",  1);
	        emb_string dir_name;
	        emb_string file_name;
	        uint8_t cab_fl;
	        bool result;
	        for(uint8_t i = 0 ; i < 4 ; i++)
	        {
	          for(uint8_t y = 0 ; y < 4 ; y++)
	          {
	        	  dir_name = "/Bank_";
	        	  dir_name += (size_t)i;
	        	  dir_name += "/Preset_";
	        	  dir_name += (size_t)y;
	        	  file_name = dir_name + "/preset.pan";
	              f_open(&file, file_name.c_str() , FA_READ);
	              f_lseek (&file, 8);
	              f_read(&file, &cab_fl , 1 , &f_size);
	              f_close(&file);

	           if ( interface2 )
	           {
	              i2hex( (i << 4) | y , hex);
	              msg_console("%s\n" , hex ) ; // вывод индекса пресета
	           }
	        	  result = dir_get_wav( dir_name,  file_name ) ;

	        	  if (!result)
	        	  {
	        		  if ( interface2 )  msg_console("\t") ;
                      msg_console("*\n") ;
	        	  }
	        	  else
	        	  {

	    		      if ( interface2 )
	    		    	{
		        		  FIL file ;
		        		  f_open(&file, (dir_name + "/" + file_name).c_str() , FA_READ);
		    		      char size_str[16] ;
		    		      kgp_sdk_libc::uitoa(f_size(&file), size_str, 10);
	    		    	  msg_console("\t%s %s\n" , file_name.c_str(), size_str ) ;
	    		    	}
	        		  else
	        		    {
		        		  msg_console("%s\n" , file_name.c_str() ) ;
	        		    }
	        		  f_close(&file);
	        	  }




	        	  i2hex(cab_fl,hex);
	        	  if (interface2) msg_console("\t" , hex ) ;
	        	  msg_console("%s\n" , hex ) ;
	          }
	        }

	        msg_console("END\n") ;

	         f_mount(0, "0:", 0);
	         return result ;
}

bool console_fs_format(std::emb_string& err_msg, TReadLine* rl )
{
	FRESULT res  ;
	FATFS fs;
	if ( (res = f_mount ( &fs , "0:",  1)) == FR_OK )
		if ( (res = f_mkfs("0:",0,0)) == FR_OK )
		{
		    string tmp = volume_label ;
		    tmp.erase(std::remove(tmp.begin(), tmp.end(), '.'), tmp.end());

			if ( (res = f_setlabel(tmp.c_str())) == FR_OK )
			      if ( (res = f_mount ( 0 , "0:",  1)) == FR_OK ) {} ;
		}

    msg_console("%s\n" , f_err2str(res));
	return res == FR_OK ;
}

bool console_fs_delete_file(std::emb_string& err_msg, TReadLine* rl , const char* file_name)
{
	FRESULT res  ;
	FATFS fs;
	if ( (res = f_mount ( &fs , "0:",  1)) == FR_OK )
		// удаление файла
		if ( (res = f_unlink (file_name)) == FR_OK )
			 res = f_mount ( 0 , "0:",  1);

    msg_console("%s\n" , f_err2str(res));
	return res == FR_OK ;
}


bool console_fs_write_file(std::emb_string& err_msg, TReadLine* rl , const char* file_name)
{
	constexpr size_t chunk_buff_size = 4096 ;
	char* chunk = new char[chunk_buff_size] ;
	if ( !chunk ) {err_msg="BUF_ALLOC_ERROR\n"; return false; }

	// запрос размера куска файла
	{
	  char size_str[16] ;
	  kgp_sdk_libc::uitoa(chunk_buff_size, size_str, 10);
	  rl->SendString("CHUNK_MAX_SIZE_") ;
	  rl->SendString(size_str) ;
	  rl->SendString("\n") ;

	}

	std::emb_string str  ;
	FRESULT res  ;
	FATFS fs;
	FIL file;
	if ( (res = f_mount ( &fs , "0:",  1)) == FR_OK )
	 {
		// запись файла
		if ( (res = f_open (&file, file_name, FA_CREATE_ALWAYS | FA_WRITE )) == FR_OK )
		{
			UINT bytes ;

			while(1)
			{
				// запрос размера
				rl->SendString("REQUEST_CHUNK_SIZE\n") ;
				// прием размера куска
			    rl->RecvLine(str);

			    char* end ;
			    uint32_t chunk_size = kgp_sdk_libc::strtol ( str.c_str() , &end, 10 );
			    if ( (chunk_size == 0) || (chunk_size > chunk_buff_size ) )
			    	{
			    	   res = f_close (&file);
			    	   break ;
			    	}
			    // прием куска данных
			    rl->RecvBuf(chunk, chunk_size);
			    res = f_write (&file, chunk, chunk_size, &bytes) ;
			}
		}

		if (res == FR_OK)
			res = f_mount ( 0 , "0:",  1) ;
		else
			      f_mount ( 0 , "0:",  1) ;
	 }




	delete [] chunk ;


    msg_console("%s\n" , f_err2str(res));
	return res == FR_OK ;
}


void save_pres(void)
{
	FATFS fs;
	FIL file;
	UINT f_size;
	DWORD seek = 0;

	f_mount (&fs, "0:", 1);

	emb_string dir;
	dir = "/Bank_";
	dir += (size_t) bank_pres[0];
	dir += "/Preset_";
	dir += (size_t) bank_pres[1];
	dir += "/preset.pan";
	f_open(&file, dir.c_str() , FA_WRITE);

	f_lseek (&file , seek);
	f_write(&file, preset_data, 128 , &f_size);
	f_sync(&file);
	f_close(&file);
	f_mount(0, "0:", 0);
}

bool load_pres(float* cd, std::emb_string& err_msg, uint8_t val)
{
	FATFS fs;
	FRESULT res;
	FIL file;
	UINT f_size;

	f_mount (&fs, "0:", 1);

	emb_string dir_name;
	emb_string file_name;
	dir_name = "/Bank_";
	dir_name += (size_t)bank_pres[0];
	dir_name += "/Preset_";
	dir_name += (size_t)bank_pres[1];

	if(val)
	{
		file_name = dir_name + "/preset.pan";
		f_open(&file, file_name.c_str() , FA_READ);
		f_read(&file, preset_data , 128 , &f_size);
		f_close(&file);
	}

	// load WAV file
	FILINFO fno;
	DIR dir;
	char *fn;   /* This function assumes non-Unicode configuration */
	#if _USE_LFN
	   const size_t sz = _MAX_LFN + 1 ;
	   char* lfn = new char[sz];   /* Buffer to store the LFN */
	   fno.lfname = lfn;
	   fno.lfsize = sz ;
	#endif

       bool result = false ;

       res = f_opendir(&dir, dir_name.c_str());                       /* Open the directory */
        if (res == FR_OK)
          {
            for (;;)
              {
                 res = f_readdir(&dir, &fno);                   /* Read a directory item */
                 if (res != FR_OK || fno.fname[0] == 0)
                   {
                     err_msg = "no *.wav file in the dir" ;
                     break;  /* Break on error or end of dir */
                   }

                 #if _USE_LFN
                     fn = *fno.lfname ? fno.lfname : fno.fname;
                     uint8_t i = 0;
                     while(fn[i])name_buf[i] = fn[i++];
                     name_buf[i] = 0;
                 #else
                     fn = fno.fname;
                 #endif
                 if ( fno.fattrib & AM_DIR)
                       continue ;
                 else
                     {                    /* It is a directory */
                          emb_string fn_name = fn ;
                          reverse(fn_name.begin(),fn_name.end()) ;
                          emb_string tmp = "vaw." ;
                          if ( fn_name.find(tmp) == 0 )
                          {
                                  FIL f ;
                                  fn_name = dir_name;
                                  fn_name+= "/" ;
                                  fn_name+= fn ;
                                  FRESULT fs_res = f_open(&f, fn_name.c_str(),  FA_READ );
                                  fs_res = f_lseek (&f, 0) ;
                                  //  read header and check as RIFF
                                  char header[5] = {0,0,0,0,0} ;
                                  UINT br ;
                                  fs_res = f_read (&f, header  , 4 , &br ) ;
                                  if ( kgp_sdk_libc::strcmp ( header,"RIFF" ) )
                                    {
                                      err_msg = "unsupported format  (no RIFF headr)" ;
                                      break ;
                                    }
                                  // read header and check as MONO
                                  fs_res = f_lseek (&f, 22) ;
                                  uint16_t chanal_count ;
                                  fs_res = f_read (&f, (void*)&chanal_count  , 2 , &br ) ;
                                  if ( chanal_count != 1 )
                                        {
                                          err_msg = "unsupported format  (no MONO chanal sets)" ;
                                          break ;
                                        }
                                  // read header and check as sample rate 48000
                                  fs_res = f_lseek (&f, 24) ;
                                  uint32_t sample_rate ;
                                  fs_res = f_read (&f, (void*)&sample_rate  , 4 , &br ) ;
                                  if ( sample_rate != 48000 )
                                        {
                                          err_msg = "unsupported format  (no 48000 sample rate sets)" ;
                                          break ;
                                        }
                                  // read header and check as 24 bits

                                  fs_res = f_lseek (&f, 34) ;
                                  uint8_t sample_bits ;
                                  fs_res = f_read (&f, (void*)&sample_bits  , 1 , &br ) ;
                                  if ( sample_bits != 24 )
                                        {
                                          err_msg = "unsupported format  (no 24 sample bits sets)" ;
                                          break ;
                                        }
                                  // read cabinet size
                                  fs_res = f_lseek (&f, 40) ;
                                  uint16_t cab_size;
                                  fs_res = f_read (&f, (void*)&cab_size  , 2 , &br ) ;
                                  cab_size /= 3;
                                  if(cab_size > 984)cab_size = 984;
                                  // read cabinet data
                                  fs_res = f_lseek (&f, 44) ;
                                  size_t i;
                                  for ( i = 0 ; i < cab_size ; i++ )
                                        {
                                          uint8_t buff[4] ;
                                          fs_res = f_read (&f, (void*)buff  , 3 , &br ) ;
                                          cd[i] = convert ( buff ) ;
                                        }
                                  if(i < 983)
                                  {
                                	  for(; i < 984 ; i++)
                                		  {
                                		    uint8_t buff[4] = {0,0,0,0};
                                		    cd[i] = convert ( buff ) ;
                                		  }
                                  }
                                  // закрытие файла
                                  fs_res = f_close(&f) ;
                                  err_msg = "read cab data OK" ;
                                  result = true ;
                                  break ;
                          }
                     }
                }
          }
         delete [] lfn ;
         f_closedir(&dir);
         f_mount(0, "0:", 0);
         return result ;
}

void flash_folder_init(void)
{
	FRESULT res;
	DIR dir;
	FATFS fs;
	FIL file;
	UINT bytes_readed;
	FILINFO fno;

#if _USE_LFN
	const size_t sz = _MAX_LFN + 1 ;
	char* lfn = new char[sz];   /* Buffer to store the LFN */
	fno.lfname = lfn;
	fno.lfsize = sz ;
#endif

	if(f_mount ( &fs , "0:",  1) != FR_OK)
	{
		f_mkfs("0:", 0, 0);
		f_setlabel(volume_label);
		f_mount(&fs, "0:", 1);
	}

	emb_string hl_dir ;
	for(size_t i = 0 ; i < 4 ; i++)
	{
		hl_dir = "/Bank_";
		hl_dir += i;
		res = f_opendir(&dir, hl_dir.c_str());
		f_closedir(&dir);
		if(res != FR_OK) res = f_mkdir(hl_dir.c_str());

		emb_string ll_dir ;
		uint8_t z = 0;
		for(size_t j = 0 ; j < 4 ; j++)
		{
			ll_dir = hl_dir + "/Preset_" ;
			ll_dir += j ;
			res = f_opendir(&dir,ll_dir.c_str());
			if(res != FR_OK)res = f_mkdir(ll_dir.c_str());
			ll_dir += "/preset.pan" ;
			res = f_open(&file, ll_dir.c_str() , FA_READ | FA_WRITE | FA_OPEN_ALWAYS) ;
			if(file.fsize == 0) f_write(&file, &default_legacy_preset, sizeof(preset_data_legacy_t), &bytes_readed);
			f_close(&file);

			while(1)
			{
				res = f_readdir(&dir, &fno);
				if((res != FR_OK) || (fno.fname[0] == 0)) break;
				if(fno.fname[0] != 46) z += 1;
			}
			z -= 1;
			f_closedir(&dir);
		}
		if(z)dir_use[i] = 1;
		else dir_use[i] = 0;
	}

	res = f_open(&file, "/system.pan" , FA_READ | FA_WRITE | FA_OPEN_ALWAYS) ;
	f_read(&file, &system_parameters, sizeof(system_parameters), &bytes_readed);

#ifdef __LA3_MOD__
	if(bytes_readed(&file) == 32) // запись map0, map1 по умолчанию
	{
		size_t fsize ;
		f_lseek (&file, 32);
		uint16_t map  = 0x1122 ; // дефолтные пресеты по умолчанию при инициализации
		f_write(&file, &map, 2, &fsize);
		f_sync(&file);
	}
#endif

	f_close(&file);

	res = f_opendir(&dir, "/tmp_preset");
	f_closedir(&dir);
	if(res != FR_OK)res = f_mkdir("/tmp_preset");

	f_mount(0, "0:", 0);
}

void save_sys(void)
{
	FATFS fs;
	FIL file;
	UINT bytes_written;
	DWORD seek = 0;
	f_mount ( &fs , "0:",  1);
	f_open(&file, "/system.pan", FA_WRITE);
	f_lseek (&file , seek);
	f_write(&file, &system_parameters, sizeof(system_parameters), &bytes_written);
	f_sync(&file);
	f_close(&file);
	f_mount(0, "0:", 0);
}
//-------------------------------------------------------------------------------------
bool console_fs_preset_copy_wav_file(std::emb_string& err_msg, TReadLine* rl , uint8_t preset_src, uint8_t preset_dst)
{
  uint8_t bank_pres_src[2] ;
  uint8_t bank_pres_dst[2] ;

  decode_preset(bank_pres_src, preset_src);

  decode_preset(bank_pres_dst, preset_dst);


  bool result = false ;
  FATFS fs;
  FRESULT res ;

  f_mount ( &fs , "0:",  1);

  FIL f_src ;
  FIL f_dst ;

  emb_string dir_name_src;
  emb_string file_name_src;

  dir_name_src = "/Bank_";
  dir_name_src += (size_t)bank_pres_src[0];
  dir_name_src += "/Preset_";
  dir_name_src += (size_t)bank_pres_src[1];



  emb_string dir_name_dst;
  emb_string file_name_dst;

  dir_name_dst = "/Bank_";
  dir_name_dst += (size_t)bank_pres_dst[0];
  dir_name_dst += "/Preset_";
  dir_name_dst += (size_t)bank_pres_dst[1];


  dir_remove_wavs_raw(dir_name_dst) ;

  if ( dir_get_wav( dir_name_src,  file_name_src ))
  {
	  res = f_open(&f_src, file_name_src.c_str(),  FA_READ );
	  res = f_open(&f_dst, (dir_name_dst + "/" + file_name_src).c_str(),  FA_CREATE_NEW );


	   char* cb = new char [_MIN_SS] ;

	   res = f_lseek (&f_src, 0) ;
	   res = f_lseek (&f_dst, 0) ;

	   UINT br ;
	   UINT bw ;
	   do
	     {
	       f_read  (&f_src, cb  , _MIN_SS , &br ) ;\
	       if ( br )
	           f_write (&f_dst, cb  , br , &bw ) ;
	     } while ( br == _MIN_SS ) ;

	   delete [] cb ;

	   f_close(&f_src) ;
	   f_close(&f_dst) ;
  }



   f_mount(0, "0:", 0);
   return result ;
}
//-------------------------------------------------------------------------------------
bool console_fs_preset_store_wav_file_tmp(std::emb_string& err_msg, TReadLine* rl , uint8_t preset)
{
	  uint8_t bank_pres_src[2] ;

	  decode_preset(bank_pres_src, preset);


	  bool result = false ;
	  FATFS fs;
	  FRESULT res ;
	  FIL file;
	  UINT f_size;
	  f_mount ( &fs , "0:",  1);

	  emb_string tmp_preset_dir = "/tmp_preset" ;

	  FIL f_src ;
	  FIL f_tmp ;

	  emb_string dir_name_src;
	  emb_string file_name_src;

	  dir_name_src = "/Bank_";
	  dir_name_src += (size_t)bank_pres_src[0];
	  dir_name_src += "/Preset_";
	  dir_name_src += (size_t)bank_pres_src[1];



	  emb_string dir_name_tmp = "/";
	  emb_string file_name_tmp;

	  // load WAV file
	  FILINFO fno_src, fno_tmp;
	  DIR dir_src, dir_tmp ;
	  char *fn;   /* This function assumes non-Unicode configuration */
	  #if _USE_LFN
	     const size_t sz = _MAX_LFN + 1 ;
	     char* lfn = new char[sz];   /* Buffer to store the LFN */
	     fno_src.lfname = lfn;
	     fno_src.lfsize = sz ;

	     fno_tmp.lfname = lfn;
	     fno_tmp.lfsize = sz ;

	  #endif



	  if (f_opendir(&dir_src, dir_name_src.c_str()) == FR_OK)
	    {
	      for (;;)
	        {
	           res = f_readdir(&dir_src, &fno_src);                   /* Read a directory item */
	           if (res != FR_OK || fno_src.fname[0] == 0)
	             {
	               err_msg = "no *.wav file in the dir" ;
	               break;  /* Break on error or end of dir */
	             }

	           #if _USE_LFN
	               fn = *fno_src.lfname ? fno_src.lfname : fno_src.fname;
	               uint8_t i = 0;
	               while(fn[i])name_buf[i] = fn[i++];
	               name_buf[i] = 0;
	           #else
	               fn = fno_src.fname;
	           #endif


	           if ( fno_src.fattrib & AM_DIR)
	                 continue ;
	           else
	               {                    /* It is a directory */
	                    emb_string fn_name = fn ;
	                    reverse(fn_name.begin(),fn_name.end()) ;
	                    emb_string tmp = "vaw." ;
	                    if ( fn_name.find(tmp) == 0 )
	                    {
	                            fn_name = dir_name_src;
	                            fn_name+= "/" ;
	                            fn_name+= fn ;

	                            file_name_src = fn ;

	                            res = f_open(&f_src, fn_name.c_str(),  FA_READ );

	                            if (res == FR_OK)
	                              {
	                          	    // удаление  всех wav в таргете директории
	                          	    dir_remove_wavs_raw(tmp_preset_dir);
	                              }

	                            tmp = tmp_preset_dir + "/" + file_name_src ;

								res = f_open(&f_tmp, tmp.c_str() ,  FA_CREATE_ALWAYS | FA_WRITE );


	                            char* cb = new char [_MIN_SS] ;
	                     	    res = f_lseek (&f_src, 0) ;
	                     	    res = f_lseek (&f_tmp, 0) ;

	                     	    UINT br ;
	                     	    UINT bw ;
	                     	    do
	                     	     {
	                     	       f_read  (&f_src, cb  , _MIN_SS , &br ) ;\
	                     	       if ( br )
	                     	           f_write (&f_tmp, cb  , br , &bw ) ;
	                     	     } while ( br == _MIN_SS ) ;

	                     	    delete [] cb ;



	                            break ;
	                    }
	               }
	             // в данном пресете нет wav, сохранять нечего
	             // xbcnbv /tmp
	             dir_remove_wavs_raw(tmp_preset_dir);
	          }
	    }

	    f_close(&f_src) ;
	    f_close(&f_tmp) ;

	    delete [] lfn ;
	    f_closedir(&dir_src);
	    f_closedir(&dir_tmp);

	   f_mount(0, "0:", 0);
	   return result ;
}
//-------------------------------------------------------------------------------------
bool console_fs_preset_load_wav_file_tmp(std::emb_string& err_msg, TReadLine* rl , uint8_t preset)
{
	  uint8_t bank_pres_dst[2] ;

	  decode_preset(bank_pres_dst, preset);


	  bool result = false ;
	  FATFS fs;
	  FRESULT res ;
	  FIL file;
	  UINT f_size;
	  f_mount ( &fs , "0:",  1);

	  FIL f_src ;
	  FIL f_dst ;



	  emb_string dir_name_dst;
	  emb_string file_name_dst;

	  dir_name_dst = "/Bank_";
	  dir_name_dst += (size_t)bank_pres_dst[0];
	  dir_name_dst += "/Preset_";
	  dir_name_dst += (size_t)bank_pres_dst[1];

	  // удаление  всех wav в таргете директории
	  dir_remove_wavs_raw(dir_name_dst);




	  emb_string  dir_name = "/tmp_preset" ;
	  emb_string  wav_file_name ;
	  result = dir_get_wav(dir_name, wav_file_name );


	  if ( !result )
	  {
		  f_mount(0, "0:", 0);
		  return false ;
	  }

	  if ( (res = f_open(&f_src, (dir_name + "/" + wav_file_name).c_str(),  FA_READ )) != FR_OK)
	    {
		  f_mount(0, "0:", 0);
		  return false ;
	    }

	  // load WAV file
	  FILINFO fno_src, fno_dst;
	  DIR dir_src, dir_dst ;
	  char *fn;   /* This function assumes non-Unicode configuration */
	  #if _USE_LFN
	     const size_t sz = _MAX_LFN + 1 ;
	     char* lfn = new char[sz];   /* Buffer to store the LFN */
	     fno_src.lfname = lfn;
	     fno_src.lfsize = sz ;

	     fno_dst.lfname = lfn;
	     fno_dst.lfsize = sz ;

	  #endif

	  if (f_opendir(&dir_dst, dir_name_dst.c_str()) == FR_OK) // проверка наличия таргет директории
	    {

	       for (;;)
	         {
	                 res = f_readdir(&dir_dst, &fno_dst);                   /* Read a directory item */
	                 if (res != FR_OK || fno_dst.fname[0] == 0)
	                   {
	                	 emb_string fn_name = dir_name_dst;
	                	 fn_name+= "/" ;

                         fn_name+= wav_file_name  ;

	                	 res = f_open(&f_dst, fn_name.c_str(),  FA_CREATE_ALWAYS | FA_WRITE );

	              	     char* cb = new char [_MIN_SS] ;

	              	     res = f_lseek (&f_src, 0) ;
	              	     res = f_lseek (&f_dst, 0) ;

	              	     UINT br ;
	              	     UINT bw ;
	              	     do
	              	     {
	              	       f_read  (&f_src, cb  , _MIN_SS , &br ) ;\
	              	       if ( br )
	              	           f_write (&f_dst, cb  , br , &bw ) ;
	              	     } while ( br == _MIN_SS ) ;

	              	     delete [] cb ;

	                     break;
	                   }
	                }
	    }



	   f_close(&f_src) ;
	   f_close(&f_dst) ;

	   delete [] lfn ;
	   f_closedir(&dir_src);
	   f_closedir(&dir_dst);

	   f_mount(0, "0:", 0);
	   return result ;
}


void console_dir_remove_wavs(const emb_string& dir_name )
{
	  FATFS fs;
	  f_mount ( &fs , "0:",  1);
	  dir_remove_wavs_raw(dir_name) ;
	  f_mount(0, "0:", 0);
}

/*
void console_dir_wavs_info(const emb_string& dir_name, TReadLine* rl)
{
	  FATFS fs;
	  f_mount ( &fs , "0:",  1);
	  dir_info_wavs_raw(dir_name, rl) ;
	  f_mount(0, "0:", 0);
}
*/


void save_map0(const uint8_t bank_preset)
{
	FATFS fs;
	FIL file;
	UINT f_size;
	DWORD seek = 32;
	f_mount ( &fs , "0:",  1);
	f_open(&file, "/system.pan" , FA_WRITE);
	f_lseek (&file , seek);
	f_write(&file, &bank_preset , 1 , &f_size);
	f_sync(&file);
	f_close(&file);
	f_mount(0, "0:", 0);
}

void load_map0(uint8_t& bank_preset)
{
	FATFS fs;
	FIL file;
	UINT f_size;
	DWORD seek = 32;
	f_mount ( &fs , "0:",  1);
	f_open(&file, "/system.pan" , FA_READ);
	f_lseek (&file , seek);
	f_read(&file, &bank_preset , 1 , &f_size);
	f_close(&file);
	f_mount(0, "0:", 0);
}

void save_map1(const uint8_t bank_preset)
{
	FATFS fs;
	FIL file;
	UINT f_size;
	DWORD seek = 33;
	f_mount ( &fs , "0:",  1);
	f_open(&file, "/system.pan" , FA_WRITE);
	f_lseek (&file , seek);
	f_write(&file, &bank_preset , 1 , &f_size);
	f_sync(&file);
	f_close(&file);
	f_mount(0, "0:", 0);
}

void load_map1(uint8_t& bank_preset)
{
	FATFS fs;
	FIL file;
	UINT f_size;
	DWORD seek = 33;
	f_mount ( &fs , "0:",  1);
	f_open(&file, "/system.pan" , FA_READ);
	f_lseek (&file, seek);
	f_read(&file, &bank_preset, 1 , &f_size);
	f_close(&file);
	f_mount(0, "0:", 0);
}
