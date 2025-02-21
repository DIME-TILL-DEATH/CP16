#include "appdefs.h"
#include "eeprom.h"
#include "ADAU/adau1701.h"
#include "ff.h"
#include "console.h"

#include "preset.h"
#include <list.h>

float __CCM_BSS__ cab_data[1024];

system_parameters_t system_parameters;

uint8_t dir_use[10];

uint8_t bank_pres[2] = { 0, 0 };

constexpr char volume_label[] = FIRMWARE_NAME;

float convertToFloat(uint8_t *in) {
	int32_t d = 0;
	float out;
	d |= (in[0]) << 8;
	d |= (in[1]) << 16;
	d |= (in[2]) << 24;

	d = d / 256;
	out = d / 8388607.0f;
	return out;
}

void cleanCabData(){
	for(int i=0; i< DSP_FIR_SIZE; i++)
		cab_data[i] = 0;
}

void init_file_info(FILINFO &fileInfo)
{
#if _USE_LFN
	const size_t sz = _MAX_LFN + 1;
	char *lfn = new char[sz]; /* Buffer to store the LFN */
	fileInfo.lfname = lfn;
	fileInfo.lfsize = sz;
#endif
}

void free_file_info(FILINFO &fileInfo)
{
#if _USE_LFN
	free(fileInfo.lfname);
#endif
}

void EEPROM_folderInit(void) {
	FRESULT res;
	DIR dir;
	FATFS fs;
	FIL file;
	UINT bytes_readed;
	FILINFO fno;

#if _USE_LFN
	const size_t sz = _MAX_LFN + 1;
	char *lfn = new char[sz]; /* Buffer to store the LFN */
	fno.lfname = lfn;
	fno.lfsize = sz;
#endif

	if (f_mount(&fs, "0:", 1) != FR_OK) {
		f_mkfs("0:", 0, 0);
		f_setlabel(volume_label);
		f_mount(&fs, "0:", 1);
	}

	emb_string hl_dir;
	hl_dir = "/ir_library";
	res = f_opendir(&dir, hl_dir.c_str());
	f_closedir(&dir);
	if (res != FR_OK)
		res = f_mkdir(hl_dir.c_str());

	for (size_t i = 0; i < 4; i++) {
		hl_dir = "/Bank_";
		hl_dir += i;
		res = f_opendir(&dir, hl_dir.c_str());
		f_closedir(&dir);
		if (res != FR_OK)
			res = f_mkdir(hl_dir.c_str());

		emb_string ll_dir;
		uint8_t z = 0;
		for (size_t j = 0; j < 4; j++) {
			ll_dir = hl_dir + "/Preset_";
			ll_dir += j;
			res = f_opendir(&dir, ll_dir.c_str());

			if (res != FR_OK)
				res = f_mkdir(ll_dir.c_str());
			ll_dir += "/preset.pan";
			res = f_open(&file, ll_dir.c_str(), FA_READ | FA_WRITE | FA_OPEN_ALWAYS);

			if (file.fsize == 0)
				f_write(&file, &default_legacy_preset, sizeof(preset_data_legacy_t), &bytes_readed);
			f_close(&file);

			while (1) {
				res = f_readdir(&dir, &fno);
				if ((res != FR_OK) || (fno.fname[0] == 0))
					break;
				if (fno.fname[0] != 46)
					z += 1;
			}

			z -= 1;
			f_closedir(&dir);
		}
		if (z)
			dir_use[i] = 1;
		else
			dir_use[i] = 0;
	}

	res = f_open(&file, "/system.pan", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	f_read(&file, &system_parameters, sizeof(system_parameters), &bytes_readed);

#ifdef __LA3_MOD__
	if(bytes_readed < sizeof(system_parameters_t)) // запись map0, map1 по умолчанию
	{
		system_parameters.la3_cln_preset = 0x00;
		system_parameters.la3_drv_preset = 0x20;
	}
#endif

#if _USE_LFN
	free(lfn);
#endif

	f_close(&file);
	f_mount(0, "0:", 0);
}

void EEPROM_saveSys(void){
	FATFS fs;
	FIL file;
	UINT bytes_written;
	DWORD seek = 0;
	f_mount(&fs, "0:", 1);
	f_open(&file, "/system.pan", FA_WRITE);
	f_lseek(&file, seek);
	f_write(&file, &system_parameters, sizeof(system_parameters), &bytes_written);
	f_sync(&file);
	f_close(&file);
	f_mount(0, "0:", 0);
}

static inline bool dir_get_wav(const emb_string &dir_name, std::emb_string &wav_file_name){
	FILINFO fno;
	DIR dir;
	char *fn; /* This function assumes non-Unicode configuration */

#if _USE_LFN
	const size_t sz = _MAX_LFN + 1;
	char *lfn = new char[sz]; /* Buffer to store the LFN */
	fno.lfname = lfn;
	fno.lfsize = sz;
#endif

	bool result = false;

	FRESULT res = f_opendir(&dir, dir_name.c_str()); /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0) {
				result = false; /* Break on error or end of dir */
				break;
		}

#if _USE_LFN
			fn = *fno.lfname ? fno.lfname : fno.fname;
#else
				fn = fno.fname;
			#endif
			if (fno.fattrib & AM_DIR)
				continue;
			else { /* It is a directory */
				emb_string fn_name = fn;
				reverse(fn_name.begin(), fn_name.end());
				emb_string tmp = "vaw.";
				if (fn_name.find(tmp) == 0) {
					fn_name = dir_name;
					fn_name += "/";
					fn_name += fn;

					// current cab file name output
					wav_file_name = fn;
					result = true;
					break;
				}
			}
		}
	}

	delete[] lfn;
	f_closedir(&dir);

	return result;
}

