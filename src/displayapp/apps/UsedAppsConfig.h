/*
 * This file is part of the InfinitimeNext distribution (https://github.com/stephen-polgar/InfiniTimeNext).
 * Copyright (c) 2024 Istvan Polgar.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/*
Comment or uncomment definitions for change built in apps.
InfiniSimNext simulator uses this settings too.
*/

#define UseCalendar
#define UseCalculator
#define UseMotion
#define UsePaddle
// #define UseDice
// #define UseTwos
// #define UseMetronome
// #define UseInfiniPaint
// #define UseWatchFaceTerminal

#define UseWatchFaceInfineat
#define UseWatchFacePineTimeStyle
// #define UseFileManager  // not ready

/*
Set the maximum number of shown notifications

Value between 5 - 8 depends on the size of selected apps,
else 'region RAM overflowed with stack' error on building.
*/
#define MaxNotifications 5