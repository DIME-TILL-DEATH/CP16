#include "console_handlers.h"
#include "comm_handlers_legacy.h"

#include "cs.h"

#include "ff.h"

#include "gpio.h"
#include "eeprom.h"

#include "ADAU/adau1701.h"

#include "preset.h"

#include "DSP/compressor.h"
#include "DSP/amp_imp.h"
#include "DSP/filters.h"
#include "DSP/sound_processing.h"
#include "DSP/Reverb/reverb.h"

void getDataPartFromStream(TReadLine* rl, char* buf, int* dataSize, int maxSize)
{
	kgp_sdk_libc::memset(buf, 0, maxSize);
	int streamPos = 0;
	do
	{
		int c;
		rl->RecvChar(c);
		if (c == '\r' || c == '\n')
		{
			if(dataSize)
				*dataSize = streamPos;
			return;
		}
		buf[streamPos++] = c;
	}while(streamPos < maxSize);
}

static void amtid_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	char hex[3] = {0,0,0};
	i2hex(AMT_DEV_ID, hex);
	msg_console("%s\r%d\n", args[0], AMT_DEV_ID);
	msg_console("END\n");
}

static void amtver_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\r%s\n", args[0], ver);
	msg_console("END\n");
}

static void preset_list_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("plist");
	for(int b = 0; b<4; b++)
		for(int p = 0; p<4; p++)
		{
			save_data_t load_data;
			ir_path_data_t link_data;
			EEPROM_loadPreset(b, p, load_data, link_data);
			EEPROM_getPresetCabPath(b, p, link_data);
			msg_console("\r%s|0%d|%s", link_data.irFileName.c_str(), load_data.parametersData.cab_sim_on, load_data.name);
		}
	msg_console("\n");
}

static void get_mode_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	char hex[3] = {0,0,0};
	if(count > 0)
	{
		if(count == 2)
		{
			char* end;
			uint8_t val = kgp_sdk_libc::strtol(args[1], &end, 16);
			system_parameters.output_mode = val;
		}

		i2hex(system_parameters.output_mode, hex);
		msg_console("%s\r%s\n", args[0], hex);
		EEPROM_saveSys();
	}
}

static void get_bp_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\r", args[0]);
	for(size_t i = 0 ; i < 2 ; i++)
	{
		char hex[3] = {0,0,0};
		i2hex(bank_pres[i], hex);
		msg_console("%s", hex ) ;
	}
	msg_console("\n");
}

static void pname_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	if(count>1)
	{
		std::emb_string command = args[1];

		if(command == "set")
		{
			int recievedBytes;
			getDataPartFromStream(rl, current_preset_name, &recievedBytes, PRESET_NAME_LENGTH);
		}
		msg_console("pname\r%s\n", current_preset_name);
	}
}

static void state_comm_handler (TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s", args[0]);
	if(count>1)
	{
		std::emb_string command = args[1];

		msg_console(" %s", args[1]);
		if(command == "set")
		{

		}
	}
	msg_console("\r");
	char hex[3] = {0,0,0} ;
	char buffer[sizeof(preset_data_t)];
	kgp_sdk_libc::memcpy(buffer, &current_preset, sizeof(preset_data_t));
	for (size_t i = 0; i < sizeof(preset_data_t); i++)
	{
		i2hex(buffer[i], hex);
		msg_console("%s", hex);
	}
	msg_console("\n");
}

static void save_pres_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\r\n", args[0]);
	EEPROM_savePreset();
	msg_console("END\n") ;
}

static void ls_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s", args[0]);

	if(count < 2)
	{
		msg_console(" INCORRECT_ARGUMENTS\r\n");
		return;
	}
	msg_console(" %s", args[1]);

	std::emb_string dirPath(args[1]);
	list<std::emb_string> fileNamesList;

	if(EEPROM_getDirWavNames(dirPath, fileNamesList, rl))
	{
		if(fileNamesList.empty())
		{
			msg_console("\r\n");
		}
		else
		{
			for(auto it = fileNamesList.begin(); it != fileNamesList.end(); ++it)
			{
				msg_console("\r%s", (*it).c_str());
			}
		}
		msg_console("\r\n");
	}
	else
	{
		msg_console(" OPEN_DIR_FAILED\r\n");
	}
}