bool console_out_currnt_cab(std::emb_string &err_msg, TReadLine *rl){
	bool result = false;

	FATFS fs;
	FRESULT res;
	FIL file;
//	UINT f_size;
	res = f_mount(&fs, "0:", 1);

	emb_string dir_name;
	emb_string file_name;
	dir_name = "/Bank_";
	dir_name += (size_t) bank_pres[0];

	dir_name += "/Preset_";
	dir_name += (size_t) bank_pres[1];

	emb_string ret_msg = "\nFILE_NOT_FIND\n";
	result = dir_get_wav(dir_name, file_name);

	if (res == FR_OK){
		emb_string file_path = dir_name + "/" + file_name;

		res = f_open(&file, file_path.c_str(), FA_READ);
		if (res == FR_OK) {
			char hex[3] = { 0, 0, 0 };
			UINT br = 0;
			char byte;

			msg_console("%s %d\n", file_name.c_str(), f_size(&file));
			TTask::Delay(1);

			while (1){
				res = f_read(&file, &byte, 1, &br);

				if (res != FR_OK) {
					result = false;
					ret_msg = "\nERROR\n";
					break;
				}
				if (br != 1) {
					result = true;
					ret_msg = "\nEND\n";
					break;
				}

				i2hex(byte, hex);
				msg_console( "%s", hex);
				TTask::Delay(1);
			}
			f_close(&file);
		}
	}

	msg_console("%s" , ret_msg.c_str() );

	f_mount(0, "0:", 0);
	return result;
}

bool console_fs_format(std::emb_string &err_msg, TReadLine *rl) {
	FRESULT res;
	FATFS fs;
	if ((res = f_mount(&fs, "0:", 1)) == FR_OK){
		if ((res = f_mkfs("0:", 0, 0)) == FR_OK) {
			string tmp = volume_label;
			tmp.erase(std::remove(tmp.begin(), tmp.end(), '.'), tmp.end());

			if ((res = f_setlabel(tmp.c_str())) == FR_OK)
				if ((res = f_mount(0, "0:", 1)) == FR_OK) {
				};
		}
	}
	msg_console("%s\n", f_err2str(res));
	return res == FR_OK;
}

bool EEPROM_delete_file(const char *file_name) {
	FRESULT res;
	FATFS fs;
	if ((res = f_mount(&fs, "0:", 1)) == FR_OK) {
		res = f_unlink(file_name);
		res = f_mount(0, "0:", 1);
	}
	return res == FR_OK;
}

