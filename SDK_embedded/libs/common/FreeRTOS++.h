#ifndef __TASK_H__
#define __TASK_H__

#include "FreeRTOS_headers.h"

#include "supc++.h" // operator new implimentation

namespace FreeRTOS

{

class TScheduler
  {
    public:
    // --- Kernel Control API

    static inline void Yeld()
      {
        taskYIELD() ;
      }

    static inline void EnterCritical()
          {
            taskENTER_CRITICAL() ;
          }
    static inline void ExitCritical()
          {
           taskEXIT_CRITICAL() ;
          }
    static inline void DisableInterrupt()
          {
           taskDISABLE_INTERRUPTS();
          }
    static inline void EnableInterrupt()
          {
           taskENABLE_INTERRUPTS() ;
          }

    static inline void StartScheduler()
              {
                vTaskStartScheduler() ;
              }
    static inline void EndScheduler()
                  {
                    vTaskEndScheduler() ;
                  }
    static inline void SuspendAll()
              {
               vTaskSuspendAll() ;
              };
    static inline portBASE_TYPE ResumeAll()
              {
               return xTaskResumeAll() ;
              }

    static inline void vTaskStepTick( TickType_t xTicksToJump )
    		{
    	 	 	 vTaskStepTick( xTicksToJump );
    		}



  };

class TTaskUtilities
  {
    public:

	typedef enum
		{   ssSuspend=0,
			ssNotStarted,
			ssRunning,
		} TSchedulerState ;

      static inline TaskHandle_t GetCurrentTaskHandle()
          {
            return xTaskGetCurrentTaskHandle();
          }
      static inline TaskHandle_t GetIdleTaskHandle()
          {
            return xTaskGetIdleTaskHandle();
          }
      static inline unsigned portBASE_TYPE GetStackHighWaterMark( TaskHandle_t xTask = NULL )
          {
            return uxTaskGetStackHighWaterMark(xTask);
          }
      static inline eTaskState GetTaskState( TaskHandle_t xTask = NULL )
          {
            return eTaskGetState(xTask);
          }
      static inline char* GetTaskName( TaskHandle_t xTask = NULL )
          {
            return (char*)pcTaskGetTaskName(xTask);
          }

      static inline TickType_t GetTickCount()
          {
            return xTaskGetTickCount();
          }
      static inline TickType_t GetTickCountFromISR()
          {
            return xTaskGetTickCountFromISR();
          }
      static inline TSchedulerState GetSchedulerState()
        {
          return (TSchedulerState) xTaskGetSchedulerState() ;
        }
      static inline unsigned portBASE_TYPE GetNumberOfTasks()
        {
          return uxTaskGetNumberOfTasks();
        }
      /* static inline void TaskList( char  *pcWriteBuffer )
        {
           vTaskList( pcWriteBuffer );
        }*/
        static inline unsigned portBASE_TYPE TaskGetSystemState( TaskStatus_t *pxTaskStatusArray, UBaseType_t uxArraySize, unsigned long *pulTotalRunTime)
        {
          return uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, pulTotalRunTime );
        }
        static inline void GetRunTimeStats( char  *pcWriteBuffer)
        {
    	   	vTaskGetRunTimeStats(pcWriteBuffer);
        }
        #if ( configUSE_APPLICATION_TASK_TAG == 1 )
          static inline void SetApplicationTaskTag(TaskHandle_t xTask, TaskHookFunction_t pxHookFunction)
            {
        	vTaskSetApplicationTaskTag( xTask, pxHookFunction );
            }
          static inline TaskHookFunction_t GetApplicationTaskTag(TaskHandle_t xTask=NULL)
            {
              return xTaskGetApplicationTaskTag( xTask );
            }
        #endif
        static inline BaseType_t CallApplicationTaskHook(TaskHandle_t xTask, void *pvParameter)
        {
        	return xTaskCallApplicationTaskHook(xTask,pvParameter);
        }

       /*static inline signed portBASE_TYPE IsTaskSuspended(TaskHandle_t xTask)
        	{
    	   	   return xTaskIsTaskSuspended( xTask );
        	}*/

      /*static inline void TaskListEx( char  *pcWriteBuffer )
        {
           vTaskListEx( pcWriteBuffer );
        };*/
  };

class TCriticalSection
	{
		public:
			TCriticalSection() { TScheduler::EnterCritical() ; }
			~TCriticalSection() { TScheduler::ExitCritical() ; }
	};

