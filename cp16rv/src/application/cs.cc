#include "appdefs.h"
#include "cs.h"
#include "init.h"
#include "filt.h"
#include "Reverb/reverb.h"
#include "sigma.h"
#include "eepr.h"
#include "AT45DB321.h"
#include "debug_led.h"

#define preamp_stage 6

/*
1.03.01(20220904)
  сбвиг нумерации версий прошивок по просьбе АМТ и Дмитрия
1.4.4  (20220717)
  рефакторинг keychk(void)
  автоматизация герерации firmware, имя и версия прошивки теперь снаружи кода в options.mk
1.4.3 (20220622)
  перенесены изменения из CP16.PA.V. на 20220622
1.4.2
  изменено кодирование bank/preset 2:2 бит в 4:4
1.03.01
  изменен порядок нумерации версий (требование Маричева)
1.03.02
  исправлены косяги команды cc
  добавлена команда preset_delete_wavs
  удалена команда dcc как частный случай preset_delete_wavs
  изменена rns для вывода размера wav в пресете
  метка раздела пришита к названию устройства
  поправлен/согласован  формат и вывод символов версии прошивки для обоих проектов cp16/cp16_pa
  добавлен функционал вывода размера файлов при обмене - для Димы, чтобы он мог вычислить состояние "прогресс-бара" при записи чтении фалов на файловую систему
1.04.00
  процесс портирование на текущий sdk
1.04.01
  исправлена утечка памяти в dir_get_wav()
1.04.02(20230501)
  исправлена ошибка вызова eeprom.c : console_fs_write_file  : rl->RecvLine(str);  c неиницилизированным полем cdc_io.recv_line
1.04.03(20230503)
  задержка в console task, необходимая для сброса данных в TUsbTask по интерфейсу usb при сбросе процессора в команде обновления прошивки
1.04.05(20240517)
  добавлен расчет и вывод в консоль уровня громкости в каналах
1.05.00 (20240925)
  добавлен функционал map0 и map1
1.05.01 (20240926)
  функционал map0 и map1 протянут с селектора на материнской плате(переключатели)
1.05.02 (20241005)
  втыкание со стрта map0
1.05.03 (20241007)
  команда sw1
*/

const uint8_t ver[] = FIRMWARE_VER ;
const uint8_t dev[] = FIRMWARE_NAME ;

uint8_t ind_en;

uint8_t usb_flag = 0;
bool sw4_state = true;

volatile uint32_t key_buf;

float m_vol = 0.0f;
volatile uint8_t m_vol_fl = 0;
volatile uint8_t m_vol_fl1 = 0;
float p_vol = 1.0f;
float ear_vol = 0.0f;
extern volatile uint8_t usb_type;

arm_biquad_casd_df1_inst_f32 preamp_instance;
float __CCM_BSS__ coeff_preamp[preamp_stage * 5];
float __CCM_BSS__ stage_preamp[preamp_stage * 4];

volatile  float vol_ind_vector[3];

void start_usb(uint8_t type);

TCSTask* CSTask ;

TCSTask::TCSTask () : TTask()
	   {
	   }

inline void keychk(void)
{
	static uint8_t key_buf_static = 0xff;

	if ( !sw4_state ) return ;

    key_buf = GPIOB->IDR & 0xc03;
    key_buf |= key_buf >> 8;
    key_buf = ~key_buf & 0xf;
    if(key_buf_static != key_buf)
    {
    	key_buf_static = key_buf;
	  bank_pres[0] = key_buf_static >> 2;
	  bank_pres[1] = key_buf_static & 3;

	  prog_ch();
    }
}

