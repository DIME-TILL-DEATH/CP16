#include "usb.h"

#include "class/msc/usbd_msc_core.h"

#include "class/cdc/usbd_cdc_core.h"
#include "class/cdc/usbd_cdc_if.h"

#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_dcd_int.h"
#include "cs.h"
#include "sigma.h"


#include "console.h"


TUsbTask* UsbTask ;

//------------------------------------------------------------------------------
TUsbTask::TUsbTask (mode_t val) : TTask()
	   {
	       if ( val == mCDC )
	       {
	    	   USBD_DeviceDesc[4] = 2;
	    	   USBD_DeviceDesc[5] = 2;
	    	   USBD_DeviceDesc[8] = 0x83;
	    	   USBD_DeviceDesc[9] = 0x04;
	    	   USBD_DeviceDesc[10] = 0x40;
	    	   USBD_DeviceDesc[11] = 0x57;
	       }

	       USB_OTG_dev = new USB_OTG_CORE_HANDLE ;

		   USBD_Init( (USB_OTG_CORE_HANDLE*)USB_OTG_dev,
	                   USB_OTG_FS_CORE_ID,
	                   &USR_desc,
					   val == mMSC ? &USBD_MSC_cb : &USBD_CDC_cb ,
	                   &USR_cb,
	                   false);
	   }

void TUsbTask::Code()
	{
		//Delay(2500);


	     bool configured = false ;
	     while(1)
 	 	{

	    	 USBD_OTG_ISR_Handler ((USB_OTG_CORE_HANDLE*)USB_OTG_dev);

                   if ( ((USB_OTG_CORE_HANDLE*)USB_OTG_dev)->dev.device_status == USB_OTG_CONFIGURED && configured == false)
                     {
#if 0
                	   //umount
                	   sig_reset(true);
                       //FSTask->SendCommand( TFsBrowser::bcFsUmount );
                       CSTask->DisplayAccess(false);
                       ind_en = 0;
                       DisplayTask->Clear();
                       DisplayTask->StringOut(0,2,TDisplayTask::fntSystem,0,(uint8_t*)"PC is connected");
#endif
                       configured = true ;
                     }
                   if ( ((USB_OTG_CORE_HANDLE*)USB_OTG_dev)->dev.device_status != USB_OTG_CONFIGURED && configured == true)
                     {
                       //NVIC_SystemReset();
                       //configured = false ;
                     }

 	 	}
	}
//------------------------------------------------------------------------------
extern "C" uint16_t CDC_Recv (uint8_t* buf, uint32_t len)
{
    const char* v = reinterpret_cast<const char*>(buf);
	for ( size_t i = 0 ; i<len ; i++ )
	   if ( ConsoleTask->WriteToInputBuff( v + i) == TQueue::qsrQueueFull )
		   return USBD_OK ;
	return USBD_OK ;
}







int cdc_send_char( const int c )
{
    char tmp = c ;
    VCP_fops.pIf_DataTx( (uint8_t*)&tmp , 1) ;
    return c ;
}

char* cdc_send_string( const char* s )
{
    VCP_fops.pIf_DataTx( (uint8_t*)s , kgp_sdk_libc::strlen(s)) ;
    return (char*)s ;
}

size_t cdc_send_buf( const char* buf , size_t size )
{
    VCP_fops.pIf_DataTx( (uint8_t*)buf , size) ;
    return size ;
}

//---------------------------------------------------------------------------
int cdc_recv_char (int& c)
{
	    char tmp ;
        ConsoleTask->ReadFromInputBuff( &tmp ) ;
        c = tmp ;
        return c;
}


size_t cdc_recv_string (emb_string& dest)
{
        dest.clear() ;
        int c ;
        do
        {
        	char tmp ;
        	ConsoleTask->ReadFromInputBuff(&tmp) ;
                c = tmp ;
                if ( !c ) return dest.length() ;
                else dest.push_back(c);
        }while(1) ;
}

size_t cdc_recv_line (emb_string& dest)
{
        dest.clear() ;
        int c ;
        do
        {
        	char tmp ;
        	ConsoleTask->ReadFromInputBuff(&tmp) ;
                c = tmp ;
                if ( c == '\n' or c == '\r' ) return dest.length() ;
                else dest.push_back(c);
        }while(1) ;
}

size_t cdc_recv_buf (char* buf , size_t size)
{
        size_t c = 0 ;
        while ( c < size )
        {
                ConsoleTask->ReadFromInputBuff(buf++) ;
                c++ ;
        }
        return c ;
}

void console_task_hw_init()
		{

		}

void cdc_enable(int& ret)
		{

		}

void cdc_disable(int& ret)
		{

		}

TConsoleTask::readline_io_t cdc_io
{
    .console_task_hw_init = console_task_hw_init,

    .enable = cdc_enable,
	.disable = cdc_disable,

    .send_char = cdc_send_char,
    .recv_char = cdc_recv_char,

    .send_string = cdc_send_string,
    .recv_string = cdc_recv_string,
	.recv_line   = cdc_recv_line,

    .send_buf = cdc_send_buf,
    .recv_buf = cdc_recv_buf,
} ;