static void ir_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("ir ");
	if(count < 2)
	{
		msg_console("error\rCOMMAND_INCORRECT\r\n");
		return;
	}
	std::emb_string command = args[1];

	const int bufferSize = 512;
	char dataBuffer[bufferSize];

	if(command == "info")
	{
		ir_path_data_t irPathData;
		DWORD irSize;
		EEPROM_getCurrentIrInfo(irPathData, irSize);
		msg_console("info\r%s\r%s\r%d\n", irPathData.irLinkPath.c_str(), irPathData.irFileName.c_str(), irSize);
	}

	if(command == "link")
	{
		int recievedSize = 0;
		getDataPartFromStream(rl, dataBuffer, &recievedSize, bufferSize);
		current_ir_link.irFileName = dataBuffer;
		getDataPartFromStream(rl, dataBuffer, &recievedSize, bufferSize);
		current_ir_link.irLinkPath = dataBuffer;

		// TODO check file exist
		emb_string irFilePath = current_ir_link.irLinkPath + "/" + current_ir_link.irFileName;
		CS_activateIr(irFilePath);

		msg_console("link\r%s\r%s\n", current_ir_link.irFileName.c_str(), current_ir_link.irLinkPath.c_str());
	}
}
//===============================================PARAMETERS COMM HANDLERS========================================================
inline void default_param_handler(uint8_t* param_ptr, TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	char hex[3] = {0,0,0};
	if(count > 0)
	{
		if(count == 2)
		{
			char* end;
			uint8_t val = kgp_sdk_libc::strtol(args[1], &end, 16);
			*param_ptr = val;
		}

		i2hex(*param_ptr, hex);
		msg_console("%s %s\n", args[0], hex);
	}
}

static void cabinet_enable_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	 default_param_handler(&current_preset.cab_sim_on, rl, args, count);
}

static void master_volume_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.volume, rl, args, count);
     processing_params.preset_volume = powf(current_preset.volume, 2.0f)*(1.0f/powf(31.0f, 2.0f));
}

static void gate_on_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
    default_param_handler(&current_preset.gate.on, rl, args, count);
}

static void gate_threshold_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
    default_param_handler(&current_preset.gate.threshold, rl, args, count);
    gate_par(current_preset.gate.threshold << 8);
}

static void gate_decay_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
    default_param_handler(&current_preset.gate.decay, rl, args, count);
    gate_par(1 | (current_preset.gate.decay << 8));
}

static void compressor_on_comm_handler (TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
    default_param_handler(&current_preset.compressor.on, rl, args, count);
}

static void compressor_sustain_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
    default_param_handler(&current_preset.compressor.sustain, rl, args, count);
    comp_par(0 | (current_preset.compressor.sustain << 8));
}

static void compressor_volume_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
    default_param_handler(&current_preset.compressor.volume, rl, args, count);
    comp_par(2 | (current_preset.compressor.volume << 8));
}


static void preamp_on_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.preamp.on, rl, args, count);
}

static void preamp_volume_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.preamp.volume, rl, args, count);
     processing_params.pream_vol = powf(current_preset.preamp.volume, 2.0f) * (1.0f/powf(31.0f, 2.0f));
}

static void preamp_low_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.preamp.low, rl, args, count);
     preamp_param(PREAMP_LOW, current_preset.preamp.low);
}

static void preamp_mid_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.preamp.mid, rl, args, count);
     preamp_param(PREAMP_MID, current_preset.preamp.mid);
}

static void preamp_high_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.preamp.high, rl, args, count);
     preamp_param(PREAMP_HIGH, current_preset.preamp.high);
}
static void amp_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.power_amp.on, rl, args, count);
}