class TQueue
  {
	private:
		QueueHandle_t Handle ;
	public:

		typedef enum { qsrPass = pdTRUE , qsrQueueFull = errQUEUE_FULL } TQueueSendResult ;
		typedef enum { qrrPass = pdTRUE , qrrQueueEmpty=errQUEUE_EMPTY } TQueueReceiveResult ;

		inline TQueue ( UBaseType_t uxQueueLength, UBaseType_t uxItemSize )
			{
				Handle = xQueueCreate( uxQueueLength , uxItemSize ) ;
			}
		inline ~TQueue ()
			{
				vQueueDelete( Handle );
				Handle = NULL ;
			}
		inline bool IsCreated()
			{
			  bool created = false ;
			  if (Handle) created = true  ;
			  return created ;

			}

		static inline UBaseType_t  MessagesWaiting(const QueueHandle_t xQueue)
			{
				return uxQueueMessagesWaiting (xQueue) ;
			}
		inline UBaseType_t  MessagesWaiting()
			{
				return uxQueueMessagesWaiting (Handle) ;
			}

		static inline UBaseType_t MessagesWaitingFromISR( const QueueHandle_t xQueue )
                        {
		              return uxQueueMessagesWaitingFromISR( xQueue ) ;
                        }

		inline UBaseType_t MessagesWaitingFromISR()
		        {
			       return uxQueueMessagesWaitingFromISR( Handle ) ;
		        }

		static inline UBaseType_t SpacesAvailable(QueueHandle_t xQueue)
                       {
		              return uxQueueSpacesAvailable(xQueue );
                       }

		inline UBaseType_t SpacesAvailable()
                       {
                              return uxQueueSpacesAvailable(Handle);
                       }


		static inline TQueueSendResult  SendToBack(QueueHandle_t xQueue , const void * pvItemToQueue , TickType_t xTicksToWait )
			{
				return (TQueueSendResult)xQueueSendToBack( xQueue , pvItemToQueue, xTicksToWait ) ;
			}
		inline TQueueSendResult  SendToBack(const void * pvItemToQueue , TickType_t xTicksToWait )
			{
				return (TQueueSendResult)xQueueSendToBack( Handle , pvItemToQueue, xTicksToWait ) ;
			}

		static inline TQueueSendResult  SendToFront(QueueHandle_t xQueue, const void * pvItemToQueue , TickType_t xTicksToWait )
			{
				return (TQueueSendResult) xQueueSendToFront( xQueue , pvItemToQueue, xTicksToWait ) ;
			}
		inline TQueueSendResult  SendToFront(const void * pvItemToQueue , TickType_t xTicksToWait )
			{
				return (TQueueSendResult) xQueueSendToFront( Handle , pvItemToQueue, xTicksToWait ) ;
			}

		static inline TQueueReceiveResult  Receive (QueueHandle_t xQueue, void * pvBuffer , TickType_t xTicksToWait )
			{
				return (TQueueReceiveResult) xQueueReceive( xQueue , pvBuffer, xTicksToWait ) ;
			}
		inline TQueueReceiveResult  Receive (void * pvBuffer , TickType_t xTicksToWait )
			{
				return (TQueueReceiveResult) xQueueReceive( Handle , pvBuffer, xTicksToWait ) ;
			}

		static inline  TQueueReceiveResult  Peak (QueueHandle_t xQueue, void * pvBuffer , TickType_t xTicksToWait )
			{
				return (TQueueReceiveResult) xQueuePeek( xQueue , pvBuffer, xTicksToWait ) ;
			}
		inline  TQueueReceiveResult  Peak (void * pvBuffer , TickType_t xTicksToWait )
			{
				return (TQueueReceiveResult) xQueuePeek( Handle , pvBuffer, xTicksToWait ) ;
			}

		static inline TQueueSendResult  SendToBackFromISR (QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken )
			{
				return (TQueueSendResult) xQueueSendToBackFromISR( xQueue , pvItemToQueue, pxHigherPriorityTaskWoken ) ;
			}
		inline TQueueSendResult  SendToBackFromISR (const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken )
			{
				return (TQueueSendResult) xQueueSendToBackFromISR( Handle , pvItemToQueue, pxHigherPriorityTaskWoken ) ;
			}

		static inline TQueueSendResult  SendToFrontFromISR (QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken )
			{
				return (TQueueSendResult) xQueueSendToFrontFromISR( xQueue , pvItemToQueue, pxHigherPriorityTaskWoken ) ;
			}
		inline TQueueSendResult  SendToFrontFromISR (const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken )
			{
				return (TQueueSendResult) xQueueSendToFrontFromISR( Handle , pvItemToQueue, pxHigherPriorityTaskWoken ) ;
			}

		static inline TQueueReceiveResult  ReceiveFromISR (QueueHandle_t xQueue, void *pvBuffer, BaseType_t *pxTaskWoken )
			{
				return (TQueueReceiveResult) xQueueReceiveFromISR( xQueue , pvBuffer, pxTaskWoken ) ;
			}
		inline TQueueReceiveResult  ReceiveFromISR (void *pvBuffer, BaseType_t *pxTaskWoken )
			{
				return (TQueueReceiveResult) xQueueReceiveFromISR( Handle , pvBuffer, pxTaskWoken ) ;
			}

		static inline void  AddToRegistry (QueueHandle_t xQueue, const char *pcQueueName)
			{
				 vQueueAddToRegistry( xQueue , pcQueueName ) ;
			}
		inline void  AddToRegistry (const char *pcQueueName)
			{
				 vQueueAddToRegistry( Handle , pcQueueName ) ;
			}
		/*  in v7.3.0 vQueueUnregisterQueue set a static function
		static inline void UnregisterQueue (QueueHandle_t xQueue )
			{
				vQueueUnregisterQueue( xQueue ) ;
			}
		inline void UnregisterQueue ()
			{
				vQueueUnregisterQueue( Handle ) ;
			}*/

  };

