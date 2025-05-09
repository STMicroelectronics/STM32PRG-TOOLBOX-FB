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
#include "Fastboot.h"
#include <experimental/filesystem>
#include <regex>

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
    fastbootCmd.append(partitionName).append(" ").append(partitionFirmwarePath) ;
    if(this->fastbootSerialNumber != "")
        fastbootCmd.append(" -s ").append(this->fastbootSerialNumber);

    fastbootCmd.append("  2>&1");
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
        displayManager.print(MSG_GREEN, L"Partition %s : Download Done\n", partitionName.c_str()) ;
        return TOOLBOX_FASTBOOT_NO_ERROR ;

    }
    else
    {
        displayManager.print(MSG_ERROR, L"Partition %s : Download Failed", partitionName.c_str()) ;
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

    std::string fastbootCmd = getFastbootProgramPath().append("oem format ") ;
    if(this->fastbootSerialNumber != "")
        fastbootCmd.append(" -s ").append(this->fastbootSerialNumber);

    fastbootCmd.append("  2>&1");
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

    std::string searchString = "FASTBOOT" ;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    size_t pos = result.find(searchString);

    if (pos != std::string::npos)
    {
        /* The fastboot tool does not have a built-in command to list a specific device by its serial number,
            Need to parse the result list and search for the device by its serial number mentioned with -sn command if it is present */
        if(this->fastbootSerialNumber != "")
        {
            pos = result.find(this->fastbootSerialNumber);
            if(pos == std::string::npos)
            {
                /* Device with this serial number is not present */
                displayManager.print(MSG_WARNING, L"No U-Boot [%s] in Fastboot mode is running !", this->fastbootSerialNumber.data()) ;
                return false ;
            }

        }

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

/**
 * @brief Fastboot::erasePartition : Erase a specific partition
 * @param partitionName: The partition name to be erased.
 * @return 0 if the operation is performed successfully, otherwise an error occurred.
 */
int Fastboot::erasePartition(const std::string partitionName)
{
    displayManager.print(MSG_NORMAL, L"Erasing partition [%s]...", partitionName.c_str());

    std::string fastbootCmd = getFastbootProgramPath().append("erase ") ;
    fastbootCmd.append(partitionName) ;
    if(this->fastbootSerialNumber != "")
        fastbootCmd.append(" -s ").append(this->fastbootSerialNumber);

    fastbootCmd.append("  2>&1");
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
        displayManager.print(MSG_GREEN, L"Partition %s : Erase Done\n", partitionName.c_str()) ;
        return TOOLBOX_FASTBOOT_NO_ERROR ;

    }
    else
    {
        displayManager.print(MSG_ERROR, L"Partition %s : Erase Failed", partitionName.c_str()) ;
        return TOOLBOX_FASTBOOT_ERROR_WRITE ;
    }
}


/**
 * @brief Fastboot::displayDevicesList : Print the list of available Fastboot devices.
 * @return 0 if the operation is performed successfully, otherwise an error occurred.
 */
int Fastboot::displayDevicesList()
{
    std::string  fastbootCmd =  getFastbootProgramPath().append(" devices") ;
    displayManager.print(MSG_NORMAL, L"fastboot command: %s", fastbootCmd.data()) ;

    FILE* pipe = popen(fastbootCmd.c_str(), "r");
    if (pipe == nullptr)
    {
        displayManager.print(MSG_ERROR, L"Failed to open pipe") ;
        return TOOLBOX_FASTBOOT_ERROR_OTHER;
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

    std::vector<std::string> serialNumbers;
    try
    {
        std::regex regex("([A-F0-9]+)\\s+(fastboot|Android Fastboot)");
        std::smatch match;

        std::string::const_iterator searchStart(result.cbegin());
        while (std::regex_search(searchStart, result.cend(), match, regex))
        {
            std::string serial = match[1];
            serialNumbers.push_back(serial);
            searchStart = match.suffix().first;
        }
    }
    catch (const std::regex_error& e)
    {
        displayManager.print(MSG_ERROR, L"Regex error: %s", e.what());
        return false ;
    }

    // Check if any devices were found
    if (serialNumbers.empty())
    {
        displayManager.print(MSG_NORMAL, L"") ;
        displayManager.print(MSG_WARNING, L"No Fastboot devices found.") ;
    }
    else
    {
        displayManager.print(MSG_GREEN, L"\nFastboot devices List") ;
        displayManager.print(MSG_NORMAL, L" Number of Fastboot devices: %d", serialNumbers.size()) ;
        int deviceCount = 1;
        for (const auto& serial : serialNumbers)
        {
            displayManager.print(MSG_NORMAL, L" [Device %d] : ", deviceCount) ;
            displayManager.print(MSG_NORMAL, L"     Serial number : %s", serial.c_str()) ;
            deviceCount++;
        }
    }

    return TOOLBOX_FASTBOOT_NO_ERROR ;
}
