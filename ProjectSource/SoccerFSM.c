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
#include "SoccerFSM.h"

#include "ES_Port.h"
#include "terminal.h"
#include "dbprintf.h"
#include "LEDFSM.h"
/*----------------------------- Module Defines ----------------------------*/
// these times assume a 10.000mS/tick timing
#define FIFTEEN_SEC 15000 //this is similar to the test harness initializing of timer


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static SoccerState_t CurrentState;
static SoccerState_t NextState; //define the next state in run function
static bool SolenoidCharge;
static bool CoinLED;
static bool Player1LED;
static uint8_t Player1Rounds = 0;
static uint8_t Player2Rounds = 0;
static uint8_t CurrentRound = 1;
static uint8_t Player1Score = 0;
static uint8_t Player2Score = 0;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;

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
bool InitSoccerFSM(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  // put us into the Initial PseudoState
  CurrentState = InitPState;
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  
  //defining INPUT Ports here
  //Coin IR Sensor Input to MicroController at port RB8
  TRISBbits.TRISB8= 1; // RB8 input,, pin RB8 is always digital!
 
  //defining IR Goal Sensor input RB9
  TRISBbits.TRISB9= 1; //RB9 input, always digital
  
  //defining IR Miss Sensor RB13
   ANSELBbits.ANSB13= 0; //digital
   TRISBbits.TRISB13= 1; //RB13 input
  
  //defining Shot button
  TRISBbits.TRISB5= 1; //RB5 input,, pin RB5 is always digital!
  
  //defining Potentiometer Input
//  ANSELBbits.ANSB15= 1; //analog
//  TRISBbits.TRISB15= 1; //RB15input
 
  
  //Defining OUTPUT Ports here
  //Solenoid on pin RB2
  ANSELBbits.ANSB2= 0; //digital
  TRISBbits.TRISB2= 0; //RB2 output
  
  //COIN LED OUTPUT PIN
  TRISBbits.TRISB10= 0; //RB10 output,, always digital
  
  //PLAYER 1 SHOOT INDICATOR OUTPUT LED
  TRISBbits.TRISB11= 0; //RB11 output,, always digital
  
  //PLAYER 2 SHOOT INDICATOR OUTPUT LED
  ANSELBbits.ANSB12= 0; //digital
  TRISBbits.TRISB12= 0; //RB12 output
  DB_printf("FSM initialized, all pins set \n");
  // LED MATRIX OUTPUT STUFF (SPI) COULD WE USE THIS WITH THE SPI HAL?? PROBABLY
  
  
  //MOTORSERVO GOES HERE *****************************************************
//    ANSELBbits.ANSB3= 0; //digital
//    TRISBbits.TRISB3= 0; //RB2 output
  
  
  //
  
  
  //put us into initial state
  CurrentState=InitPState;
  
  
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
bool PostSoccerFSM(ES_Event_t ThisEvent)
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
ES_Event_t RunSoccerFSM(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  
 //used in case we do not enter any switch cases
  NextState=CurrentState;
  
  switch (CurrentState)
  {
    case InitPState:        // If current state is initial Psedudo State
    {
      //turn on LED indicating coin slot
        CoinLED= PORTBbits.RB10;
        CoinLED= 1;

        
        //move to next state
        NextState= Wait4Coin;
        DB_printf("Initialized State soccer/n");
        ReturnEvent.EventType = ES_NO_EVENT;  // Clear return event   
    }
    break;

    case Wait4Coin:        // If current state is state one
    {
      //do this
        DB_printf("Waitf for Coin State");
        if (ThisEvent.EventType == CoinDetect){
            //LED lights up player 1 turn;
            Player1LED=PORTBbits.RB11;
            Player1LED= 1;
            DB_printf("Coin Detected State"); 

            
            //charge up solenoid
            SolenoidCharge= PORTBbits.RB2;
            SolenoidCharge= 0; //by sending current to solenoid, we charge it
            
            //allow goalie movement
            //***** DO THIS NEED SERVO SERVICE HERE************************************************
            
            //Set and Start Timer by initializing:
            ES_Timer_InitTimer(SHOTCLOCK_TIMER, FIFTEEN_SEC);
        }
  
              NextState= Wait4Player1Shot; //setting next state as waiting for player 1 to shoot
    }
    break;
    
    case Wait4Player1Shot:        // If current state is state one
    {
        if (ThisEvent.EventType == ShotButtonDown || ThisEvent.EventType == ES_TIMEOUT){
            
            //launch solenoid
            SolenoidCharge= PORTBbits.RB2;
            SolenoidCharge= 1; //by sending current to solenoid, we charge it
            NextState= Wait4Player1Ball;
            PostSoccerFSM(ThisEvent);  // Trigger transition
            ReturnEvent.EventType = ES_NO_EVENT;  // Clear return event            
        }
    }
    break;
    
    case Wait4Player2Shot:        // If current state is state one
    {
      if (ThisEvent.EventType == ShotButtonDown || ThisEvent.EventType == ES_TIMEOUT){
            
            //launch solenoid
            SolenoidCharge= PORTBbits.RB2;
            SolenoidCharge= 1; //by sending current to solenoid, we charge it
            NextState= Wait4Player2Ball;
            PostSoccerFSM(ThisEvent);  // Trigger transition
            ReturnEvent.EventType = ES_NO_EVENT;  // Clear return event            
        }
    }
    break;
    
    case Wait4Player1Ball:        // If current state is state one
    
        if (CheckBallReturnPlayer1()) {
            // Ball returned for Player 1, transition to next state
            SolenoidCharge= PORTBbits.RB2;
            SolenoidCharge= 0;            
            NextState = Wait4Player1Shot;
            PostSoccerFSM(ThisEvent);  // Trigger transition
            ReturnEvent.EventType = ES_NO_EVENT;  // Clear return event
        }
      //if goal beam broken
        //increment player 1 score (need to init this counter somewhere)
        //charge up solenoid
        // light up player2 led
        //reinitialize timer
        
      //if miss beam broken
            //charge up solenoid
        // light up player2 led
        //reinitialize timer
    
    break;
    
    case Wait4Player2Ball:        // If current state is state one
        if (CheckBallReturnPlayer1()) {
            // Ball returned for Player 1, transition to next state
            SolenoidCharge= PORTBbits.RB2;
            SolenoidCharge= 0;
            NextState = Wait4Player1Shot;
            PostSoccerFSM(ThisEvent);  // Trigger transition
            ReturnEvent.EventType = ES_NO_EVENT;  // Clear return event
        }
      //do this
    
    break;
    
    case CheckingEndGame:        // If current state is state one
        if (CurrentRound < 2) {
            // If rounds are less than 2, continue the game
            CurrentRound++;
            NextState = Wait4Player1Shot;  // Next round
            PostSoccerFSM(ThisEvent);  // Trigger transition
            ReturnEvent.EventType = ES_NO_EVENT;  // Clear return event
        } else {
            // End game, display winner
//            DisplayWinner();
            NextState = EndGame;  // Transition to End Game
            PostSoccerFSM(ThisEvent);  // Trigger transition
            ReturnEvent.EventType = ES_NO_EVENT;  // Clear return event
        }    
      //do this
    
    break;
    
    
    default:
      ;
  } 
  CurrentState=NextState; // set the current state to be next state 
  // end switch on Current State
  return ReturnEvent;
}



// Helper Functinos

bool CheckMissBeamSensor(void) {
    if (PORTBbits.RB13 == 0) {
        // Sensor is blocked, return true
        return true;
    } else {
        // Sensor is not blocked, return false
        return false;
    }
}

// Helper function to check Player 1's ball return
bool CheckBallReturnPlayer1() {
    if (CheckMissBeamSensor()) {
        Player1Rounds++;
        Player1Score += 1;  // Increase score for Player 1
//        UpdateDisplayForPlayer1();
        return true;
    }
    return false;
}

// Helper function to check Player 2's ball return
bool CheckBallReturnPlayer2() {
    if (CheckMissBeamSensor()) {
        Player2Rounds++;
        Player2Score += 1;  // Increase score for Player 2
//        UpdateDisplayForPlayer2();
        return true;
    }
    return false;
}

// Function to update the display for Player 1
//void UpdateDisplayForPlayer1() {
//    UpdateDotMatrixDisplay(Player1Score, Player2Score);
//    DisplayPlayerTurn(1); // Change this to the LED
//}

// Function to update the display for Player 2
//void UpdateDisplayForPlayer2() {
//    UpdateDotMatrixDisplay(Player1Score, Player2Score);
//    DisplayPlayerTurn(2); // Change this to the LED
//}

// Function to update the score display
//void UpdateDotMatrixDisplay(uint8_t Player1Score, uint8_t Player2Score) {
    // Display the current scores on the dot matrix (you can call your display functions here)
    // Example: DM_DisplayScore(Player1Score, Player2Score);
//    DM_DisplayScore(Player1Score, Player2Score);  // This function would handle the display update
//}

// Display winner at the end of the game
//void DisplayWinner() {
//    if (Player1Score > Player2Score) {
//        // Display Player 1 as winner
//        DM_DisplayWinner(1);  // Example function to show Player 1 as winner
//    } else if (Player2Score > Player1Score) {
//        // Display Player 2 as winner
//        DM_DisplayWinner(2);  // Example function to show Player 2 as winner
//    } else {
//        // Display draw if scores are equal
//        DM_DisplayDraw();  // Example function to display a draw
//    }
//}
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
SoccerState_t QuerySoccerFSM(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