class TSemaphore
	{
		private:
			SemaphoreHandle_t Handle ;
		public:
			typedef enum { fstBinary = 0 , fstMutex, fstRecursiveMutex, fstCounting } TFreeRTOSSemaphoreType ;

			inline TSemaphore ( TFreeRTOSSemaphoreType SemaphoreType ,  UBaseType_t uxCountValue = 0 , UBaseType_t uxInitialCount = 0 )
				{
					switch ( SemaphoreType )
					{
						case fstBinary : vSemaphoreCreateBinary (Handle) ; break ;
						case fstMutex  : Handle = xSemaphoreCreateMutex() ; break ;
						case fstRecursiveMutex : Handle = xSemaphoreCreateRecursiveMutex () ; break ;
						case fstCounting : Handle = xSemaphoreCreateCounting( uxCountValue, uxInitialCount ) ; break ;
						default : Handle = NULL ;
					}
				}
			inline ~TSemaphore ()
				{
					vQueueDelete(Handle);
					Handle = NULL ;
				}

			inline SemaphoreHandle_t GetHandle() { return Handle ; }

			static inline BaseType_t Take( SemaphoreHandle_t Handle , TickType_t  BlockTime)
			        {
			                return xSemaphoreTake( Handle , BlockTime ) ;
			        }
			inline BaseType_t Take( TickType_t  BlockTime)
				{
					return xSemaphoreTake( Handle , BlockTime ) ;
				}
			static inline BaseType_t  TakeRecursive ( SemaphoreHandle_t Handle , TickType_t  BlockTime )
			                                {
			                                        return xSemaphoreTakeRecursive( Handle , BlockTime ) ;
			                                }
			inline BaseType_t  TakeRecursive ( TickType_t  BlockTime )
				{
					return xSemaphoreTakeRecursive( Handle , BlockTime ) ;
				}
			static inline BaseType_t  AltTake ( SemaphoreHandle_t Handle , TickType_t  BlockTime )
			                                {
			                                        return xSemaphoreAltTake( Handle , BlockTime ) ;
			                                }
			inline BaseType_t  AltTake ( TickType_t  BlockTime )
				{
					return xSemaphoreAltTake( Handle , BlockTime ) ;
				}
			static inline BaseType_t  Give (SemaphoreHandle_t Handle)
			                                {
			                                        return xSemaphoreGive( Handle ) ;
			                                }
			inline BaseType_t  Give ()
				{
					return xSemaphoreGive( Handle ) ;
				}
			static inline BaseType_t  GiveRecursive (SemaphoreHandle_t Handle)
			                                {
			                                        return xSemaphoreGiveRecursive( Handle ) ;
			                                }
			inline BaseType_t  GiveRecursive ()
				{
					return xSemaphoreGiveRecursive( Handle ) ;
				}
			static inline BaseType_t  AltGive (SemaphoreHandle_t Handle)
			                                {
			                                        return xSemaphoreAltGive( Handle ) ;
			                                }
			inline BaseType_t  AltGive ()
				{
					return xSemaphoreAltGive( Handle ) ;
				}
			static inline BaseType_t GiveFromISR ( SemaphoreHandle_t Handle , signed portBASE_TYPE*  HigherPriorityTaskWoken )
			                                {
			                                   return xSemaphoreGiveFromISR( Handle, HigherPriorityTaskWoken ) ;
			                                }
			inline BaseType_t GiveFromISR ( BaseType_t*  const HigherPriorityTaskWoken )
				{
				   return xSemaphoreGiveFromISR( Handle, HigherPriorityTaskWoken ) ;
				}
  };

