#include "console_handlers.h"

#include "comm_handlers_legacy.h"

#include "cs.h"

#include "math.h"
#include "ff.h"

#include "gpio.h"
#include "eeprom.h"

#include "ADAU/adau1701.h"

#include "preset.h"

#include "PROCESSING/amp_imp.h"
#include "PROCESSING/compressor.h"
#include "PROCESSING/filters.h"
#include "PROCESSING/reverb.h"
#include "PROCESSING/sound_processing.h"

extern char __CCM_BSS__ buff[];
char hex[3] = { 0, 0, 0 };
uint16_t imp_count = 0;

// первый аргумент - имя файла(актуально для второго 0 ) второй - 0 это запись файла импульса , данные идут побайтно с подтверждением ,
// 1 это загрузка импульса - данные идут семплами(по 3 байта начиная с младшева)

volatile uint32_t buff_size = rev_size * sizeof(float);
volatile uint32_t stream_pos;

static void current_cabinet_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	msg_console("%s", args[0]);

	std::emb_string err_str;
	size_t chunk;
	if (count == 1) {
		msg_console("\r");
		console_out_currnt_cab(err_str, rl);
		return;
	}

	if (count == 3) {
		msg_console(" %s %s\r", args[1], args[2]);
		rev_en = 1;
		if (args[2][0] == '0' && kgp_sdk_libc::strlen(args[2]) == 1) {
			while (!rev_en1);

			stream_pos = 0;
			do {
				int c;
				char w;
				rl->RecvChar(c);
				if (c == '\r') {
					delete_current_cab(err_str, rl);
					FATFS fs;
					FRESULT res;
					FIL f;
					UINT bw;

					res = f_mount(&fs, "0:", 1);
					if (res != FR_OK) {
						msg_console("error: 'f_mount' result '%s'\n",
								f_err2str(res));
						return;
					}
					emb_string file_name;
					file_name = "0:/Bank_";
					file_name += (size_t) bank_pres[0];
					file_name += "/Preset_";
					file_name += (size_t) bank_pres[1];
					file_name += "/";
					file_name += args[1];
					FRESULT fs_res = f_open(&f, file_name.c_str(),
							FA_CREATE_ALWAYS | FA_WRITE);
					if (fs_res != FR_OK) {
						msg_console("error: file %s not created, 'f_open' result '%s'\n", args[1] , f_err2str(res));
						f_mount(0, "0:", 0);
						return;
					}
					//----------------------------------------------------------------------------------------------
					// скидывание остатков потока из буффера если они там есть
					fs_res = f_write(&f, buff, stream_pos, &bw);
					if (bw != stream_pos || fs_res != FR_OK) {
						f_close(&f);
						f_unlink(file_name.c_str());
						msg_console("file write operation failed\n");
						f_mount(0, "0:", 0);
						return;
					}
					//---------------------------------------------------------------------------------------------
					f_close(&f);
					f_mount(0, "0:", 0);
					kgp_sdk_libc::memset(buff, 0, buff_size);
					rev_en = 0;
					rev_en1 = 0;
					msg_console("ccEND\n");
					return;
				}
				if (c > 57)
					c -= 39;
				w = (c - '0') << 4;
				rl->RecvChar(c);
				if (c == '\r') {
					msg_console("SYNC ERROR\n");
					return;
				}
				if (c > 57)
					c -= 39;
				w |= c - '0';
				buff[stream_pos++] = w; // запись в буффер очередного байта
			} while (1);
			//--------------------------------------------------------------------------------------
		} else {
			while (!rev_en1)
				;
			imp_count = 0;
			chunk = 3;
			uint8_t buff1[3] = { 0, 0, 0 };
			do {
				int c;
				char w;
				for (size_t i = 0; i < chunk; i++) {
					rl->RecvChar(c);
					if (c == '\r') {
						if (imp_count < 984) {
							for (; imp_count < 984; imp_count++) {
								buff1[0] = buff1[1] = buff1[2] = 0;
								cab_data[imp_count] = convert((uint8_t*) buff1);
							}
						}

						dsp_upload_ir(cab_data);
						processing_params.impulse_avaliable = 1;
						current_preset.cab_sim_on = 1;

						imp_count = 0;
						rev_en = 0;
						rev_en1 = 0;
						msg_console("ccEND\n");
						return;
					}
					if (c > 57)
						c -= 39;
					w = (c - '0') << 4;
					rl->RecvChar(c);
					if (c > 57)
						c -= 39;
					w |= c - '0';
					buff1[i] = w;
				}
				if (imp_count < 984)
					cab_data[imp_count++] = convert((uint8_t*) buff1);
			} while (1);
			return;
		}
	}
	msg_console("invalid args count\n" );
}

static void read_name_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	ir_path_data_t path_data;
	EEPROM_getPresetCabPath(bank_pres[0], bank_pres[1], path_data);
	msg_console("%s\r%s\n", args[0], path_data.irFileName.c_str());
}