static void amp_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.power_amp.volume, rl, args, count);
     processing_params.amp_vol = powf(current_preset.power_amp.volume, 2.0f) * (10.0f/powf(31.0f, 2.0f)) + 1.0f;
}

static void amp_slave_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.power_amp.slave, rl, args, count);
     processing_params.amp_slave = powf(current_preset.power_amp.slave, 4.0f) * (0.99f/powf(31.0f, 4.0f)) + 0.01f;
}

static void amp_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.power_amp.type, rl, args, count);
     pa_update_coefficients(current_preset.power_amp.type);
}

static void presence_on_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.power_amp.presence_on, rl, args, count);
}

static void presence_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.power_amp.presence_vol, rl, args, count);
     set_shelf(current_preset.power_amp.presence_vol); // in RV was *(25.0f/31.0f));
}

static void lpf_on_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.eq1.lp_on, rl, args, count);
}

static void lpf_volume_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.eq1.lp_freq, rl, args, count);
     float lopas = powf(195 - current_preset.eq1.lp_freq, 2.0f)*(19000.0f/powf(195.0f,2.0f))+1000.0f;
	 SetLPF(lopas);
}

static void hpf_on_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.eq1.hp_on, rl, args, count);
}

static void hpf_volume_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&current_preset.eq1.hp_freq, rl, args, count);
	float hipas = current_preset.eq1.hp_freq*(980.0f/255.0f) + 20.0f;
	SetHPF(hipas);
}

//====================================GEN 2 commands====================================
static void eq1_band_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	char hex[3] = {0,0,0};
	 uint8_t band_num = args[1][0] - 48;
	 if(count == 2)
	 {
		 i2hex(current_preset.eq1.band_type[band_num], hex );

		 msg_console("%s\n" , hex );
		 return;
	 }

	 if(count == 3)
	 {
		 char* end;
		 int32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
		 current_preset.eq1.band_type[band_num] = val;

		 filt_ini(band_num, current_preset.eq1.freq, current_preset.eq1.Q);
		 set_filt(band_num, current_preset.eq1.gain[band_num], (band_type_t)current_preset.eq1.band_type[band_num]);
	 }
}

static void eq_on_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.eq1.parametric_on, rl, args, count);
}

static void early_on_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.reverb.on, rl, args, count);
}

static void early_volume_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.reverb.volume, rl, args, count);
     processing_params.ear_vol = current_preset.reverb.volume * (1.0f/31.0f);
}

static void early_type_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
     default_param_handler(&current_preset.reverb.type, rl, args, count);
}
//-------------------------------------------SERVICE COMMAND HANDLERS--------------------------------------------------------------------
static void fs_format_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	msg_console("fsf\r");
	std::emb_string err_str ;
	console_fs_format(err_str, rl);
}

static void fw_update_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	msg_console("fwu\r");
	std::emb_string err_msg  ;
	if ( console_fs_write_file(err_msg, rl , "0:/firmware"))
	{
		TTask::Delay(100); // задержка, необходимая для сброса данных в TUsbTask по интерфейсу usb
		NVIC_SystemReset();
	}
}

static void programm_change_comm_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	char* end;

	if(count > 1)
	{
		uint32_t val = kgp_sdk_libc::strtol (args[1], &end, 16);
		if(end != args[1])
		{
			decode_preset(bank_pres, val);
			preset_change();
			TTask::Delay(50);
			msg_console("pc %d%d\rEND\n", bank_pres[0], bank_pres[1]) ;
			return;
		}
	}
	msg_console("pc\rPARAM_ERROR\n") ;
}

static void preset_wav_copy_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
   std::emb_string err_msg  ;

   char* end ;
   uint32_t preset_src  = kgp_sdk_libc::strtol ( args[1] , &end, 16 );
   uint32_t preset_dst = kgp_sdk_libc::strtol ( args[2] , &end, 16 );

   console_fs_preset_copy_wav_file(err_msg, rl , preset_src, preset_dst) ;
   msg_console("END\n") ;
}

