#include "appdefs.h"
#include "cs.h"
#include "init.h"
#include "filt.h"
#include "sigma.h"
#include "eepr.h"
#include "AT45DB321.h"
#include "debug_led.h"
#include "amp_imp.h"
#include "Reverb/reverb.h"

/*
1.03.01 (202200004)
  сдбвиг нумерации версий прошивок по просьбе АМТ и Дмитрия
1.3.9a(202200004)
  ребилд
1.3.9 (202200717)
  рефакторинг keychk(void)
  автоматизация герерации firmware, имя и версия прошивки теперь снаружи кода в options.mk
1.3.8 (20220622)
  рефакторинг (удаление мертвого кода), перенос в CP16.V.
1.3.7 (20220615)
  исправлены ошибки в команде pwc
1.3.6 (20220614)
  команда pwl безусловно чистить целевую директорию пресета от wav
  перед попыткой копированя из /tmp_preset
1.3.5 (20220611)
  результат команды pws унифицирован - всегла возвращает END\n
  pws чистит /tmp_preset если в пресете источнике небыло файла wav
1.3.4 (20220605)
  в команде rns
     изменен формат результата для упрощения алгоритма парсера на внешнем приложении
     добавлена функция нерекурсивного поиска wav целевой директории с целью вывода только "первого" файла
  временные данные пресета теперь хранятся в папке /tmp_preset
  временный wav файл теперь сохраняет свое имя
  при загрузке в пресет tmp wav - все файлы wav в пресете удаляются
1.3.3 (20220520)
  добавлено сохранение и загрузка wav через tmp
1.3.2 (20220513)
  изменено кодирование bank/preset 2:2 бит в 4:4
1.3.1
  добавлено копирование пресетов из одного в другой без выгрузки на хост
3.2
  симметрично для версии не PA
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
1.04.06(20241007)
  функционал map0 и map1 протянут с селектора на материнской плате(переключатели), втыкание со стрта map0
1.04.07(20241007)
  добавлена команда sw1
*/
const uint8_t ver[] = FIRMWARE_VER  ;
const uint8_t dev[] = FIRMWARE_NAME ;

#define eq_stage 5
#define presen_stage 1
#define preamp_stage 6
arm_fir_instance_f32 fir_instance ;
arm_biquad_casd_df1_inst_f32 eq_instance;
arm_biquad_casd_df1_inst_f32 presen_instance;
arm_biquad_casd_df1_inst_f32 preamp_instance;
float __CCM_BSS__ Coeffs[taps_fir];
float __CCM_BSS__ Src[block_size];
float __CCM_BSS__ Dst[block_size];
float __CCM_BSS__ State[taps_fir + block_size -1];
float __CCM_BSS__ coeff_eq[eq_stage * 5];
float __CCM_BSS__ stage_eq[eq_stage * 4];
float __CCM_BSS__ coeff_presen[presen_stage * 5];
float __CCM_BSS__ stage_presen[presen_stage * 4];
float __CCM_BSS__ coeff_preamp[preamp_stage * 5];
float __CCM_BSS__ stage_preamp[preamp_stage * 4];


uint8_t ind_en;

uint8_t usb_flag = 0;
bool sw4_state = 1;

volatile uint32_t key_buf;

float m_vol = 0.0f;
volatile uint8_t m_vol_fl = 0;
volatile uint8_t m_vol_fl1 = 0;
float p_vol = 1.0f;
float amp_vol = 1.0f;
float amp_sla = 1.0f;
float ear_vol = 1.0f;

extern ad_data_t adc_data[];
extern da_data_t dac_data[];

extern volatile uint8_t usb_type;

void start_usb(uint8_t type);
inline float soft_clip_amp(float in);
inline float soft_clip_pre(float in);
inline float dc_block(float in);
inline void indic_pwm(float in);

volatile  float vol_ind_vector[3];

TCSTask* CSTask ;

TCSTask::TCSTask () : TTask()
	   {
	   }
inline void keychk(void)
{
	static  uint8_t key_buf_local = 0xff;
	if ( !sw4_state ) return ;

    key_buf = GPIOB->IDR & 0xc03;
    key_buf |= key_buf >> 8;
    key_buf = ~key_buf & 0xf;
    if(key_buf_local != key_buf)
    {
    	key_buf_local = key_buf;
	  bank_pres[0] = key_buf_local >> 2;
	  bank_pres[1] = key_buf_local & 3;
	  prog_ch();
    }
}

