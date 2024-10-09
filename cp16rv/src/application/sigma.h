#ifndef __SIGMA_H__

#include "appdefs.h"
        void adau_transmit (uint8_t* adr , uint32_t size );
        void adau_init_ic (void);
        void sig_load (float* cab_data , uint8_t* buf);
        void sig_volume(float val);
        void sig_load1 (uint8_t* buf);
        void dsp_mute(void);
        void dsp_run(void);
        void dsp_clear(void);

        typedef union
              { uint32_t sample;
          struct{
                uint16_t sampleL;
                uint16_t sampleH;};
            } ad_chanal_t ;
        typedef union
              { uint32_t sample;
          struct{
                uint16_t sampleL;
                uint16_t sampleH;};
            } da_chanal_t ;
        typedef union
            {
              int64_t val ;
              struct {
                ad_chanal_t left ;
                ad_chanal_t right ;
              };
            } ad_data_t ;
        typedef union
            {
              int64_t val ;
              struct {
                da_chanal_t left ;
                da_chanal_t right ;
              };
            } da_data_t ;


        ad_data_t& init_get_adc_data();
        da_data_t& init_get_dac_data();

        void sig_reset_init();
        void sig_reset(bool state);
        bool sig_reset();

        extern uint16_t buff_adc[];
        extern uint16_t buff_dac[];

#endif /*__SIGMA_H__*/
