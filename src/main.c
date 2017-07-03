////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Copyright (c) 2016-2017 Leonardo Consoni <consoni_2519@hotmail.com>       //
//                                                                            //
//  This file is part of RobRehabSystem.                                      //
//                                                                            //
//  RobRehabSystem is free software: you can redistribute it and/or modify    //
//  it under the terms of the GNU Lesser General Public License as published  //
//  by the Free Software Foundation, either version 3 of the License, or      //
//  (at your option) any later version.                                       //
//                                                                            //
//  RobRehabSystem is distributed in the hope that it will be useful,         //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of            //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              //
//  GNU Lesser General Public License for more details.                       //
//                                                                            //
//  You should have received a copy of the GNU Lesser General Public License  //
//  along with RobRehabSystem. If not, see <http://www.gnu.org/licenses/>.    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifdef __unix__
  #define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

//#include "utils/debug/async_debug.h"

#include "system.h"


static volatile bool isRunning = true;

void HandleExit( int signal )
{
  //DEBUG_PRINT( "received exit signal: %d", signal );
  isRunning = false;
}

void HandleError( int signal )
{
  //DEBUG_PRINT( "received error signal: %d", signal );
  exit( EXIT_FAILURE );
}

/* Program entry-point */
int main( const int argc, const char* argv[] )
{
  const struct timespec UPDATE_TIMESPEC = { .tv_nsec = 1000000 * UPDATE_INTERVAL_MS };
  
  time_t rawTime;
  time( &rawTime );
  //DEBUG_PRINT( "starting control program at time: %s", ctime( &rawTime ) );
  
  atexit( System_End );
  
  signal( SIGINT, HandleExit );   // Handle Keyboard Interruption (Crtl^C)
  signal( SIGSEGV, HandleError ); // Try to prevent not running termination calls on a Segmentation Fault event
  
  if( System_Init( argc, argv ) )
  {
    while( isRunning ) // Check for program termination conditions
    {
      System_Update();
      
      nanosleep( &UPDATE_TIMESPEC, NULL ); // Sleep to give the desired loop rate.
    }
  }
  
  time( &rawTime );
  //DEBUG_PRINT( "ending control program at time: %s", ctime( &rawTime ) );
  
  exit( EXIT_SUCCESS );
}