bool EEPROM_console_write_file(std::emb_string &err_msg, TReadLine *rl, const char *file_name) {
	constexpr size_t chunk_buff_size = 2048;
	char *chunk = new char[chunk_buff_size];
	if (!chunk)
	{
		msg_console("BUF_ALLOC_ERROR\n");
		return false;
	}

	// запрос размера куска файла
	{
		char size_str[16];
		kgp_sdk_libc::uitoa(chunk_buff_size, size_str, 10);
		rl->SendString("CHUNK_MAX_SIZE_");
		rl->SendString(size_str);
		rl->SendString("\n");

	}

	std::emb_string str;
	FRESULT res;
	FATFS fs;
	FIL file;
	if ((res = f_mount(&fs, "0:", 1)) == FR_OK)
	{
		// запись файла
		if ((res = f_open(&file, file_name, FA_CREATE_ALWAYS | FA_WRITE)) == FR_OK)
		{
			UINT bytes;

			uint8_t attempts = 0;
			while (1)
			{
				// запрос размера
				rl->SendString("REQUEST_CHUNK_SIZE\n");
				// прием размера куска
				rl->RecvLine(str);

				char *end;
				uint32_t chunk_size = kgp_sdk_libc::strtol(str.c_str(), &end, 10);

				if(chunk_size > chunk_buff_size)
				{
					res = f_close(&file);
					rl->SendString("CHUNK_IS_TOO_BIG\r\n");
					break;
				}


				if(chunk_size == 0)
				{

					if(end == str.c_str())
					{
						rl->SendString("END_PTR\n");
						continue;
					}
					else
					{
						if(attempts < 3)
						{
							attempts++;
							rl->SendString("ZERO_SIZE_RECIEVED\n");
							continue;
						}
						else
						{
							res = f_close(&file);
							break;
						}
					}
				}
				// прием куска данных
				rl->RecvBuf(chunk, chunk_size);
				res = f_write(&file, chunk, chunk_size, &bytes);
			}
		}

		if (res == FR_OK)
			res = f_mount(0, "0:", 1);
		else
			f_mount(0, "0:", 1);
	}

	delete[] chunk;

	msg_console("%s\n", f_err2str(res));
	return res == FR_OK;
}

void EEPROM_loadPreset(uint8_t bank, uint8_t preset, save_data_t &loaded_data, ir_path_data_t &ir_link) {
	FATFS fs;

	FIL file;
	UINT f_size;

	f_mount(&fs, "0:", 1);

	emb_string dir_name;
	emb_string file_name;
	dir_name = "/Bank_";
	dir_name += (size_t) bank;
	dir_name += "/Preset_";
	dir_name += (size_t) preset;

	file_name = dir_name + "/preset.pa2";

	if (f_open(&file, file_name.c_str(), FA_READ) != FR_OK) {
		file_name = dir_name + "/preset.pan";
		f_open(&file, file_name.c_str(), FA_READ);

		uint8_t legacy_preset_data[128];
		f_read(&file, legacy_preset_data, 128, &f_size);

		preset_data_legacy_t *convert_struct = (preset_data_legacy_t*) &legacy_preset_data;
		preset_from_legacy(&loaded_data.parametersData, convert_struct);

		kgp_sdk_libc::memset(loaded_data.name, 0, PRESET_NAME_LENGTH);
	} else {
		f_read(&file, &loaded_data, sizeof(save_data_t), &f_size);
	}

	if (!EEPROM_getPresetIrLink(bank, preset, ir_link)) {
		ir_link.irFileName.clear();
		ir_link.irLinkPath.clear();
	}

	f_close(&file);
	f_mount(0, "0:", 0);
}

