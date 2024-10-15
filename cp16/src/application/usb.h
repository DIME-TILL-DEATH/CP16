#ifndef __USB_H__
#define __USB_H__

#include "appdefs.h"
#include "console.h"

extern TConsoleTask::readline_io_t cdc_io ;

class TUsbTask : public TTask
{
  public:

	 typedef enum { mCDC, mMSC } mode_t;

     TUsbTask () ;
     virtual ~TUsbTask() {} ;
     TUsbTask (mode_t val);

     inline void* UsbOtgDevHandle() { return USB_OTG_dev ; }
  private:
     void Code() ;
     void*  USB_OTG_dev ;
};

extern TUsbTask* UsbTask ;

#endif /*__USB_H__*/