void TCSTask::Code()
{

  init();

  Gate_Change_Preset ();
  Compressor_init();
  Compressor_Change_Preset(0,0);

  arm_fir_init_f32(&fir_instance, taps_fir, Coeffs, State, block_size);
  arm_biquad_cascade_df1_init_f32(&eq_instance, eq_stage , coeff_eq , stage_eq);
  arm_biquad_cascade_df1_init_f32(&presen_instance, presen_stage , coeff_presen , stage_presen);
  arm_biquad_cascade_df1_init_f32(&preamp_instance, preamp_stage , coeff_preamp , stage_preamp);

  flash_folder_init();

  adau_init_ic();

  dela(0xffff);

  if(sys_para[2] == 2)sig_invert(1);

  extern const uint8_t ver[];
  uint8_t temp = 0;
  for(uint8_t i = 0 ; i < 8 ; i++)if(ver[i] != sys_para[23 + i])temp++;
  if(temp)
  {
	  for(uint8_t i = 0 ; i < 11 ; i++)sys_para[13 + i] = dev[i];
//	  for(uint8_t i = 0 ; i < 8 ; i++)sys_para[23 + i] = ver[i];
	  for(uint8_t i = 0 ; i < 8 ; i++)sys_para[23 + i] = ver[i];
	  save_sys();
  }

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
	  else {
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

int32_t __CCM_BSS__ ccl[block_size];
int32_t __CCM_BSS__ ccr[block_size];
int32_t __CCM_BSS__ cc_cab;
uint32_t __CCM_BSS__ cl[block_size];
uint32_t __CCM_BSS__ cr[block_size];
uint8_t block_po = 0;

float __CCM_BSS__ inp_eq_preamp[block_size];
float __CCM_BSS__ out_eq_preamp[block_size];
uint8_t eq_preamp_fl;
float __CCM_BSS__ inp_fir_amp[block_size];
float __CCM_BSS__ out_fir_amp[block_size];
float __CCM_BSS__ inp_fir_amp1[block_size];
float __CCM_BSS__ out_fir_amp1[block_size];
uint8_t fir_amp_fl;
float __CCM_BSS__ inp_eq[block_size];
float __CCM_BSS__ out_eq[block_size];
float __CCM_BSS__ inp_eq_1[block_size];
float __CCM_BSS__ out_eq_1[block_size];
uint8_t eq_fl;
float __CCM_BSS__ inp_sampleL[block_size];
float __CCM_BSS__ inp_sampleR[block_size];
float __CCM_BSS__ out_sampleL[block_size];
float __CCM_BSS__ out_sampleR[block_size];

float __CCM_BSS__ gate_buf[block_size];
float __CCM_BSS__ compr_buf[block_size];


float ind_clean_max;
float ind_cab_max;
uint16_t pwm = 0;
uint16_t pwm_count;
float pwm_count_f = 0.0f;
float pwm_count_f_temp = 0.0f;
float pwm_count_f_old = 0.0f;
uint16_t pwm_count_po;
uint8_t pick_fl;

float pream_vol = 1.0f;
float __CCM_BSS__ fir_in[block_size];
float __CCM_BSS__ fir_out[block_size];
uint8_t dma_ht_fl = 0;

extern "C" void DMA1_Stream3_IRQHandler()
{
//--------------------------------------------------------Start---------------------
  if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_HTIF3))
  {
	  DMA_ClearITPendingBit ( DMA1_Stream3 , DMA_IT_HTIF3);
	  dma_ht_fl = 0;
  }
  else {
	  DMA_ClearITPendingBit ( DMA1_Stream3 , DMA_IT_TCIF3);
	  dma_ht_fl = 1;
  }

  float avg_in = 0 ;

  for(uint8_t i = 0 ; i < block_size; i++)
  {
	  uint8_t a = i + dma_ht_fl * block_size;
	  cl[i] = ror16(adc_data[a].left.sample);
	  cr[i] = ror16(adc_data[a].right.sample);
	  ccl[i] = cl[i];
	  ccr[i] = cr[i];
	  ccl[i] = ccl[i] >> 8;
	  ccr[i] = ccr[i] >> 8;

	  inp_sampleL[i] = ccl[i];
	  inp_sampleR[i] = ccr[i];
	  inp_sampleL[i] = dc_block(inp_sampleL[i]) * 0.000000119f;  //-----> Output ADC
	  inp_sampleR[i] *= 0.000000119f;                            //-----> Output CAB
	  gate_buf[i] = gate_out(inp_sampleL[i]);
	  if(prog_data[compr_on])inp_sampleL[i] = compr_out(inp_sampleL[i]);

	  avg_in += abs((int32_t)(inp_sampleL[i] * 8388607.0f));
  }



//-------------------------------------Fender--------------------------------------------
  if(prog_data[eq_po])
  {
     arm_biquad_cascade_df1_f32(&eq_instance, inp_sampleL, out_eq, block_size);
     if(prog_data[eq_on])for(uint8_t i = 0 ; i < block_size; i++)inp_sampleL[i] = out_eq[i];
  }
  arm_biquad_cascade_df1_f32(&preamp_instance, inp_sampleL, out_eq_preamp, block_size);
  for(uint8_t i = 0 ; i < block_size; i++)
  {
     if(prog_data[preamp_on])inp_sampleL[i] = out_eq_preamp[i] * pream_vol * 3.0f;
  }
//--------------------------------------Amplifier----------------------------------------
  if(prog_data[amp_on])
  {
     for(uint8_t i = 0 ; i < block_size; i++)inp_sampleL[i] = soft_clip_amp(inp_sampleL[i] * amp_vol) * amp_sla;
     if(prog_data[a_t] != 8)
     {
		  arm_fir_f32(&fir_instance,inp_sampleL,out_fir_amp,block_size);
		  for(uint8_t i = 0 ; i < block_size; i++)inp_sampleL[i] = out_fir_amp[i];
     }
  }
//-----------------------------------------------------------
  //GPIO_SetBits(GPIOB,GPIO_Pin_7);
//---------------------------------------Cab data or Dry signal-------------------------
  if(!prog_data[cab_on] || !cab_fl)for(uint8_t i = 0 ; i < block_size; i++)inp_sampleR[i] = inp_sampleL[i];
//--------------------------------------HPF----------------------------------------------
  if(prog_data[hip_on])
  {
	  for(uint8_t i = 0 ; i < block_size; i++)inp_sampleR[i] = filt_hp(inp_sampleR[i]);
  }
//--------------------------------------EQ-----------------------------------------------
  if(!prog_data[eq_po])
  {
     arm_biquad_cascade_df1_f32(&eq_instance, inp_sampleR, out_eq, block_size);
     if(prog_data[eq_on])for(uint8_t i = 0 ; i < block_size; i++)inp_sampleR[i] = out_eq[i];
  }
//---------------------------------------LPF-----------------------------------------------
  if(prog_data[lop_on])for(uint8_t i = 0 ; i < block_size; i++)inp_sampleR[i] = filt_lp(inp_sampleR[i]);
//--------------------------------------Presence-------------------------------------------
  arm_biquad_cascade_df1_f32(&presen_instance, inp_sampleR, out_sampleR, block_size);
  if(prog_data[pr_on])for(uint8_t i = 0 ; i < block_size; i++)inp_sampleR[i] = out_sampleR[i];

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
  for(uint8_t i = 0 ; i < block_size; i++)
  {
	 uint8_t a = i + dma_ht_fl * block_size; //pointer
	 if(prog_data[gate_on])inp_sampleR[i] *= gate_buf[i];  // gate

	 if(prog_data[er_on])accum = inp_sampleR[i] * 0.7f;
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
	 inp_sampleR[i] += ear_outL * ear_vol;
//---------------------------------------------PWM indicator---------------------------------
 	  float a_ind = fabsf(inp_sampleR[i]);
  	  if(pwm_count_f < a_ind)pwm_count_f = a_ind;
  	  pwm_count_po++;
  	  pwm_count_po &= 0x1ff;
  	  if(!pwm_count_po)
  	  {
  		  if(pwm_count_f > 0.9f)GPIO_SetBits(GPIOB,GPIO_Pin_14);
  		  else GPIO_ResetBits(GPIOB,GPIO_Pin_14);
  		  pwm_count_f = 0.0f;
  	  }
//--------------------------------------------------------------------------------------

	 inp_sampleR[i] = out_clip(inp_sampleR[i] * p_vol);// * mute_mas;
	 ccl[i] = inp_sampleR[i] * 8388607.0f * m_vol;
	 if(sys_para[2] == 1)ccl[i] = ccl[i] >> 1;
	 cl[i] = ccl[i] << 8;
	 dac_data[a].right.sample = ror16(cl[i]); // master out
	 ccl[i] = out_clip(inp_sampleL[i] * 0.3f) * 8388607.0f * m_vol;
	 cl[i] = ccl[i] << 8;
	 dac_data[a].left.sample = ror16(cl[i]);

	 // расчет индикации громкости входа
	 vol_ind_vector[1] += vol_ind_k[0] * ( abs(ccl[i]) * vol_ind_k[2] - vol_ind_vector[1]) ;
	 vol_ind_vector[2] += vol_ind_k[0] * ( abs(ccl[i]) * vol_ind_k[2] - vol_ind_vector[2]) ;
  }
//---------------------------------------------End-------------------------------------
  //GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}

void param_set(void)
{
  if(!prog_data[preamp_vol])prog_data[preamp_vol] = 13;
  if(!prog_data[amp_slave])prog_data[amp_slave] = 31;
  if(!prog_data[sustein])prog_data[sustein] = 15;
  if(!prog_data[compr_vol])prog_data[compr_vol] = 15;
  gate_par(prog_data[gate_th] << 8);
  gate_par(1 | (prog_data[gate_att] << 8));
  comp_par(0 | (prog_data[sustein] << 8));
  comp_par(2 | (prog_data[compr_vol] << 8));
  p_vol = powf(prog_data[pres_lev],2.0f)*(1.0f/powf(31.0f,2.0f));
  pream_vol = powf(prog_data[preamp_vol],2.0f)*(1.0f/powf(31.0f,2.0f));
  for(uint8_t i = 0 ; i < 3 ; i++)pre_param(i,prog_data[preamp_lo + i]);
  amp_vol = powf(prog_data[a_vol],2.0f)*(10.0f/powf(31.0f,2.0f)) + 1.0f;
  amp_sla = powf(prog_data[amp_slave],4.0f)*(0.99f/powf(31.0f,4.0f)) + 0.01f;
  int a = taps_fir - 1;
  switch(prog_data[a_t]){
  case 0:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = PP_6L6[i];break;
  case 1:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = PP_EL34[i];break;
  case 2:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = SE_6L6[i];break;
  case 3:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = SE_EL34[i];break;
  case 4:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = tc_1[i];break;
  case 5:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = fender[i];break;
  case 6:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = jcm800[i];break;
  case 7:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = lc50[i];break;
  case 9:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = mes_mod[i];break;
  case 10:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = mes_vint[i];break;
  case 11:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = Pr0_Re0_5150[i];break;
  case 12:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = Pr5_Re5_5150[i];break;
  case 13:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = Pr8_Re7_5150[i];break;
  case 14:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = Pr9_Re8_5150[i];break;
  }
  for(uint8_t i = 0 ; i < 5 ; i++)filt_ini(i , prog_data + fr1 , prog_data + q1);
  for(uint8_t i = 0 ; i < 5 ; i++)set_filt(i , prog_data[eq1 + i]);
  float lopas = powf(195 - prog_data[lop],2.0f)*(19000.0f/powf(195.0f,2.0f)) + 1000.0f;
  SetLPF(lopas);
  float hipas = prog_data[hip]*(980.0f/255.0f) + 20.0f;
  SetHPF(hipas);
  set_shelf(prog_data[pr_vol]*(31.0f/31.0f));
  ear_vol = prog_data[e_vol] * (1.0/31.0);
}


void prog_ch(void)
{
  // смена пресета

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



inline float soft_clip_amp(float in)
{
	float aaa = fabsf(in);
	if(aaa < 0.1618f)aaa *= 4.294115f;
	else aaa = 0.81f * ((aaa - 0.11f) / (fabsf(aaa - 0.11f) + 0.033f)) + 0.2f;

	if(in < 0.0f)in = -aaa;
	else in = aaa;
	return in;
}
inline float soft_clip_pre(float in)
{
	if(in > 0.0f)
	{
		if(in >= 0.21697f)
		{
			in = 0.54f * ((in - 0.12f) / (fabsf(in - 0.12f) + 0.03f)) - 0.195441f;
		}
	}
	else {
		in = -in;
		if(in >= 0.7117f)in = 0.8f * ((in - 0.53f) / (fabsf(in - 0.53f) + 0.1f)) + 0.19569f;
		in = -in;
	}
	return in;
}

float in_dc_old;
float out_dc_old;
inline float dc_block(float in)
{
	float a = in - in_dc_old + 0.995f * out_dc_old;
	in_dc_old = in;
	out_dc_old = a;
	return a;
}

inline void indic_pwm(float in)
{
	  float a = fabsf(in);
	  if(pwm_count_f < a)pwm_count_f = a;
	  pwm_count_po++;
	  if(!pwm_count_po)
	  {
		  if(pwm_count_f > 0.9f)
		  {
			  pwm_count_f_old = ((pwm_count_f - 0.9f)*(pwm_count_f - 0.9f)) * (256.0f/(0.1f*0.1f));
			  pick_fl = 1;
		  }
		  else
			{
			  pwm_count_f_old = 0.0f;
			  pick_fl = 0;
		  }
		  pwm_count_f = 0.0f;
	  }
	  if(pwm_count_f_temp < pwm_count_f_old)pwm_count_f_temp += 0.01f;
	  else {
		  if(pwm_count_f_temp > pwm_count_f_old)
		  {
			  if(pick_fl)pwm_count_f_temp -= 0.01f;
			  else pwm_count_f_temp -= 1.0f;
		  }
		  else pwm_count_f_temp = pwm_count_f_old;
	  }
	  pwm_count = pwm_count_f_temp;
}
void sig_invert(uint8_t val)
{
  uint8_t buf[6] = {0x03,0xd7,0,0x80,0,0};
  if(val)buf[2] = 0xff;
  adau_transmit((uint8_t*)buf , 6);
}