static void preset_map0_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	char hex[3] = {0,0,0};
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
	char hex[3] = {0,0,0};
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
	char hex[3] = {0,0,0};
	uint8_t bp ;
	load_map0(bp);
	i2hex( bp,hex);
	const char* a[2] = {0,hex} ;
	programm_change_comm_handler( rl , a , 2 ) ;
}

static void use_map1_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	char hex[3] = {0,0,0};
	uint8_t bp ;
	load_map1(bp);
	i2hex( bp,hex);
	const char* a[2] = {0,hex} ;
	programm_change_comm_handler( rl , a , 2 ) ;
}

//========================================================================================================================

void consoleSetCmdHandlers(TReadLine* rl)
{
	setConsoleCmdDefaultHandlers(rl);
	set_legacy_handlers(rl);

	rl->AddCommandHandler("amtdev", amtid_comm_handler);
	rl->AddCommandHandler("amtver", amtver_command_handler);

	rl->AddCommandHandler("plist", preset_list_command_handler);

	rl->AddCommandHandler("gm", get_mode_comm_handler);
	rl->AddCommandHandler("gb", get_bp_comm_handler);

	rl->AddCommandHandler("pname", pname_comm_handler);

	rl->AddCommandHandler("state", state_comm_handler);

	rl->AddCommandHandler("pc", programm_change_comm_handler);
	rl->AddCommandHandler("sp", save_pres_comm_handler);

	rl->AddCommandHandler("ls", ls_comm_handler);
	rl->AddCommandHandler("ir", ir_comm_handler);

	// *****************params comm handlers********
	rl->AddCommandHandler("ce", cabinet_enable_comm_handler);

	rl->AddCommandHandler("mv", master_volume_comm_handler);

	rl->AddCommandHandler("go", gate_on_comm_handler);
	rl->AddCommandHandler("gt", gate_threshold_comm_handler);
	rl->AddCommandHandler("gd", gate_decay_comm_handler);

	rl->AddCommandHandler("co", compressor_on_comm_handler);
	rl->AddCommandHandler("cs", compressor_sustain_comm_handler);
	rl->AddCommandHandler("cv", compressor_volume_comm_handler);

	rl->AddCommandHandler("pro", preamp_on_comm_handler);
	rl->AddCommandHandler("prv", preamp_volume_comm_handler);
	rl->AddCommandHandler("prl", preamp_low_command_handler);
	rl->AddCommandHandler("prm", preamp_mid_command_handler);
	rl->AddCommandHandler("prh", preamp_high_command_handler);

	rl->AddCommandHandler("ao", amp_on_command_handler);
	rl->AddCommandHandler("av", amp_volume_command_handler);
	rl->AddCommandHandler("as", amp_slave_command_handler);
	rl->AddCommandHandler("at", amp_type_command_handler);
	rl->AddCommandHandler("po", presence_on_comm_handler);
	rl->AddCommandHandler("pv", presence_volume_command_handler);

	rl->AddCommandHandler("lo", lpf_on_comm_handler);
	rl->AddCommandHandler("lv", lpf_volume_comm_handler);
	rl->AddCommandHandler("ho", hpf_on_comm_handler);
	rl->AddCommandHandler("hv", hpf_volume_comm_handler);
	//====================================GEN 2 commands====================================
	rl->AddCommandHandler("eq1_bt", eq1_band_type_command_handler);

	rl->AddCommandHandler("eo", early_on_comm_handler);
	rl->AddCommandHandler("ev", early_volume_comm_handler);
	rl->AddCommandHandler("et", early_type_comm_handler);

// *********************service comms*******************
rl->AddCommandHandler("sm0", preset_map0_command_handler);
rl->AddCommandHandler("sm1", preset_map1_command_handler);
rl->AddCommandHandler("um0", use_map0_command_handler);
rl->AddCommandHandler("um1", use_map1_command_handler);

rl->AddCommandHandler("fsf", fs_format_command_handler);
rl->AddCommandHandler("fwu", fw_update_command_handler);
}

//------------------------------------------------------------------------------
