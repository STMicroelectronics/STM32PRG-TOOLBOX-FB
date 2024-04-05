/*
 * Copyright 2024 STMicroelectronics
 *
 * Based on fastboot v34.0.5
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <stdarg.h>
#include <iostream>

/* Colors macros for console*/
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGREY 7
#define DARKGREY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15
#define BLINK 128

enum messageType
{
    MSG_NORMAL,
    MSG_GREEN,
    MSG_WARNING,
    MSG_ERROR,
};

class DisplayManager
{
public:
    static DisplayManager& getInstance() ;
    void print(messageType messageType, const wchar_t* message, ...);

private:
    DisplayManager();
    void displayMessage(messageType type, const wchar_t* str) ;
};

#endif // DISPLAYMANAGER_H