void TCSTask::Code()
{

  init();

  Gate_Change_Preset ();
  Compressor_init();
  Compressor_Change_Preset(0,0);

  arm_biquad_cascade_df1_init_f32(&preamp_instance, preamp_stage , coeff_preamp , stage_preamp);

  flash_folder_init();

  adau_init_ic();

  bank_pres[0] = bank_pres[1] = 0;

  extern const uint8_t ver[];
  uint8_t temp = 0;
  for(uint8_t i = 0 ; i < 8 ; i++)if(ver[i] != sys_para[23 + i])temp++;
  if(temp)
  {
	  for(uint8_t i = 0 ; i < 11 ; i++)sys_para[13 + i] = dev[i];
	  for(uint8_t i = 0 ; i < 8 ; i++)sys_para[23 + i] = ver[i];
	  save_sys();
  }

  prog_ch();

  dsp_run();

  while(1)
     {
	    if(!usb_flag)
	      {
	         if(GPIOA->IDR & GPIO_Pin_9)
		         {
		            usb_flag = 1;
		            void start_usb(uint8_t type);
		            start_usb(usb_type);
		            sw4_state = false ;
		         }
	      }
	    else
	      {
		     if(!(GPIOA->IDR & GPIO_Pin_9))
		     NVIC_SystemReset();
	      }


#ifdef __LA3_MOD__
	    uint8_t bp ;
	    load_map0(bp);
	    decode_preset( bank_pres, bp);
	    prog_ch();
#else
	    keychk();  // управление пресетами с внешних устройств по средством gpio обычных cp16
#endif
    }
}

int32_t ccl;
int32_t ccr;
int32_t cc_cab;
uint32_t cl;
uint32_t cr;
float c1;
float c2;
float gate_buf;
float pream_vol;

uint16_t pwm = 0;
uint16_t pwm_count;
float pwm_count_f = 0.0f;
float pwm_count_f_temp = 0.0f;
float pwm_count_f_old = 0.0f;
uint16_t pwm_count_po;
uint8_t pick_fl;

