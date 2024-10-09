/**
  ******************************************************************************
  * @file    usbd_storage_template.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Memory management layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_mem.h"
#include "appdefs.h"

//#define SD_USB

#ifndef SD_USB
   #include "storage.h"
#else
  #include "diskio.h"
  #include "sdcard_diskio.h"
#endif



/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata[] = {//36
  
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (USBD_STD_INQUIRY_LENGTH - 5),
  0x00,
  0x00,	
  0x00,
  'K', 'l', 'e', 'n', 'T', 'e', 'c', 'h', /* Manufacturer : 8 bytes */
  'T', 'o', 'o', 'l', 's', ' ', 'f', 'a', /* Product      : 16 Bytes */
  'c', 't', 'o', 'r', 'y', ' ', ' ', ' ',
  '0', '.', '0' ,'1',                     /* Version      : 4 Bytes */
}; 


/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the microSD card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Init (uint8_t lun)
{

    #ifndef SD_USB

    #else
	  sdcard_disk_initialize ();
    #endif
	  return (0);
}

/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{



    #ifndef SD_USB
      *block_num = storage_get_block_count(lun) ;
	  *block_size = storage_get_block_size(lun) ;
    #else
      *block_num = sdcard_disk_get_block_count(lun) ;
	  *block_size = sdcard_disk_get_block_size(lun) ;
    #endif

	return (0);
}

/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  STORAGE_IsReady (uint8_t lun)
{
    #ifndef SD_USB
	   return (0);
    #else
	   return (int8_t)sdcard_disk_status() ;
    #endif
}

/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
	return  0;
}

/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Read (uint8_t lun, 
                 uint8_t *buf, 
                 uint32_t blk_index,
                 uint16_t blk_count)
{
    #ifndef SD_USB
	  return storage_read (buf, blk_index, blk_count);
    #else
      return (int8_t)sdcard_disk_read (buf , blk_index , blk_count);
    #endif
}
/*******************************************************************************
* Function Name  : Write_Memory
* Description    : Handle the Write operation to the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Write (uint8_t lun, 
                  uint8_t *buf, 
                  uint32_t blk_index,
                  uint16_t blk_count)
{
    #ifndef SD_USB
       return storage_write (buf, blk_index, blk_count);
    #else
       return (int8_t)sdcard_disk_write (buf , blk_index , blk_count);
    #endif
}
/*******************************************************************************
* Function Name  : Write_Memory
* Description    : Handle the Write operation to the STORAGE card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_GetMaxLun (void)
{

  #ifndef SD_USB
	 return storage_get_lun_number() - 1;
  #else
     return 0 ;
  #endif
}


const USBD_STORAGE_cb_TypeDef USBD_MICRO_SDIO_fops =
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  STORAGE_Inquirydata,

};


USBD_STORAGE_cb_TypeDef  *USBD_STORAGE_fops = (USBD_STORAGE_cb_TypeDef*)&USBD_MICRO_SDIO_fops;

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


