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

#include <algorithm>
#include "Inc/Fastboot.h"
#include <filesystem>

Fastboot::Fastboot()
{

}

/**
 * @brief Fastboot::flashPartition : Get the fastboot command ready, then flash one partition..
 * @param partitionName: The name of the flash partition to update.
 * @param partitionFirmwarePath: The binary file to be used to program.
 * @return 0 if the operation is performed successfully, otherwise an error occurred.
 */
int Fastboot::flashPartition(const std::string partitionName, const std::string partitionFirmwarePath)
{
    displayManager.print(MSG_NORMAL, L"Partition name  : %s", partitionName.c_str());
    displayManager.print(MSG_NORMAL, L"Firmware path   : %s\n", partitionFirmwarePath.c_str());

    std::string fastbootCmd = getFastbootProgramPath().append("flash ") ;
    fastbootCmd.append(partitionName).append(" ").append(partitionFirmwarePath).append("  2>&1") ;
#ifdef _WIN32
    fastbootCmd = "\"" + fastbootCmd + "\"" ;
#endif
    displayManager.print(MSG_NORMAL, L"fastboot command: %s", fastbootCmd.data()) ;

    FILE* pipe = popen(fastbootCmd.c_str(), "r");
    if (pipe == nullptr)
    {
        displayManager.print(MSG_ERROR, L"Failed to open pipe") ;
        return TOOLBOX_FASTBOOT_ERROR_NO_MEM;
    }

    char buffer[4096];
    std::string result = "";
    while (!feof(pipe))
    {
        if (fgets(buffer, 4096, pipe) != nullptr)
        {
            result += buffer;
        }
    }
    pclose(pipe);

    std::cout << result << std::endl ;
    std::string searchString = "Finished.";
    size_t pos = result.find(searchString);
    if (pos != std::string::npos)
    {
        displayManager.print(MSG_GREEN, L"Patition %s : Download Done\n", partitionName.c_str()) ;
        return TOOLBOX_FASTBOOT_NO_ERROR ;

    }
    else
    {
        displayManager.print(MSG_ERROR, L"Patition %s : Download Failed", partitionName.c_str()) ;
        return TOOLBOX_FASTBOOT_ERROR_WRITE ;
    }
}

/**
 * @brief Fastboot::oemFormatMemory : Execute OEM-specific command to configure the partitions list into the target memory.
 * @return 0 if the operation is performed successfully, otherwise an error occurred.
 */
int Fastboot::oemFormatMemory()
{
    displayManager.print(MSG_NORMAL, L"Memory partitioning...\n") ;

    std::string fastbootCmd = getFastbootProgramPath().append("oem format ").append("  2>&1")  ;
    displayManager.print(MSG_NORMAL, L"fastboot command: %s", fastbootCmd.data()) ;

    FILE* pipe = popen(fastbootCmd.c_str(), "r");
    if (pipe == nullptr)
    {
        displayManager.print(MSG_ERROR, L"Failed to open pipe") ;
        return TOOLBOX_FASTBOOT_ERROR_NO_MEM;
    }

    char buffer[4096];
    std::string result = "";
    while (!feof(pipe))
    {
        if (fgets(buffer, 4096, pipe) != nullptr)
        {
            result += buffer;
        }
    }
    pclose(pipe);

    std::cout << result << std::endl ;
    std::string searchString = "Finished.";
    size_t pos = result.find(searchString);
    if (pos != std::string::npos)
    {
        displayManager.print(MSG_NORMAL, L"Target memory partitioning is done.") ;
        return TOOLBOX_FASTBOOT_NO_ERROR ;

    }
    else
    {
        displayManager.print(MSG_ERROR, L"Failed to setup the partitions format.") ;
        return TOOLBOX_FASTBOOT_ERROR_WRITE ;
    }
}

/**
 * @brief Fastboot::isUbootFastbootRunning : Check if there is a device with U-Boot in fastboot is running
 * @return True if a STM32 device in fastboot mode is running , otherwise no device is present.
 */
bool Fastboot::isUbootFastbootRunning()
{
    std::string  fastbootCmd = getFastbootProgramPath().append("devices") ;
    FILE* pipe = popen(fastbootCmd.c_str(), "r");
    if (pipe == nullptr)
    {
        displayManager.print(MSG_ERROR, L"Failed to open pipe") ;
        return false;
    }

    char buffer[4096];
    std::string result = "";

    while (!feof(pipe))
    {
        if (fgets(buffer, 4096, pipe) != nullptr)
        {
            result += buffer;
        }
    }
    pclose(pipe);

    std::string searchString = "fastboot" ;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    size_t pos = result.find(searchString);

    if (pos != std::string::npos)
    {
        displayManager.print(MSG_GREEN, L"U-Boot in Fastboot mode is running !") ;
        return true ;

    }
    else
    {
        displayManager.print(MSG_WARNING, L"No U-Boot in Fastboot mode is running !") ;
        return false ;
    }
}

/**
 * @brief Fastboot::getFastbootProgramPath : Get the path of fastboot program from the project directory.
 * @return The fastboot executable path.
 */
std::string Fastboot::getFastbootProgramPath()
{
    std::string path = this->toolboxFolder; //from the project tree
#ifdef _WIN32
    path.append("\\fastboot\\Windows\\fastboot.exe") ;
    path = "\"" + path + "\" " ;
#elif __APPLE__
    path.append("/fastboot/MacOS/fastboot") ;
    path = "\"" + path + "\" " ;
#elif __linux__
    path.append("/fastboot/Linux/fastboot") ;
    path = "\"" + path + "\" " ;
#else
    path = "" ;
#endif

    displayManager.print(MSG_NORMAL, L"fastboot application path : %s", path.c_str()) ;
    return path;
}
