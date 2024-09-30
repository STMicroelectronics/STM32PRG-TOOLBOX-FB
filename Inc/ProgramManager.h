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

#ifndef PROGRAMMANAGER_H
#define PROGRAMMANAGER_H

#include <iostream>
#include "FileManager.h"
#include "DisplayManager.h"
#include "Fastboot.h"
#include "Error.h"

class ProgramManager
{
public:
    ProgramManager(const std::string toolboxFolder, const std::string fastbootSerialNumber = "");
    ~ProgramManager();
    int startFlashingService(const std::string inputTsvPath) ;

private:
    DisplayManager displayManager = DisplayManager::getInstance() ;
    FileManager fileManager  = FileManager::getInstance() ;
    Fastboot *fastbootInterface;
    fileTSV *parsedTsvFile ;
};

#endif // PROGRAMMANAGER_H