extern "C" void DMA1_Stream3_IRQHandler()
{
  DMA_ClearITPendingBit ( DMA1_Stream3 , DMA_IT_TCIF3);

	  cr = ror16(init_get_adc_data().right.sample);
	  cl = ror16(init_get_adc_data().left.sample);

	  ccr = cr;
	  ccl = cl;
	  ccr = ccr >> 8;
	  ccl = ccl >> 8;

	  c1 = ccl;

	  float avg_in = 0 ;
	  avg_in += abs((int32_t)(ccl * 8388607.0f));

	  if(prog_data[cab_on] && cab_fl)c1 = ccr;

	  cc_cab = ccr;
	  c1 *= 0.000000119f;

	  gate_buf = gate_out(c1);
	  if(prog_data[compr_on])c1 = compr_out(c1);

	  if(prog_data[pr_on])c1 = proc_shelf(c1);

	  if(prog_data[hip_on])c1 = filt_hp(c1);

	  if(prog_data[eq_on])c1 = biquad(c1);

	  if(prog_data[lop_on])c1 = filt_lp(c1);

	  arm_biquad_cascade_df1_f32(&preamp_instance, &c1, &c2, 1);
	  if(prog_data[preamp_on])c1 = c2 * pream_vol * 3.0f;

	  if(prog_data[gate_on])c1 *= gate_buf;

	  if(prog_data[er_on])accum = c1*0.7f;
	  else accum = 0.0f;
	if(!rev_en)
	{
	  switch (prog_data[e_t]){
	  case 0:early1();break;
	  case 1:early2();break;
	  case 2:early3();break;
	  }
	}
	else rev_en1 = 1;

	  c2 = (c1 + ear_outR*ear_vol) * 8388607.0f;
	  c1 += ear_outL*ear_vol;

//-----------------------------------------------Indic peack--------------------------------
	  	  float a = fabsf(c1);
	  	  if(pwm_count_f < a)pwm_count_f = a;
	  	  pwm_count_po++;
	  	  pwm_count_po &= 0x1ff;
	  	  if(!pwm_count_po)
	  	  {
	  		  if(pwm_count_f > 0.9f)GPIO_SetBits(GPIOB,GPIO_Pin_14);
	  		  else GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	  		  pwm_count_f = 0.0f;
	  	  }
//----------------------------------------------------------------------------------------

	  c1 *= 8388607.0f;

	  if(m_vol_fl)
	  {
		  if(m_vol > 0.0f)m_vol -= 0.01f;
		  else {
			  m_vol = 0.0f;
			  m_vol_fl1 = 1;
		  }
	  }
	  else {
		  if(m_vol < 1.0f)m_vol += 0.01f;
		  else {
			  m_vol = 1.0f;
			  m_vol_fl1 = 1;
		  }
	  }

	  c1 *= p_vol*m_vol;
	  c2 *= p_vol*m_vol;
	  ccl =  -c1;
	  ccr =  -c2;

	  switch (sys_para[2]){
	  case 1: ccl = ccl >> 1 ; ccr = ccr >> 1; break;
	  case 2: ccr = -ccl;
	  }

	  ccl = __SSAT(ccl,24);
	  ccr = __SSAT(ccr,24);

	  ccl = ccl << 8;
	  ccr = ccr << 8;
	  cl = ccl;
	  cr = ccr;
	  cl = ror16(cl);
	  cr = ror16(cr);
	  init_get_dac_data().left.sample  = cl;
	  init_get_dac_data().right.sample = cr;

		 // расчет индикации громкости входа
	  vol_ind_vector[1] += vol_ind_k[0] * ( abs(ccl) * vol_ind_k[2] - vol_ind_vector[1]) ;
	  vol_ind_vector[2] += vol_ind_k[0] * ( abs(ccr) * vol_ind_k[2] - vol_ind_vector[2]) ;
}
void param_set(void)
{
  if(prog_data[cab_on])
  {
	  sig_volume(powf(prog_data[pres_lev],2.0)*(1.0/powf(31.0,2.0)));
	  p_vol = 1.0;
  }
  else p_vol = powf(prog_data[pres_lev],2.0)*(1.0/powf(31.0,2.0));
  if(!prog_data[sustein])prog_data[sustein] = 15;
  if(!prog_data[compr_vol])prog_data[compr_vol] = 15;
  gate_par(prog_data[gate_th] << 8);
  gate_par(1 | (prog_data[gate_att] << 8));
  comp_par(0 | (prog_data[sustein] << 8));
  comp_par(2 | (prog_data[compr_vol] << 8));
  if(!prog_data[preamp_vol])prog_data[preamp_vol] = 28;
  pream_vol = powf(prog_data[preamp_vol],2.0f)*(1.0f/powf(31.0f,2.0f));
  for(uint8_t i = 0 ; i < 3 ; i++)pre_param(i,prog_data[preamp_lo + i]);
  for(uint8_t i = 0 ; i < 5 ; i++)filt_ini(i , prog_data + fr1 , prog_data + q1);
  for(uint8_t i = 0 ; i < 5 ; i++)set_filt(i , prog_data[eq1 + i]);
  float lopas = powf(195 - prog_data[lop],2.0)*(19000.0/powf(195.0,2.0))+1000.0;
  SetLPF(lopas);
  float hipas = prog_data[hip]*(980.0/255.0)+20.0;
  SetHPF(hipas);
  set_shelf(prog_data[pr_vol]*(25.0/31.0));
  ear_vol = prog_data[e_vol]*(1.0/31.0);
}
void prog_ch(void)
{
  m_vol_fl1 = 0;
  m_vol_fl = 1;
  while(!m_vol_fl1);
  emb_string err_msg;
  if(load_pres(cab_data,err_msg,1) != true)
	{
	  cab_fl = 0;
	  Puls_conf(1);
	}
  else {
	  sig_load(cab_data , imp_buf_uint);
	  cab_fl = 1;
	  Puls_conf(0);
  }
  param_set();
  m_vol_fl = 0;
}
