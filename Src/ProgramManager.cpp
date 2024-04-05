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


#include "Inc/ProgramManager.h"
#include <chrono>

using namespace std ;

ProgramManager::ProgramManager(const std::string toolboxFolder)
{
    fastbootInterface = new Fastboot() ;
    fastbootInterface->toolboxFolder = toolboxFolder ;
    parsedTsvFile = nullptr;
}

ProgramManager::~ProgramManager()
{
    delete fastbootInterface ;
    delete parsedTsvFile ;
}

/**
 * @brief ProgramManager::startFlashingService: Navigate through the partition list and flash the appropriate firmwares.
 * @param inputTsvPath: The TSV file to apply.
 * @return 0 if the operation is performed successfully, otherwise an error occurred.
 */
int ProgramManager::startFlashingService(const std::string inputTsvPath)
{
    auto start = std::chrono::high_resolution_clock::now(); // get start time

    int ret = TOOLBOX_FASTBOOT_NO_ERROR ;

    if(fastbootInterface->isUbootFastbootRunning() == false)
    {
        displayManager.print(MSG_NORMAL, L"No flashing service will be performed !");
        return TOOLBOX_FASTBOOT_ERROR_NO_DEVICE ;
    }

    if(fileManager.openTsvFile(inputTsvPath, &parsedTsvFile) != 0)
    {
        displayManager.print(MSG_ERROR, L"Failed to download TSV partitions: %s", inputTsvPath.c_str());
        return TOOLBOX_FASTBOOT_ERROR_NO_FILE ;
    }

    displayManager.print(MSG_NORMAL, L"-----------------------------------------");
    displayManager.print(MSG_GREEN, L"TSV fastboot downloading...");
    displayManager.print(MSG_NORMAL, L"  TSV path           : %s", inputTsvPath.data() );
    displayManager.print(MSG_NORMAL, L"  Partitions number  : %lu", parsedTsvFile->partitionsList.size() );
    displayManager.print(MSG_NORMAL,L"-----------------------------------------\n" );

    if(fastbootInterface->oemFormatMemory() != TOOLBOX_FASTBOOT_NO_ERROR)
    {
        displayManager.print(MSG_ERROR, L"Failed to format partitions, No flashing service will be performed !");
        return TOOLBOX_FASTBOOT_ERROR_INTERFACE_NOT_SUPPORTED;
    }

    displayManager.print(MSG_NORMAL, L"\nStart flashing service...\n\n");

    for(auto &part: parsedTsvFile->partitionsList)
    {
        std::string patternNone = "none\"";

        if((part.opt == "-") || (part.binary == "none") || (part.binary.size() >= patternNone.size() && part.binary.substr(part.binary.size() - patternNone.size()) == patternNone)) //ignore the field containing none keyword
            continue ;

        if((part.partName == "fsbl1") && (part.offset == "boot1"))
        {
            ret = fastbootInterface->flashPartition("mmc1boot0", part.binary) ; //U-Boot's keyword to update this specific boot partition for eMMC memory: fsbl1
            if(ret != TOOLBOX_FASTBOOT_NO_ERROR)
                break ;
        }
        else if ((part.partName == "fsbl2") && (part.offset == "boot2"))
        {
            ret = fastbootInterface->flashPartition("mmc1boot1", part.binary) ; //U-Boot's keyword to update this specific boot partition for eMMC memory: fsbl2
            if(ret != TOOLBOX_FASTBOOT_NO_ERROR)
                break ;
        }
        else
        {
            ret = fastbootInterface->flashPartition(part.partName, part.binary) ;
            if(ret != TOOLBOX_FASTBOOT_NO_ERROR)
                break ;
        }

    }

    if(ret == TOOLBOX_FASTBOOT_NO_ERROR)
    {
        auto end = std::chrono::high_resolution_clock::now(); // get end time
        auto duration = std::chrono::duration_cast< std::chrono::milliseconds>(end - start);
        displayManager.print(MSG_NORMAL, L"Flashing service finished."),
        displayManager.print(MSG_GREEN, L"Time elapsed to flash all partitions: %ld min, %02ld s, %03ld ms", (duration.count() / (1000 * 60)), ((duration.count() / 1000) % 60), (duration.count() % 1000));
    }
    else
    {
        displayManager.print(MSG_ERROR, L"Failed to flash partitions !");
    }

    return ret ;
}