class TTask
  {
     private:
       static void VMTable_Code( TTask* ptr ) { ptr->Code() ; }
     protected:
       TaskHandle_t Handle ;
     public:
       inline TTask() : Handle(NULL)
                {
                };

       inline virtual ~TTask()
                {
                  if (Handle)
                   {
                     vTaskDelete( Handle );
                     Handle = NULL ;
                   }
                };

       inline TaskHandle_t GetHandle() { return Handle ; }

       // ������� ����������� ������
       virtual void Code(void) = 0  ;
       inline void Create( const char* name , int stack_size , int priority )
               {
                 // ������� ������ ������� ������ � VMTable � ����������� � xTaskCreate
    	         xTaskCreate( (TaskFunction_t)VMTable_Code ,
    	                      ( const char * ) name ,
    	                      (short unsigned int)stack_size ,
    	                      (void *)this ,
    	                      (long unsigned int)priority ,
    	                      (TaskHandle_t *)&Handle ) ;
               }
       inline void Delete()
               {
                 vTaskDelete( Handle );
               }
       static inline void Delete( TaskHandle_t pxTask )
               {
                 vTaskDelete( pxTask );
               }
       static inline void Delay(TickType_t xTicksToDelay)
               {
                 vTaskDelay( xTicksToDelay );
               }
       static inline void DelayUntil( TickType_t *pxPreviousWakeTime, TickType_t xTimeIncrement )
               {
                 vTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement );
               }
       inline unsigned portBASE_TYPE GetPriority()
               {
                 return uxTaskPriorityGet( Handle );
               }
       static inline unsigned portBASE_TYPE GetPriority( TaskHandle_t pxTask )
               {
                 return uxTaskPriorityGet( pxTask );
               }
       inline void SetPriority( unsigned portBASE_TYPE uxNewPriority )
               {
                 vTaskPrioritySet( Handle , uxNewPriority ) ;
               }
       static inline void SetPriority( TaskHandle_t pxTask, unsigned portBASE_TYPE uxNewPriority )
               {
                 vTaskPrioritySet( pxTask, uxNewPriority ) ;
               }
       inline void Suspend()
               {
                 vTaskSuspend( Handle );
               }
       static inline void Suspend( TaskHandle_t pxTaskToSuspend )
               {
                 vTaskSuspend( pxTaskToSuspend );
               }
       inline void Resume()
               {
                 vTaskResume( Handle ) ;
               }
       static inline void Resume( TaskHandle_t pxTaskToResume )
               {
                 vTaskResume( pxTaskToResume );
               }
       static inline portBASE_TYPE ResumeFromISR( TaskHandle_t pxTaskToResume )
               {
                 return xTaskResumeFromISR( pxTaskToResume );
               }
       inline portBASE_TYPE ResumeFromISR()
               {
                 return xTaskResumeFromISR( Handle );
               }

       static inline unsigned portBASE_TYPE  GetStackHighWaterMark( TaskHandle_t xTask  )
       	   	   {
    	   	   	   return TTaskUtilities::GetStackHighWaterMark(xTask) ;
       	   	   }
       inline portBASE_TYPE GetStackHighWaterMark()
       	   	   {
    	   	   	   return TTaskUtilities::GetStackHighWaterMark( Handle ) ;
       	   	   }

  } ;

class TQueuedTask : public TTask
	{
		protected:
			typedef struct
				{
					unsigned portBASE_TYPE length;
					unsigned portBASE_TYPE item_size ;
				}TQueueParams ;
				inline unsigned portBASE_TYPE  Receive (void * Item , TickType_t timeout = portMAX_DELAY ) { return queue->Receive( Item , timeout ) ; }
				bool Init()
				{
					TQueueParams params;
					SetQueueParams(&params);
					queue = new TQueue ( params.length, params.item_size ) ;
					if ( !queue ) return false ;
					return queue->IsCreated() ;
				}

		private:
				TQueue* queue ;
				virtual void SetQueueParams(TQueueParams* params) = 0 ;
		public:
			inline TQueuedTask () : TTask() {}
			inline virtual ~TQueuedTask () { delete  queue ; }

			inline unsigned portBASE_TYPE SendToBack( const void* Item , TickType_t timeout = portMAX_DELAY ) { return queue->SendToBack(Item,timeout) ; }
			inline unsigned portBASE_TYPE SendToFront( const void* Item , TickType_t timeout = portMAX_DELAY ) { return queue->SendToFront(Item,timeout) ; }
			inline unsigned portBASE_TYPE SendToBackFromISR( const void* Item , portBASE_TYPE *pxHigherPriorityTaskWoken ) { return queue->SendToBackFromISR(Item,pxHigherPriorityTaskWoken) ; }
			inline unsigned portBASE_TYPE SendToFrontFromISR( const void* Item , portBASE_TYPE *pxHigherPriorityTaskWoken ) { return queue->SendToFrontFromISR(Item,pxHigherPriorityTaskWoken) ; }

			inline bool IsInitialized() { return queue->IsCreated(); } ;
	};

}

using namespace FreeRTOS ;

#endif /*__TASK_H__*/