void EEPROM_savePreset(void) {
	FATFS fs;
	FIL file;
	UINT f_size;
	DWORD seek = 0;

	f_mount(&fs, "0:", 1);

	emb_string dir_name;
	emb_string file_name;
	dir_name = "/Bank_";
	dir_name += (size_t) bank_pres[0];
	dir_name += "/Preset_";
	dir_name += (size_t) bank_pres[1];
	file_name = dir_name + "/preset.pan";

	// write legacy data. backward compatibility
	f_open(&file, file_name.c_str(), FA_WRITE);

	uint8_t legacy_preset_data[128];
	legacy_from_preset((preset_data_legacy_t*) legacy_preset_data, &current_preset);

	f_lseek(&file, seek);
	f_write(&file, legacy_preset_data, sizeof(preset_data_legacy_t), &f_size);
	f_sync(&file);
	f_close(&file);

	// write gen2 data
	file_name = dir_name + "/preset.pa2";
	f_open(&file, file_name.c_str(), FA_READ | FA_WRITE | FA_OPEN_ALWAYS);

	save_data_t save_data;
	kgp_sdk_libc::memcpy(&save_data.parametersData, &current_preset, sizeof(preset_data_t));
	kgp_sdk_libc::memcpy(save_data.name, current_preset_name, PRESET_NAME_LENGTH);

	f_lseek(&file, seek);
	f_write(&file, &save_data, sizeof(save_data_t), &f_size);
	f_sync(&file);
	f_close(&file);

	// write link data
	if (current_ir_link.irFileName != "") {
		file_name = dir_name + "/ir.lnk";
		f_open(&file, file_name.c_str(), FA_WRITE | FA_CREATE_ALWAYS);
		f_puts(current_ir_link.irFileName.c_str(), &file);
		f_puts("\n", &file);
		f_puts(current_ir_link.irLinkPath.c_str(), &file);
		f_sync(&file);
		f_close(&file);
	}

	f_mount(0, "0:", 0);
}

bool EEPROM_getPresetIrLink(uint8_t bank, uint8_t preset, ir_path_data_t &outIrLink) {
	FATFS fs;
	FRESULT res;
	f_mount(&fs, "0:", 1);

	bool result = false;

	emb_string dirName;
	emb_string irFilePath;
	dirName = "/Bank_";
	dirName += (size_t) bank; //bank_pres[0];
	dirName += "/Preset_";
	dirName += (size_t) preset; //bank_pres[1];

	FIL file;
	emb_string linkFilePath = dirName + "/" + "ir.lnk";
	res = f_open(&file, linkFilePath.c_str(), FA_READ);
	if (res == FR_OK) {
		TCHAR buf[256];
		ir_path_data_t linkData;
		res = f_lseek(&file, 0);
		f_gets(buf, 256, &file);
		buf[kgp_sdk_libc::strcspn(buf, "\n")] = 0;
		linkData.irFileName = buf;

		f_gets(buf, 256, &file);
		linkData.irLinkPath = buf;
		f_close(&file);

		emb_string irFilePath = linkData.irLinkPath + "/" + linkData.irFileName;

		res = f_open(&file, irFilePath.c_str(), FA_READ);
		if (res == FR_OK) {
			outIrLink.irFileName = linkData.irFileName;
			outIrLink.irLinkPath = linkData.irLinkPath;
			f_close(&file);
			result = true; // wav file exists
		}
	}
	f_mount(0, "0:", 0);
	return result;
}

void EEPROM_getPresetCabPath(uint8_t bank, uint8_t preset, ir_path_data_t &outCabPath){
	if (bank == bank_pres[0] && preset == bank_pres[1]) // data in RAM, not on the flash
	{
		if (current_ir_link.irFileName != "")
		{
			outCabPath = current_ir_link;
			return;
		}
	}
	else
	{
		ir_path_data_t irLinkData;
		if (EEPROM_getPresetIrLink(bank, preset, irLinkData))
		{
			outCabPath = irLinkData;
			return;
		}
	}

	FATFS fs;
	FRESULT res;
	f_mount(&fs, "0:", 1);

	emb_string dirName;
	dirName = "/Bank_";
	dirName += (size_t) bank; //bank_pres[0];
	dirName += "/Preset_";
	dirName += (size_t) preset; //bank_pres[1];

	// load WAV file
	FILINFO fileInfo;
	init_file_info(fileInfo);
	DIR dir;

	f_opendir(&dir, dirName.c_str());
	res = f_findfirst(&dir, &fileInfo, dirName.c_str(), "*.wav");
	if (res == FR_OK)
	{
#if _USE_LFN
		char *fn = *fileInfo.lfname ? fileInfo.lfname : fileInfo.fname;
#else
		char* fn = fileInfo.fname;
	#endif
		outCabPath.irFileName = fn;
		outCabPath.irLinkPath = dirName;
	}
	else
	{
		outCabPath.irFileName.clear();
		outCabPath.irLinkPath.clear();
	}

	free_file_info(fileInfo);
	f_closedir(&dir);
	f_mount(0, "0:", 0);
}

