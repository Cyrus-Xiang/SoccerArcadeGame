/****************************************************************************
 Module
   ServoService.c

 Revision
   1.0.1

 Description
   This is a Servo file for implementing a simple service under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/16/12 09:58 jec      began conversion from ServoFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ServoService.h"
#include "dbprintf.h"
#include "PIC32_AD_Lib.h"
#include "PWM_PIC32.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
// #define AD_Channel (1 << 0)
// #define Num_AD_Channels  1
#define PWM_period_us 20000
#define low_PW_us 500
#define upper_PW_us 2500
#define ticks_per_us 2.5
// static uint32_t LastAD_Val [Num_AD_Channels];
static uint8_t MyPriority;
int32_t diff_AD;
static uint8_t DutyCycle;
static uint16_t PulseWidth;
static uint16_t Pot_reading;
static uint16_t PW_mid_us;
static uint16_t PW_range_us;
static bool moveAllowed = false;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitServoService

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

 Author
     J. Edward Carryer, 01/16/12, 10:00
****************************************************************************/
bool InitServoService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/

  // initialze pins for for potentiometer
  ADC_ConfigAutoScan(BIT9HI); // bit5 corresponds to AN5/RB3
  TRISBbits.TRISB15 = 1;      // configure the pin as input
  ANSELBbits.ANSB15 = 1;      // Configure RB15 as analog IO


  // initialize PWM for motor control
  PWMSetup_BasicConfig(1);
  PWMSetup_SetPeriodOnTimer(PWM_period_us * ticks_per_us, _Timer2_);
  DB_printf("PWM period is set to %u ticks \n", PWM_period_us * ticks_per_us);
  PWMSetup_AssignChannelToTimer(1, _Timer2_);
  PWMSetup_MapChannelToOutputPin(1, PWM_RPB3);
  PW_range_us = upper_PW_us - low_PW_us;
  PW_mid_us = (uint16_t)PW_range_us / 2 + low_PW_us;
  PulseWidth = PW_mid_us * ticks_per_us;

  PWMOperate_SetPulseWidthOnChannel(PulseWidth, 1);

  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  DB_printf("servo service initialized \n");
  if (ES_PostToService(MyPriority, ThisEvent) == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/****************************************************************************
 Function
     PostServoService

 Parameters
     EF_Event_t ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostServoService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunServoService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event_t RunServoService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************
   in here you write your service code
   *******************************************/
  switch (ThisEvent.EventType)
  {
  case EnableServo:
  {
    moveAllowed = true;
  }
  break;
  case DisableServo:
  {
    moveAllowed = false;
    //return to the neutral position
    PulseWidth = PW_mid_us * ticks_per_us;
    PWMOperate_SetPulseWidthOnChannel(PulseWidth, 1);
  }
  break;
  case Pot_Val_Update:
  {
    // DB_printf("Pot_Val_update event received in servo service n");
    // DB_printf(" %u \n",ThisEvent.EventParam);
    Pot_reading = ThisEvent.EventParam;
    PulseWidth = (uint16_t)((double)(Pot_reading / 1024.0) * PW_range_us + low_PW_us) * ticks_per_us;
    if (moveAllowed)
    {
      PWMOperate_SetPulseWidthOnChannel(PulseWidth, 1);
    }
    // DB_printf("new pulse width is %u \n", PulseWidth);
  }
  break;
  default:
    break;
  }
  return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
