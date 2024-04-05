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

#ifndef FASTBOOT_H
#define FASTBOOT_H

#include <iostream>
#include "Inc/DisplayManager.h"
#include "Inc/Error.h"

class Fastboot
{
public:
    Fastboot();
    int flashPartition(const std::string partitionName, const std::string partitionFirmwarePath) ;
    int oemFormatMemory() ;
    bool isUbootFastbootRunning() ;
    std::string toolboxFolder = "" ;

private:
    DisplayManager displayManager = DisplayManager::getInstance() ;
    std::string getFastbootProgramPath() ;
};

#endif // FASTBOOT_H