bool EEPROM_getDirWavNames(const std::emb_string &dirPath, list<std::emb_string> &fileNamesList, TReadLine *rl) {
	FATFS fs;
	f_mount(&fs, "0:", 1);

	FRESULT res;

	DIR dir;
	FILINFO fileInfo;
	init_file_info(fileInfo);

	res = f_opendir(&dir, dirPath.c_str());
	if (res != FR_OK)
	{
		free_file_info(fileInfo);
		f_mount(0, "0:", 0);
		return false;
	}
	res = f_readdir(&dir, &fileInfo);

	while (res == FR_OK && fileInfo.fname[0]) {
#if _USE_LFN
		char *fn = *fileInfo.lfname ? fileInfo.lfname : fileInfo.fname;
#else
		char* fn = fileInfo.fname;
	#endif

		emb_string reversedName = fn;
		reverse(reversedName.begin(), reversedName.end());
		if (reversedName.find("vaw.") == 0) {
			fileNamesList.push_back(fn);
		}

		res = f_readdir(&dir, &fileInfo); /* Search for next item */
	}

	free_file_info(fileInfo);
	f_closedir(&dir);
	f_mount(0, "0:", 0);
	return true;
}

void EEPROM_getCurrentIrInfo(ir_path_data_t &outIrData, int32_t &resultSize) {
	FATFS fs;
	f_mount(&fs, "0:", 1);

	emb_string filePath = current_ir_link.irLinkPath + "/"
			+ current_ir_link.irFileName;
	FIL file;
	FRESULT res = f_open(&file, filePath.c_str(), FA_READ);
	if (res == FR_OK) {
		outIrData = current_ir_link;
		resultSize = file.fsize;

		f_close(&file);
		f_mount(0, "0:", 0);
	} else {
		emb_string dirName;
		dirName = "/Bank_";
		dirName += (size_t) bank_pres[0];
		dirName += "/Preset_";
		dirName += (size_t) bank_pres[1];

		DIR dir;
		FILINFO fileInfo;
		init_file_info(fileInfo);

		f_opendir(&dir, dirName.c_str());
		res = f_findfirst(&dir, &fileInfo, dirName.c_str(), "*.wav");
		if (res == FR_OK) {
#if _USE_LFN
			char *fn = *fileInfo.lfname ? fileInfo.lfname : fileInfo.fname;
#else
			char* fn = fileInfo.fname;
		#endif
			outIrData.irFileName = fn;
			outIrData.irLinkPath.clear();
			resultSize = fileInfo.fsize;
		} else {
			outIrData.irFileName.clear();
			outIrData.irLinkPath.clear();
			resultSize = -1;
		}

		free_file_info(fileInfo);
		f_closedir(&dir);
		f_mount(0, "0:", 0);
	}
}

bool EEPROM_loadIr(float *cabData, const std::emb_string &irFilePath, std::emb_string &err_msg) {
	FATFS fs;
	FIL file;
	f_mount(&fs, "0:", 1);

	FRESULT fs_res = f_open(&file, irFilePath.c_str(), FA_READ);
	if (fs_res == FR_OK) {
		fs_res = f_lseek(&file, 0);
		//  read header and check as RIFF
		char header[5] = { 0, 0, 0, 0, 0 };
		UINT br;
		fs_res = f_read(&file, header, 4, &br);
		if (kgp_sdk_libc::strcmp(header, "RIFF")) {
			err_msg = "unsupported format  (no RIFF header)";
			return false;
		}

		// read header and check as MONO
		fs_res = f_lseek(&file, 22);
		uint16_t channel_count;
		fs_res = f_read(&file, (void*) &channel_count, 2, &br);
		if (channel_count != 1) {
			err_msg = "unsupported format  (no MONO channel sets)";
			return false;
		}

		// read header and check as sample rate 48000
		fs_res = f_lseek(&file, 24);
		uint32_t sample_rate;
		fs_res = f_read(&file, (void*) &sample_rate, 4, &br);
		if (sample_rate != 48000) {
			err_msg = "unsupported format  (no 48000 sample rate sets)";
			return false;
		}

		// read header and check as 24 bits
		fs_res = f_lseek(&file, 34);
		uint8_t sample_bits;
		fs_res = f_read(&file, (void*) &sample_bits, 1, &br);
		if (sample_bits != 24) {
			err_msg = "unsupported format  (no 24 sample bits sets)";
			return false;
		}

		// read cabinet size
		fs_res = f_lseek(&file, 40);
		uint16_t cab_size;
		fs_res = f_read(&file, (void*) &cab_size, 2, &br);
		cab_size /= 3;
		if (cab_size > 984)
			cab_size = 984;

		// read cabinet data
		fs_res = f_lseek(&file, 44);
		size_t i;
		for (i = 0; i < cab_size; i++) {
			uint8_t buff[4];
			fs_res = f_read(&file, (void*) buff, 3, &br);
			cabData[i] = convertToFloat(buff);
		}

		if (i < 983) {
			for (; i < 984; i++) {
				uint8_t buff[4] = { 0, 0, 0, 0 };
				cabData[i] = convertToFloat(buff);
			}
		}

		fs_res = f_close(&file);
		err_msg = "read cab data OK";
		return true;
	} else {
		err_msg = "can't open file";
		return false;
	}
}

