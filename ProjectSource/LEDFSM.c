/****************************************************************************
 Module
   TemplateFSM.c

 Revision
   1.0.1

 Description
   This is a template file for implementing flat state machines under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/15/12 11:12 jec      revisions for Gen2 framework
 11/07/11 11:26 jec      made the queue static
 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "LEDFSM.h"
#include "DM_Display.h"
#include "PIC32_SPI_HAL.h"
#include "FontStuff.h"
#include "ES_DeferRecall.h"
#include "dbprintf.h"



/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static TemplateState_t CurrentState;
static TemplateState_t PreviousState;

// ADDED VARIABLES FOR SOCCER PROJECT
static uint8_t Player1Score = 0;
static uint8_t Player2Score = 0;
static uint8_t TimerValue = 0;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;
static ES_Event_t DeferralQueue[3 + 1];

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateFSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 18:55
****************************************************************************/
bool InitLEDFSM(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  // put us into the Initial PseudoState
//  CurrentState = WaitForChar;
  // NEW ADDITIONS FOR SOCCER PROJ BELOW
    CurrentState = InitLEDState;
    TimerValue = 15;  // Start timer at 15??
    Player1Score = 0;
    Player2Score = 0;
  
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  

  //need to initialize the dm_display stuff
   int num_modules= 4;
    //from the prelab 
    SPISetup_DisableSPI(SPI_SPI1);
    SPISetup_BasicConfig(SPI_SPI1);
    SPISetup_SetLeader(SPI_SPI1, SPI_SMP_MID); //set spi to master mode
    SPISetup_SetBitTime(SPI_SPI1, 100000); //start at 10000 and if doesn't work bump it to a Million
    SPISetup_MapSSOutput(SPI_SPI1, SPI_RPA0); 
    SPISetup_MapSDOutput(SPI_SPI1, SPI_RPA1);
    SPISetup_SetClockIdleState(SPI_SPI1, SPI_CLK_LO); //set clock idle state and active edge via spec sheet
    SPISetup_SetActiveEdge(SPI_SPI1, SPI_FIRST_EDGE);
    SPISetup_SetXferWidth(SPI_SPI1, SPI_16BIT); //sending 16 bit characters bc hexidecimal?
    SPI1BUF; //clear buffer
    SPISetEnhancedBuffer(SPI_SPI1, true);
    SPISetup_EnableSPI(SPI_SPI1);
    IFS0CLR =_IFS0_INT4IF_MASK; //tbh idk what this is
    
    while ( false == DM_TakeInitDisplayStep() ){}
  DB_printf("\rES_INIT received in Service %d\r\n", MyPriority);
    //DEFFERAL QUEUE INIT
  ES_InitDeferralQueueWith(DeferralQueue, ARRAY_SIZE(DeferralQueue));
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
     PostTemplateFSM

 Parameters
     EF_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostLEDFSM(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateFSM

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/

// ****************************COMMENTED OUT OLD LED FSM HERE
//ES_Event_t RunLEDFSM(ES_Event_t ThisEvent)
//{
//  ES_Event_t ReturnEvent;
//  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
//
//  switch (CurrentState)
//  {
//    case WaitForChar:        // If current state is initial Psedudo State
//    {
//      if (ThisEvent.EventType == ES_NEW_KEY)    // IF A NEW KEY IS PUSHED
//      {
//        // this is where you would put any actions associated with the
//        // transition from the initial pseudo-state into the actual
//        // initial state waiting for a character
//        
//          
//          //triggers scrolling by 4
//          DM_ScrollDisplayBuffer(4);
//          DM_AddChar2DisplayBuffer((char)ThisEvent.EventParam);
// 
//          //making a local event type
//            ES_Event_t LocalEvent;     
//            LocalEvent.EventType = ES_WAITBUFFER;
//            PostLEDFSM(LocalEvent);
//            CurrentState= WaitForBuffer;
//            
//      }
//      else if (ThisEvent.EventType == ES_WAITCHAR){
//          DM_ScrollDisplayBuffer(4);
//          DM_AddChar2DisplayBuffer(ThisEvent.EventParam);
//          
// 
//          //making a local event type
//            ES_Event_t LocalEvent;     
//            LocalEvent.EventType = ES_WAITBUFFER;
//            PostLEDFSM(LocalEvent);
//            CurrentState= WaitForBuffer;
//      }
//    }
//    break;
//
//    case WaitForBuffer:        // If current state is WaitForBuffer
//        // do nothing until trigger occurs
//        //when trigger happens: scrolling occurs, write to display, and updates display
//    {
//        if (ThisEvent.EventType == ES_NEW_KEY){
//            ES_DeferEvent(DeferralQueue, ThisEvent);
//        }
//        
//       if (ThisEvent.EventType == ES_WAITBUFFER) {
//          
//           if (false == DM_TakeDisplayUpdateStep()){
//              //making a local event type
//                ES_Event_t LocalEvent;     
//                LocalEvent.EventType = ES_WAITBUFFER;
//              // sending to next state:
//                //CurrentState = WaitForBuffer;
//                PostLEDFSM(LocalEvent);
//                }
//            else{
//            CurrentState = WaitForChar;
//            ES_RecallEvents(MyPriority, DeferralQueue);
////            LocalEvent.EventType = ES_WAITBUFFER;
//            }
//       }
//         
//    }
//    break;
//     default:
//    break;                                  // end switch on Current State
//  return ReturnEvent;
//}
//}

// NEW RUN LED FSM BELOW (STATES ADDED TO HEADER )
ES_Event_t RunLEDFSM(ES_Event_t ThisEvent) {
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT;  // Default return event

    switch (CurrentState) {
        case InitLEDState:
            // Initialize the display (clear it) and set up timer/score display
            // Clear the display at the start
            // Initialize Player Scores and Timer
            DM_DisplayScore(Player1Score, 2);  // Display Player 1 score on module 2
            DM_DisplayScore(Player2Score, 3);  // Display Player 2 score on module 3
            DM_DisplayTimer(TimerValue);  // Display timer on module 1
            CurrentState = DisplayScoreState;  // Transition to Display Score State
            break;

        case DisplayScoreState:
            // This state will be responsible for updating scores and timer as the game progresses
            // If scores are updated, refresh the display
            DM_DisplayScore(Player1Score, 2);  // Update Player 1 score on module 2
            DM_DisplayScore(Player2Score, 3);  // Update Player 2 score on module 3
            DM_DisplayTimer(TimerValue);  // Update Timer display on module 1
            break;

        case UpdateTimerState:
            // This state is responsible for updating the timer value
            TimerValue++;  // Increment timer (this logic could be tied to a game clock)
            DM_DisplayTimer(TimerValue);  // Update the timer display
            break;

        default:
            break;
    }

    return ReturnEvent;
}

// Function to update the player scores (called when score changes)
void UpdatePlayerScore(uint8_t Player, uint8_t Score) {
    if (Player == 1) {
        Player1Score = Score;
    } else if (Player == 2) {
        Player2Score = Score;
    }
    // Transition to update the display with the new scores
    CurrentState = DisplayScoreState;  // Transition to Display Score State
}

// Function to update the timer value
void UpdateTimer(uint8_t NewTime) {
    TimerValue = NewTime;
    // Transition to update the display with the new timer
    CurrentState = UpdateTimerState;  // Transition to Update Timer State
}

/****************************************************************************
 Function
     QueryTemplateSM

 Parameters
     None

 Returns
     TemplateState_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:21
****************************************************************************/
TemplateState_t QueryLEDFSM(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/

