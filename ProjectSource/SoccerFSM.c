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

/*----------------------------- Module Defines ----------------------------*/
// these times assume a 10.000mS/tick timing
#define TimePerRound_ms 11000
#define TotalRounds 2
#define InactiveTimeAllowed 20000
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
static void TurnOffJumboLEDs(void);
static void ReturnToWait4Coin(void);
/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static SoccerState_t CurrentState;
static SoccerState_t NextState; //define the next state in run function
static bool CoinLED;
static bool Player1LED;
static uint8_t Player1Score = 0;
static uint8_t Player2Score = 0;
static  CurrentRound = 1;
// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;
static uint8_t CoinCount = 0;//counting the coins inserted

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
  
  //define ball placement Sensor RB4
  
   TRISBbits.TRISB4= 1; //RB4 input, always digital

  //defining Shot button
  TRISBbits.TRISB5= 1; //RB5 input,, pin RB5 is always digital!
  
 
  
  //Defining OUTPUT Ports here
  //Solenoid on pin RB2
  ANSELBbits.ANSB2= 0; //digital
  TRISBbits.TRISB2= 0; //RB2 output
  LATBbits.LATB2 = 0; //initialize the solenoid as off
  //COIN LED OUTPUT PIN
  TRISBbits.TRISB10= 0; //RB10 output,, always digital
  //buzzer pin
  ANSELBbits.ANSB1= 0; //digital
  TRISBbits.TRISB1= 0; //RB1 output
  LATBbits.LATB1 = 0; //initialize buzzer as off
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
  ES_Event_t Event2Post;

  switch (ThisEvent.EventType)
  {
    case ES_TIMEOUT:{
      if (ThisEvent.EventParam == Solenoid_shutdown_timer)
      {
        LATBbits.LATB2 = 0; //turn off solenoid   
        DB_printf("solenoid turned off\n");

      }else if (ThisEvent.EventParam == SHOTCLOCK_TIMER);
      {
        Event2Post.EventType=ShotButtonDown;
        PostSoccerFSM(Event2Post);
        DB_printf("timeout! shots fired! \n");
      }
      
      
    }
    break;
  case CoinDetect:{
    DB_printf("coin detected\n");
  }
    break;
  case MissBeamBroken:{
    DB_printf("shot missed \n");
  }
    break;
    case GoalBeamBroken:{
    DB_printf("goal detected\n");
  }
  break;
      case BallPlaced:{
    DB_printf("ball placement detected \n");
  }
    break;
      case ShotButtonDown:{
    DB_printf("shot button down detected \n");
  }
    break;
  default:
    break;
  }
  
 //used in case we do not enter any switch cases
  NextState=CurrentState;
  
  switch (CurrentState)
  {
    case InitPState:        // If current state is initial Psedudo State
    {
      //turn of all Jumbo LEDs first
      TurnOffJumboLEDs();
      //turn on LED indicating coin slot
        LATBbits.LATB10 = 1;
        //move to next state
        NextState= Wait4Coin;
        DB_printf("InitPState reached\n");
        DB_printf("went to Wait for Coin State \n");
       
    }
    break;

    case Wait4Coin:        // If current state is state one
    {
        if (ThisEvent.EventType == CoinDetect){
          CoinCount++;
          Event2Post.EventType=LED_CoinCountMsg;
          PostLEDService(Event2Post);
        }
        if (CoinCount>=2){
            CoinCount = 0;
            //clear player scores and round numbers for the new game
            Player1Score = 0;
            Player2Score = 0;
            CurrentRound = 1;
            //LED lights up player 1 turn;
            LATBbits.LATB11 = 1; 
//            DB_printf("Coin Detected State"); 
            LATBbits.LATB10 = 0; //turns off coin indicator LED
            
            //allow goalie movement
            Event2Post.EventType = EnableServo;
            PostServoService(Event2Post);
            NextState= Wait4BallPlacement1; //setting next state as waiting for player 1 to shoot
            DB_printf("went to Wait4BallPlacement1 \n");
            //tell LED matrix to instruct the user to place the ball
            Event2Post.EventType = LED_Wait4Place_Msg;
            PostLEDService(Event2Post);
            //set inactivity clock
            ES_Timer_InitTimer(InactivityTimer,InactiveTimeAllowed);
            
        }
  
             
    }
    break;
    case Wait4BallPlacement1:{
      if (ThisEvent.EventType == BallPlaced)
      {
           NextState = Wait4Player1Shot;
            ES_Timer_InitTimer(SHOTCLOCK_TIMER, TimePerRound_ms);
            //reinitialize LED countdown
            //make LED display scores and count downs
            Event2Post.EventType = EnterScoreLED;
            Event2Post.EventParam = TimePerRound_ms/1000;
            PostLEDService(Event2Post);
            Event2Post.EventType = LED_RestartTimer4Player;
            PostLEDService(Event2Post);
            //turn off inactivity timer
        ES_Timer_StopTimer(InactivityTimer);
      }        //check for user inactivity
      else if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == InactivityTimer)
          {
            ReturnToWait4Coin();
            NextState = Wait4Coin;
          }
      
    }
    break;
    case Wait4BallPlacement2:{
      if (ThisEvent.EventType == BallPlaced)
      {
        NextState = Wait4Player2Shot;
        ES_Timer_InitTimer(SHOTCLOCK_TIMER, TimePerRound_ms);
        //make LED display scores and count downs
            Event2Post.EventType = EnterScoreLED;
            Event2Post.EventParam = TimePerRound_ms/1000;
            PostLEDService(Event2Post);
        //reinitialize LED countdown
        Event2Post.EventType = LED_RestartTimer4Player;
        PostLEDService(Event2Post);
        //turn off inactivity timer
        ES_Timer_StopTimer(InactivityTimer);
      }
      else if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == InactivityTimer)
          {
            ReturnToWait4Coin();
            NextState = Wait4Coin;
          }
    }
    break;
    case Wait4Player1Shot:        // If current state is state one
    {
        
//        if (ThisEvent.EventType == ShotButtonDown || ThisEvent.EventType == ES_TIMEOUT){
        if (ThisEvent.EventType == ShotButtonDown){ //player 1 shoots here
            ES_Timer_StopTimer(SHOTCLOCK_TIMER);
            //launch solenoid
            LATBbits.LATB2 = 1;
            ES_Timer_InitTimer(Solenoid_shutdown_timer,1000);//start timer for shutting down solenoid
            //turn off player 1 indicator light
            LATBbits.LATB11 = 0;
            NextState= Wait4Player1Ball;
            DB_printf("went to Wait for Player 1 Ball \n");
            //post ballshot event to LED FSM so that it shuts down the timer on display
            Event2Post.EventType = BallShot;
            PostLEDService(BallShot);
        }
    }
    break;
    
    case Wait4Player2Shot:        // If current state is state one
    {   
//      if (ThisEvent.EventType == ShotButtonDown || ThisEvent.EventType == ES_TIMEOUT){ //player 2 shoots
    if (ThisEvent.EventType == ShotButtonDown){ //player 2 shoots
            ES_Timer_StopTimer(SHOTCLOCK_TIMER);
            DB_printf("button detected in FSM \n");
            //launch solenoid
            LATBbits.LATB2 = 1;
            ES_Timer_InitTimer(Solenoid_shutdown_timer,1000);//start timer for shutting down solenoid
            //turn off player 2 indicator
            LATBbits.LATB12 = 0;
            NextState= Wait4Player2Ball;
            DB_printf("went to wait 4 player2 ball \n");
            //post ballshot event to LED FSM so that it shuts down the timer on display
            Event2Post.EventType = BallShot;
            PostLEDService(BallShot);
        }
    }
    break;
    
    case Wait4Player1Ball:        // waiting for player 1 to either score or miss
    
        if (ThisEvent.EventType == GoalBeamBroken) { // GOALLLL
            
            //increase goal counter 
      
            Player1Score += 1;
            //update player1 score on LED
            Event2Post.EventType = LED_P1ScoreUpdate;
            Event2Post.EventParam = Player1Score;
            PostLEDService(Event2Post);
            // Ball returned for Player 1, transition to next state
             //LED lights up player 2 turn;
            LATBbits.LATB12 = 1; 
            NextState = Wait4BallPlacement2;
            DB_printf("goal!! went to Wait4BallPlacement2 \n");
            //turn on buzzer
            Event2Post.EventType = TurnBuzzerOn;
            PostBuzzerService(Event2Post);
            //set inactivity clock
            ES_Timer_InitTimer(InactivityTimer,InactiveTimeAllowed);
        }
    
        else if (ThisEvent.EventType == MissBeamBroken){
          
            // Ball returned for Player 1, transition to next state
             //LED lights up player 2 turn;
            LATBbits.LATB12 = 1; 
            NextState = Wait4BallPlacement2;
            DB_printf("missed! went to Wait4BallPlacement2 \n");
            //set inactivity clock
            ES_Timer_InitTimer(InactivityTimer,InactiveTimeAllowed);
            
        }
    

    break;
    
    case Wait4Player2Ball:        // If current state is state one

      //do this
    if (ThisEvent.EventType == GoalBeamBroken) { // GOALLLL
            
            //increase goal counter 
          
            Player2Score += 1;
            //update player 2 score on LED Matrix
            Event2Post.EventType = LED_P2ScoreUpdate;
            Event2Post.EventParam = Player2Score;
            PostLEDService(Event2Post);
            // Ball returned for Player 1, transition to next state 
  
            NextState = CheckingEndGame;
            PostSoccerFSM(Event2Post);
            DB_printf("goal!! went to Checking Endgame \n");
            //turn on buzzer
            Event2Post.EventType = TurnBuzzerOn;
            PostBuzzerService(Event2Post);
          
        }
    
        else if (ThisEvent.EventType == MissBeamBroken){
           
            // Ball returned for Player 1, transition to next state
              
            NextState = CheckingEndGame;
            PostSoccerFSM(Event2Post);
            DB_printf("missed!! went to wait Checking Endgame\n");
        }
    
    break;
    
    case CheckingEndGame:        // If current state is state one
    DB_printf("Checking End Game \n");
        if (CurrentRound < TotalRounds) {
            // If rounds are less than 2, continue the game
            CurrentRound++;
            NextState = Wait4BallPlacement1;  // Next round
            LATBbits.LATB11 = 1; //LED lights up player 1 turn;
           
            DB_printf("went to Wait4BallPlacement1 \n");
            //set inactivity clock
            ES_Timer_InitTimer(InactivityTimer,InactiveTimeAllowed);
        } 
        else {
          DB_printf("End of game \n");
            // End game 
            NextState = Wait4Coin;  
            ReturnToWait4Coin();
            //display winner
            Event2Post.EventType = DisplayWinner;
            if (Player1Score > Player2Score)
            {
              Event2Post.EventParam = 1;
            }else if (Player1Score < Player2Score)
            {
              Event2Post.EventParam = 2;
            }else // it's a tie
            {
              Event2Post.EventParam = 0;
            }
            PostLEDService(Event2Post);
            
        }    
    
    break;
       default:
    break;
  } 
  CurrentState=NextState; // set the current state to be next state 

  return ReturnEvent;
}

/*************************
 * private functions
*/
static void TurnOffJumboLEDs(void){
  LATBbits.LATB10 = 0; 
  LATBbits.LATB11 = 0; 
  LATBbits.LATB12 = 0; 
  
}
//this function resets jumbo LEDs, disables servo, and make LED Matrix display display user inactivity
static void ReturnToWait4Coin(void){
  ES_Event_t Event2Post;
  //turn of all Jumbo LEDs first and turn on wait4 coin led
  TurnOffJumboLEDs();
  LATBbits.LATB10 = 1;//please insert coin LED on
  //disable servo
  Event2Post.EventType = DisableServo;
  PostServoService(Event2Post);
  Event2Post.EventType = UserInactivity;
  PostLEDService(Event2Post);
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
SoccerState_t QuerySoccerFSM(void)
{
  return CurrentState;
  
}

/***************************************************************************
 private functions
 ***************************************************************************/