static void rns_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	msg_console("rns\r");
	for (int b = 0; b < 4; b++)
		for (int p = 0; p < 4; p++) {
			save_data_t load_data;
			ir_path_data_t link_data;
			EEPROM_loadPreset(b, p, load_data, link_data);
			EEPROM_getPresetCabPath(b, p, link_data);
			msg_console("%s\n0%d\n", link_data.irFileName.c_str(),
					load_data.parametersData.cab_sim_on);
		}
	msg_console("END\n");
}

static void get_state_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	msg_console("%s\r", args[0]);

	uint8_t legacy_preset_data[128];

	if (count == 1) {
		for (size_t i = 0; i < pdCount; i++) {
			legacy_from_preset((preset_data_legacy_t*) legacy_preset_data,
					&current_preset);
			i2hex(legacy_preset_data[i], hex);
			msg_console("%s", hex);
		}
		msg_console("\n");
		return;
	}
	size_t a = 0;
	char w;
	int c;

	do {
		rl->RecvChar(c);
		if (c == '\r') {
			set_parameters();
			msg_console("gsEND\n");
			return;
		}

		if (c > 57)
			c -= 39;
		w = (c - '0') << 4;
		rl->RecvChar(c);
		if (c == '\r') {
			msg_console("SYNC ERROR\n");
			return;
		}
		if (c > 57)
			c -= 39;
		w |= c - '0';
		legacy_preset_data[a++] = w;

		preset_from_legacy(&current_preset,
				(preset_data_legacy_t*) legacy_preset_data);
	} while (1);
}

static void lpf_on_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	default_param_handler(&current_preset.eq1.lp_on, rl, args, count);
}

static void lpf_volume_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	char hex[3] = { 0, 0, 0 };
	uint8_t val;
	if (count > 0) {
		if (count == 2) {
			char *end;
			val = kgp_sdk_libc::strtol(args[1], &end, 16);
			current_preset.eq1.lp_freq = powf(195 - val, 2.0f)
					* (19000.0f / powf(195.0f, 2.0f)) + 1000.0f;
			SetLPF(current_preset.eq1.lp_freq);
		}

		i2hex(val, hex);
		msg_console("%s %s\n", args[0], hex);
	}
}

static void hpf_on_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	default_param_handler(&current_preset.eq1.hp_on, rl, args, count);
}

static void hpf_volume_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	char hex[3] = { 0, 0, 0 };
	uint8_t val;
	if (count > 0) {
		if (count == 2) {
			char *end;
			val = kgp_sdk_libc::strtol(args[1], &end, 16);
			current_preset.eq1.hp_freq = val * (980.0f / 255.0f) + 20.0f;
			SetHPF(current_preset.eq1.hp_freq);
		}

		i2hex(val, hex);
		msg_console("%s %s\n", args[0], hex);
	}
}

static void eq_on_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	if (count > 0) {
		if (count == 2) {
			char *end;
			uint8_t val = kgp_sdk_libc::strtol(args[1], &end, 16);
			current_preset.eq1.parametric_on = val;
		}

		i2hex(current_preset.eq1.parametric_on, hex);
		msg_console("%s %s\n", args[0], hex);
	}
}

static void eq_volume_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	uint8_t band_num = args[1][0] - 48;
	if (count > 1) {
		if (count == 3) {
			char *end;
			uint32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
			current_preset.eq1.gain[band_num] = val;

			filterInit(band_num, current_preset.eq1.freq[band_num],
					current_preset.eq1.Q[band_num]);
			filterCalcCoefs(band_num, current_preset.eq1.gain[band_num],
					(band_type_t) current_preset.eq1.band_type[band_num]);
		}
		i2hex(current_preset.eq1.gain[band_num], hex);
		msg_console("%s %d %s\r\n", args[0], band_num, hex);
	}
}

static void eq_freq_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	uint8_t band_num = args[1][0] - 48;

	if (count > 1) {
		if (count == 3) {
			char *end;
			int32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
			current_preset.eq1.freq[band_num] = convertLegacyFreq(band_num,
					val);

			filterInit(band_num, current_preset.eq1.freq[band_num],
					current_preset.eq1.Q[band_num]);
			filterCalcCoefs(band_num, current_preset.eq1.gain[band_num],
					(band_type_t) current_preset.eq1.band_type[band_num]);
		}
		i2hex(current_preset.eq1.freq[band_num], hex);
		msg_console("%s %d %s\r\n", args[0], band_num, hex);
	}
}

static void eq_q_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	uint8_t band_num = args[1][0] - 48;

	if (count > 1) {
		if (count == 3) {
			char *end;
			int32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
			current_preset.eq1.Q[band_num] = val;

			filterInit(band_num, current_preset.eq1.freq[band_num],
					current_preset.eq1.Q[band_num]);
			filterCalcCoefs(band_num, current_preset.eq1.gain[band_num],
					(band_type_t) current_preset.eq1.band_type[band_num]);
		}
		i2hex(current_preset.eq1.Q[band_num], hex);
		msg_console("%s %d %s\r\n", args[0], band_num, hex);
	}
}

