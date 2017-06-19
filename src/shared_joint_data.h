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


#ifndef SHARED_JOINT_DATA_H
#define SHARED_JOINT_DATA_H

enum { JOINT_POSITION, JOINT_FORCE, JOINT_STIFFNESS,
       JOINT_EMG_1, JOINT_EMG_2, JOINT_EMG_3, JOINT_EMG_4, JOINT_EMG_5, JOINT_FLOATS_NUMBER };

#define JOINT_DATA_BLOCK_SIZE JOINT_FLOATS_NUMBER * sizeof(float)

#endif // SHARED_JOINT_DATA_H
