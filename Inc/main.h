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

#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include "DisplayManager.h"
#include "Error.h"

constexpr uint8_t  MAX_COMMANDS_NBR = 10 ;
constexpr uint8_t  MAX_PARAMS_NBR = 5 ;

struct command
{
    std::string cmd; // command
    uint8_t nParams; // number of parameters
    std::string Params[MAX_PARAMS_NBR]; // optional parameters.
};


command argumentsList[MAX_COMMANDS_NBR];
const std::string supportedCommandList[MAX_COMMANDS_NBR]={"-d", "--download", "?", "-h", "--help", "-v", "-sn", "--serial", "-l", "--list"} ;

DisplayManager displayManager = DisplayManager::getInstance() ;
int extractProgramCommands (int numberCommands, char* commands[]);
bool compareStrings(const std::string& str1, const std::string& str2, bool caseInsensitive) ;
void showHelp();

#endif // MAIN_H
