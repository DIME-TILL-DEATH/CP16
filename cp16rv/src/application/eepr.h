#pragma once

#include "appdefs.h"
#include "console.h"

extern float cab_data[] ;
extern uint16_t delay_time;
extern uint16_t early_type;
extern volatile uint8_t prog;
extern volatile uint8_t prog1;
extern volatile uint32_t flash_adr;
extern uint8_t prog_data[];
extern uint8_t prog_data_temp[];
extern uint8_t prog_data_temp1[];
extern uint8_t imya[];
extern uint8_t imya1[];
extern const uint8_t imya_init[];
extern uint8_t imya_t [];
extern uint8_t imya1_t[];
extern uint8_t control[];
extern uint8_t sys_para[];
extern uint8_t imp_buf_uint[];
extern char name_buf [];
extern const uint8_t no_loaded[];
extern volatile uint16_t adc_low;
extern volatile uint16_t adc_high;
extern volatile uint16_t adc_val;
extern volatile float adc_val1;



extern uint8_t bank_pres[];enum  bank_pres_field_t { bpf_bank=0, bpf_preset=1 } ;

inline uint8_t encode_preset( const uint8_t bank, const uint8_t preset )
{
    return ((bank & 0xf) << 4) | (preset & 0xf) ;
}

inline auto decode_preset(const uint32_t val)
{
	struct bank_preset_t { uint8_t bank; uint8_t preset ; } bp = { (val >> 4) & 0xf , val & 0xf } ;
	return bp ;
}

inline void decode_preset( uint8_t* bp,  const uint32_t val)
{
	auto [bank, preset] = decode_preset(val);
	bp[bpf_bank]   = bank;
	bp[bpf_preset]  = preset;
}

inline void decode_preset_dir( const uint8_t val, emb_string& preset_dir_name)
{
	auto [bank, preset] = decode_preset(val);
	preset_dir_name.clear();
	preset_dir_name = "/Bank_";
	preset_dir_name += (size_t)bank;
	preset_dir_name += "/Preset_";
	preset_dir_name += (size_t)preset;
}

inline void next_preset(uint8_t dir , bank_pres_field_t bpf)
{
    if(dir)
    {
	     bank_pres[bpf]++;
	     bank_pres[bpf] &= 3;
    }
    else {
  	  if(bank_pres[bpf] > 0)bank_pres[bpf]--;
  	  else bank_pres[bpf] = 3;
    }
};

extern uint8_t dir_use[];
extern uint8_t cab_fl;

void eepr_write (uint8_t nu);
void eepr_read_prog(uint8_t nu);
void eepr_read_imya(uint8_t nu);
void save_sys(void);
void prog_to_temp(void);
void temp_to_prog(void);
void temp1_to_prog(void);
void prog_to_temp1(void);
void flash_folder_init(void);
bool load_pres(float* cd, std::emb_string& err_msg,uint8_t val);
void save_pres(void);
bool console_out_currnt_cab(std::emb_string& err_msg, TReadLine* rl);
void delete_current_cab(std::emb_string& err_msg, TReadLine* rl);
bool console_out_currnt_nam(std::emb_string& err_msg, TReadLine* rl );
bool console_out_full_nam(std::emb_string& err_msg, TReadLine* rl, bool interface2 );
bool console_fs_format(std::emb_string& err_msg, TReadLine* rl );
bool console_fs_write_file(std::emb_string& err_msg, TReadLine* rl , const char* file_name);
bool console_fs_delete_file(std::emb_string& err_msg, TReadLine* rl , const char* file_name);

bool console_fs_preset_copy_wav_file(std::emb_string& err_msg, TReadLine* rl , uint8_t preset_src, uint8_t preset_dst); // копирование wav одного bank/preset в другой
bool console_fs_preset_load_wav_file_tmp(std::emb_string& err_msg, TReadLine* rl , uint8_t preset);
bool console_fs_preset_store_wav_file_tmp(std::emb_string& err_msg, TReadLine* rl , uint8_t preset);

void console_dir_remove_wavs(const emb_string& dir_name );
//void console_dir_wavs_info(const emb_string& dir_name, TReadLine* rl);

void save_map0(const uint8_t bank_preset);
void load_map0(uint8_t& bank_preset);
void save_map1(const uint8_t bank_preset);
void load_map1(uint8_t& bank_preset);
