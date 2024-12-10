#pragma once

#include "appdefs.h"
#include "console.h"
#include "preset.h"

enum
{
	PHONES = 0,
	LINE,
	BALANCE,
	MONITOR
};

#define FIRMWARE_STRING_SIZE 20
typedef struct
{
	uint8_t empty[2];
	uint8_t output_mode;
	uint8_t reserved[8];
	uint8_t eol_symb = '\n';
	char firmware_version[FIRMWARE_STRING_SIZE];
#ifdef __LA3_MOD__
	uint8_t la3_cln_preset;
	uint8_t la3_drv_preset;
#endif
}system_parameters_t;

extern system_parameters_t system_parameters;
extern float cab_data[] ;

extern uint8_t impulse_buffer[];
extern char name_buf[];

extern uint8_t bank_pres[];

enum  bank_pres_field_t { bpf_bank=0, bpf_preset=1 } ;

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

extern uint8_t dir_use[];

void EEPROM_folderInit(void);
void EEPROM_loadPreset(uint8_t bank, uint8_t preset, save_data_t& save_data, ir_path_data_t& ir_link);
void EEPROM_savePreset();

void EEPROM_saveSys(void);

bool EEPROM_getPresetIrLink(uint8_t bank, uint8_t preset, ir_path_data_t& outIrLink);
void EEPROM_getPresetCabPath(uint8_t bank, uint8_t preset, ir_path_data_t& outCabPath);
bool EEPROM_loadIr(float* cabData, const std::emb_string& irFilePath, std::emb_string& err_msg);
void EEPROM_getCurrentIrInfo(ir_path_data_t& outIrData, int32_t& resultSize);
bool EEPROM_getDirWavNames(const std::emb_string& dirPath, list<std::emb_string>& fileNamesList, TReadLine* rl);


void delete_current_cab(std::emb_string& err_msg, TReadLine* rl);
bool console_out_currnt_cab(std::emb_string& err_msg, TReadLine* rl);
bool console_fs_format(std::emb_string& err_msg, TReadLine* rl );
bool console_fs_write_file(std::emb_string& err_msg, TReadLine* rl , const char* file_name);
bool EEPROM_delete_file(const char* file_name);

bool console_fs_preset_copy_wav_file(std::emb_string& err_msg, TReadLine* rl , uint8_t preset_src, uint8_t preset_dst); // копирование wav одного bank/preset в другой

void console_dir_remove_wavs(const emb_string& dir_name );

void save_map0(const uint8_t bank_preset);
void load_map0(uint8_t& bank_preset);
void save_map1(const uint8_t bank_preset);
void load_map1(uint8_t& bank_preset);