//-----------------------------FILE MANAGEMENT-------------------------------------------------
void delete_current_cab(std::emb_string &err_msg, TReadLine *rl){
	FATFS fs;
	FRESULT res;

	f_mount(&fs, "0:", 1);
	emb_string dir_name;
	emb_string file_name;
	dir_name = "/Bank_";
	dir_name += (size_t) bank_pres[0];
	;
	dir_name += "/Preset_";
	dir_name += (size_t) bank_pres[1];

	// load WAV file
	FILINFO fno;
	DIR dir;
	char *fn; /* This function assumes non-Unicode configuration */
#if _USE_LFN
	const size_t sz = _MAX_LFN + 1;
	char *lfn = new char[sz]; /* Buffer to store the LFN */
	fno.lfname = lfn;
	fno.lfsize = sz;
#endif

	res = f_opendir(&dir, dir_name.c_str()); /* Open the directory */
	if (res == FR_OK)
	{
		for (;;)
		{
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0) {
				err_msg = "no *.wav file in the dir";
				break; /* Break on error or end of dir */
			}

#if _USE_LFN
			fn = *fno.lfname ? fno.lfname : fno.fname;
#else
		                     fn = fno.fname;
		                 #endif
			if (fno.fattrib & AM_DIR)
				continue;
			else { /* It is a directory */
				emb_string fn_name = fn;
				reverse(fn_name.begin(), fn_name.end());
				emb_string tmp = "vaw.";
				if (fn_name.find(tmp) == 0) {
					fn_name = dir_name;
					fn_name += "/";
					fn_name += fn;

					f_unlink(fn_name.c_str());
				}

			}
		}
		f_closedir(&dir);
	}
	f_mount(0, "0:", 0);
}

static inline void dir_remove_wavs_raw(const emb_string &dir_name) {
	emb_string wav_to_delete;
	while (dir_get_wav(dir_name, wav_to_delete)) {
		f_unlink((dir_name + "/" + wav_to_delete).c_str());
	}
}

bool EEPROM_copyFile(emb_string &errMsg, const emb_string &srcPath, const emb_string &dstPath) {
	FATFS fs;
	FRESULT res;

	f_mount(&fs, "0:", 1);

	FIL f_src;
	FIL f_dst;

	res = f_open(&f_src, srcPath.c_str(), FA_READ);
	if (res != FR_OK) {
		errMsg = "SRC_PATH_UNAVALIABLE";
		return false;
	}

	res = f_open(&f_dst, dstPath.c_str(), FA_WRITE | FA_OPEN_ALWAYS); //FA_CREATE_NEW );
	if (res != FR_OK) {
		errMsg = "DST_PATH_UNAVALIABLE";
		return false;
	}

	char *cb = new char[_MIN_SS];

	res = f_lseek(&f_src, 0);
	res = f_lseek(&f_dst, 0);

	UINT br;
	UINT bw;
	do {
		f_read(&f_src, cb, _MIN_SS, &br);
		if (br)
			f_write(&f_dst, cb, br, &bw);
	} while (br == _MIN_SS);

	delete[] cb;

	f_close(&f_src);
	f_close(&f_dst);

	f_mount(0, "0:", 0);
	return true;
}

void console_dir_remove_wavs(const emb_string &dir_name) {
	FATFS fs;
	f_mount(&fs, "0:", 1);
	dir_remove_wavs_raw(dir_name);
	f_mount(0, "0:", 0);
}

