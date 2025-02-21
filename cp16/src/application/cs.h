#ifndef __CS_H__

class TCSTask : public TTask
{
public:
  TCSTask () ;
  inline void Give()
     {
        sem-> Give () ;
     }
  inline void GiveFromISR()
    {
       BaseType_t HigherPriorityTaskWoken ;
       sem-> GiveFromISR ( &HigherPriorityTaskWoken) ;
       if ( HigherPriorityTaskWoken )
    	   TScheduler::Yeld();
    }

  inline void DisplayAccess( bool val )
  {
    DispalyAccess = val ;
    if ( val )
       Give();
  }

  inline bool DisplayAccess()
  {
    return DispalyAccess ;
  }


  private:
     void Code() ;

     TSemaphore* sem ;
     bool DispalyAccess ;

};


void preset_change(void);
void set_parameters(void);
bool CS_activateIr(const emb_string& irFilePath);

extern uint8_t ind_en;

extern TCSTask* CSTask ;

extern char loadedCab[256];

extern uint8_t m1_up[];
extern uint8_t m1_up1[];

extern volatile uint8_t rev_en;
extern volatile uint8_t rev_en1;
extern const char ver[];
extern const char dev[];

extern bool sw4_state ;

#endif /*__CS_H__*/
