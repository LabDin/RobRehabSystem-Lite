////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Copyright (c) 2016-2019 Leonardo Consoni <consoni_2519@hotmail.com>       //
//                                                                            //
//  This file is part of RobotSystem-Lite.                                    //
//                                                                            //
//  RobotSystem-Lite is free software: you can redistribute it and/or modify  //
//  it under the terms of the GNU Lesser General Public License as published  //
//  by the Free Software Foundation, either version 3 of the License, or      //
//  (at your option) any later version.                                       //
//                                                                            //
//  RobotSystem-Lite is distributed in the hope that it will be useful,       //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of            //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              //
//  GNU Lesser General Public License for more details.                       //
//                                                                            //
//  You should have received a copy of the GNU Lesser General Public License  //
//  along with RobotSystem-Lite. If not, see <http://www.gnu.org/licenses/>.  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "sensor.h"

#include "input.h"

#include "tinyexpr/tinyexpr.h"

#include "data_io/interface/data_io.h" 
#include "debug/data_logging.h"
#include "timing/timing.h"

#include "config_keys.h"

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const char* INPUT_VARIABLE_NAMES[] = { "in0", "in1", "in2", "in3", "in4", "in5" };

struct _SensorData
{
  Input* inputsList;
  size_t inputsNumber;
  double* inputValuesList;
  te_variable* inputVariables;
  te_expr* transformFunction;
  Log log;
};

Sensor Sensor_Init( const char* configName )
{
  char filePath[ DATA_IO_MAX_PATH_LENGTH ];
  DEBUG_PRINT( "trying to create sensor %s", configName );
  sprintf( filePath, KEY_CONFIG "/" KEY_SENSOR "/%s", configName );
  DataHandle configuration = DataIO_LoadStorageData( filePath );
  if( configuration == NULL ) return NULL;
  
  DEBUG_PRINT( "sensor configuration found on data handle %p", configuration );
  
  Sensor newSensor = (Sensor) malloc( sizeof(SensorData) );
  memset( newSensor, 0, sizeof(SensorData) );  
  
  bool loadSuccess = true;
  
  newSensor->inputsNumber = DataIO_GetListSize( configuration, KEY_INPUT "s" );
  newSensor->inputsList = (Input*) calloc( newSensor->inputsNumber, sizeof(Input) );
  newSensor->inputValuesList = (double*) calloc( newSensor->inputsNumber, sizeof(double) );
  newSensor->inputVariables = (te_variable*) calloc( newSensor->inputsNumber, sizeof(te_variable) );
  for( size_t inputIndex = 0; inputIndex < newSensor->inputsNumber; inputIndex++ )
  {
    DEBUG_PRINT( "%s", DataIO_GetDataString( DataIO_GetSubData( configuration, KEY_INPUT "s.%lu", inputIndex ) ) );
    newSensor->inputsList[ inputIndex ] = Input_Init( DataIO_GetSubData( configuration, KEY_INPUT "s.%lu", inputIndex ) );
    if( newSensor->inputsList[ inputIndex ] == NULL ) loadSuccess = false;
   
    newSensor->inputVariables[ inputIndex ].name = INPUT_VARIABLE_NAMES[ inputIndex ];
    newSensor->inputVariables[ inputIndex ].address = &(newSensor->inputValuesList[ inputIndex ]);
  }
  
  int expressionError;
  const char* transformExpression = DataIO_GetStringValue( configuration, INPUT_VARIABLE_NAMES[ 0 ], KEY_OUTPUT );
  newSensor->transformFunction = te_compile( transformExpression, newSensor->inputVariables, newSensor->inputsNumber, &expressionError );
  if( expressionError > 0 ) loadSuccess = false;
      
  if( DataIO_HasKey( configuration, KEY_LOG ) )
    newSensor->log = Log_Init( DataIO_GetBooleanValue( configuration, false, KEY_LOG "." KEY_FILE ) ? configName : "", 
                               (size_t) DataIO_GetNumericValue( configuration, 3, KEY_LOG "." KEY_PRECISION ) );
  
  DataIO_UnloadData( configuration );
  
  if( !loadSuccess )
  {
    Sensor_End( newSensor );
    return NULL;
  }    
  
  return newSensor;
}

void Sensor_End( Sensor sensor )
{
  if( sensor == NULL ) return;
  
  for( size_t inputIndex = 0; inputIndex < sensor->inputsNumber; inputIndex++ )
    Input_End( sensor->inputsList[ inputIndex ] );
  free( sensor->inputsList );
  free( sensor->inputValuesList );
  free( sensor->inputVariables );
  
  te_free( sensor->transformFunction );
  
  Log_End( sensor->log );
  
  free( sensor );
}

double Sensor_Update( Sensor sensor )
{
  if( sensor == NULL ) return 0.0;
  
  for( size_t inputIndex = 0; inputIndex < sensor->inputsNumber; inputIndex++ )
    sensor->inputValuesList[ inputIndex ] = Input_Update( sensor->inputsList[ inputIndex ] );
    
  double sensorOutput = te_eval( sensor->transformFunction );
  
  Log_EnterNewLine( sensor->log, Time_GetExecSeconds() );
  Log_RegisterList( sensor->log, sensor->inputsNumber, sensor->inputValuesList );
  Log_RegisterValues( sensor->log, 1, sensorOutput ); 
  
  return sensorOutput;
}
  
bool Sensor_HasError( Sensor sensor )
{
  if( sensor == NULL ) return false;
  
  for( size_t inputIndex = 0; inputIndex < sensor->inputsNumber; inputIndex++ )
    if( Input_HasError( sensor->inputsList[ inputIndex ] ) ) return true;
  
  return false;
}

void Sensor_Reset( Sensor sensor )
{
  if( sensor == NULL ) return;
  
  for( size_t inputIndex = 0; inputIndex < sensor->inputsNumber; inputIndex++ )
    Input_Reset( sensor->inputsList[ inputIndex ] );
}

void Sensor_SetState( Sensor sensor, enum SensorState newState )
{
  if( sensor == NULL ) return;
  
  enum SigProcState newProcessingState = SIG_PROC_STATE_MEASUREMENT;
  if( newState == SENSOR_STATE_OFFSET ) newProcessingState = SIG_PROC_STATE_OFFSET;
  else if( newState == SENSOR_STATE_CALIBRATION ) newProcessingState = SIG_PROC_STATE_CALIBRATION;
  
  for( size_t inputIndex = 0; inputIndex < sensor->inputsNumber; inputIndex++ )
    Input_SetState( sensor->inputsList[ inputIndex ], newProcessingState );
}