static void eq_position_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	msg_console("%s", args[0]);
	if (count == 1) {
		i2hex(0, hex);
		msg_console("%s\n", hex);
		return;
	}

	return; // don't do anything in gen2 firmware
	char *end;
	uint32_t val = kgp_sdk_libc::strtol(args[1], &end, 16);
	if (val) {
		DSP_set_module_to_processing_stage(BYPASS, 0);
		DSP_set_module_to_processing_stage(CM, 1);
		DSP_set_module_to_processing_stage(EQ, 2);
		DSP_set_module_to_processing_stage(PR, 3);
		DSP_set_module_to_processing_stage(PA, 4);
		DSP_set_module_to_processing_stage(IR, 5);
		DSP_set_module_to_processing_stage(HP, 6);
		DSP_set_module_to_processing_stage(LP, 7);
		DSP_set_module_to_processing_stage(NG, 8);
	} else {
		DSP_set_module_to_processing_stage(BYPASS, 0);
		DSP_set_module_to_processing_stage(CM, 1);
		DSP_set_module_to_processing_stage(PR, 2);
		DSP_set_module_to_processing_stage(PA, 3);
		DSP_set_module_to_processing_stage(IR, 4);
		DSP_set_module_to_processing_stage(HP, 5);
		DSP_set_module_to_processing_stage(EQ, 6);
		DSP_set_module_to_processing_stage(LP, 7);
		DSP_set_module_to_processing_stage(NG, 8);
	}

	msg_console("%s\n", hex);
}

static void esc_comm_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	preset_change();
	msg_console("%s\r\nEND\n", args[0]);
}

static void load_current_cab_command_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	msg_console("%s\r\n", args[0]);
	ir_path_data_t link_data;
	EEPROM_getPresetCabPath(bank_pres[0], bank_pres[1], link_data);
	CS_activateIr(link_data.irLinkPath + "/" + link_data.irFileName);
	msg_console("END\n");
}

//------------------------------------------------------------------------------
static void sw4_command_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {
	if (count == 1) {
		msg_console("%s\n" , sw4_state ? "enable" : "disable");
		return;
	}
	if (!kgp_sdk_libc::strcmp(args[1], "enable"))
		sw4_state = true;
	else
		sw4_state = false;

	msg_console("END\n");
}

//------------------------------------------------------------------------------
static void preset_wavs_delete_command_handler(TReadLine *rl,
		TReadLine::const_symbol_type_ptr_t *args, const size_t count) {

	emb_string preset_dir_name;
	if (count == 1) // удаление в текщем пресете
			{
		uint8_t bpenc = encode_preset(bank_pres[bpf_bank],
				bank_pres[bpf_preset]);
		decode_preset_dir(bpenc, preset_dir_name);
	} else {
		char *end;
		uint8_t arg1 = kgp_sdk_libc::strtol(args[1], &end, 16);
		decode_preset_dir(arg1, preset_dir_name);
	}

	console_dir_remove_wavs(preset_dir_name);

	msg_console("END\n");
}

//======================================================================================
void set_legacy_handlers(TReadLine *rl) {
	rl->AddCommandHandler("rn", read_name_comm_handler);
	rl->AddCommandHandler("rns", rns_comm_handler);
	rl->AddCommandHandler("gs", get_state_comm_handler);

	rl->AddCommandHandler("cc", current_cabinet_comm_handler);
	rl->AddCommandHandler("esc", esc_comm_handler);
	rl->AddCommandHandler("lcc", load_current_cab_command_handler);
	//rl->AddCommandHandler("dcc", delete_current_cab_command_handler);

	// *****************params comm handlers********
	rl->AddCommandHandler("eqo", eq_on_comm_handler);
	rl->AddCommandHandler("eqp", eq_position_comm_handler);
	rl->AddCommandHandler("eqv", eq_volume_comm_handler);
	rl->AddCommandHandler("eqf", eq_freq_comm_handler);
	rl->AddCommandHandler("eqq", eq_q_comm_handler);

	rl->AddCommandHandler("lo", lpf_on_comm_handler);
	rl->AddCommandHandler("lv", lpf_volume_comm_handler);
	rl->AddCommandHandler("ho", hpf_on_comm_handler);
	rl->AddCommandHandler("hv", hpf_volume_comm_handler);

	// *********************service comms*******************
	rl->AddCommandHandler("pwsd", preset_wavs_delete_command_handler); // удаляет все *.wav в директории текущего пресета или в том на который указывает параметр

	rl->AddCommandHandler("sw4", sw4_command_handler);
}
//------------------------------------------------------------------------------
