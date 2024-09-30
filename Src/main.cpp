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

#include "main.h"
#include "ProgramManager.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

std::string PRG_TOOLBOX_FASTBOOT_VERSION = "2.0.0";
std::string toolboxRootPath = "" ;

int main(int argc, char* argv[])
{
    std::string fastbootSerialNumber = "";

    displayManager.print(MSG_NORMAL, L"      -------------------------------------------------------------------") ;
    displayManager.print(MSG_NORMAL, L"                      PRG-TOOLBOX-FB v%s                      ", PRG_TOOLBOX_FASTBOOT_VERSION.c_str()) ;
    displayManager.print(MSG_NORMAL, L"      -------------------------------------------------------------------\n\n") ;

    int nCommands  = extractProgramCommands(argc, argv);
    if((nCommands < 0 ) || (nCommands == 0))
        return EXIT_FAILURE ;

    try
    {
        toolboxRootPath = fs::path(argv[0]).parent_path().string();
        if(toolboxRootPath.empty())
            toolboxRootPath = "." ; // The working directory is the same as where the executable is located.

        displayManager.print(MSG_NORMAL, L"TOOLBOX parent path : %s ", toolboxRootPath.c_str()) ;
    }
    catch(...)
    {
        displayManager.print(MSG_ERROR, L"Failed to get the Toolbox Root Path") ;
        return EXIT_FAILURE;
    }

    /* Check the list of commands before starting the execution (Option that can be placed anywhere on the command line) */
    for (int cmdIdx=0; cmdIdx < nCommands; cmdIdx++)
    {
        if(compareStrings(argumentsList[cmdIdx].cmd , "-sn", true) || compareStrings(argumentsList[cmdIdx].cmd , "--serial", true))
        {
            if((argumentsList[cmdIdx].nParams != 1))
            {
                displayManager.print(MSG_ERROR, L"Wrong parameters for -sn/--serial command") ;
                showHelp();
                return EXIT_FAILURE;
            }

            fastbootSerialNumber = argumentsList[cmdIdx].Params[0];
            std::transform(fastbootSerialNumber.begin(), fastbootSerialNumber.end(), fastbootSerialNumber.begin(), ::toupper);
            displayManager.print(MSG_NORMAL, L"Selected device serial number : %s", fastbootSerialNumber.data()) ;
        }
    }

    /* Search and execute commands */
    for (int cmdIdx=0; cmdIdx < nCommands; cmdIdx++)
    {
        if (compareStrings(argumentsList[cmdIdx].cmd , "-?", true) || compareStrings(argumentsList[cmdIdx].cmd , "-h", true) || compareStrings(argumentsList[cmdIdx].cmd , "--help", true))
        {
            showHelp();
        }
        else if(compareStrings(argumentsList[cmdIdx].cmd , "-v", true) || compareStrings(argumentsList[cmdIdx].cmd , "--version", true))
        {
            displayManager.print(MSG_NORMAL, L"PRG-TOOLBOX-FB version : %s",  PRG_TOOLBOX_FASTBOOT_VERSION.data()) ;
        }
        else if(compareStrings(argumentsList[cmdIdx].cmd , "-l", true) || compareStrings(argumentsList[cmdIdx].cmd , "--list", true))
        {
            Fastboot *fastbootInterface = new Fastboot() ;
            fastbootInterface->toolboxFolder = toolboxRootPath ;
            int ret = fastbootInterface->displayDevicesList();

            delete fastbootInterface;
            if(ret)
                return EXIT_FAILURE;
        }
        else if(compareStrings(argumentsList[cmdIdx].cmd , "-sn", true) || compareStrings(argumentsList[cmdIdx].cmd , "--serial", true))
        {
            /* It has already been treated previously */
            continue ;
        }
        else if(compareStrings(argumentsList[cmdIdx].cmd , "-d", true) || compareStrings(argumentsList[cmdIdx].cmd , "--download", true))
        {
            if(argumentsList[cmdIdx].nParams > 1 )
            {
                displayManager.print(MSG_ERROR, L"Extra parameters for -d/--download command") ;
                showHelp();
                return EXIT_FAILURE;
            }

            std::string tsvFilePath = argumentsList[cmdIdx].Params[0];
            if(tsvFilePath.substr(tsvFilePath.size() - 4) != ".tsv" )
            {
                displayManager.print(MSG_ERROR, L"Download command : wrong file extension !\nExpected file extension is .tsv") ;
                return EXIT_FAILURE;
            }

            ProgramManager *programMng = new ProgramManager(toolboxRootPath, fastbootSerialNumber);
            int ret = programMng->startFlashingService(std::move(tsvFilePath) );
            delete programMng;

            if(ret)
            {
                displayManager.print(MSG_ERROR, L"Download command failed !") ;
                return EXIT_FAILURE;
            }

        }
        else
        {
            displayManager.print(MSG_ERROR, L"Wrong command [ %s ]: Unknown command or command missed some parameters.\nPlease refer to the help for the supported commands.", argumentsList[cmdIdx].cmd.c_str()) ;
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}

/**
 * @brief compareStrings: Compare two strings with with case tracking.
 * @param str1: First string to compare.
 * @param str2: Second string to compare.
 * @param caseInsensitive: True to compare with insensitve case, otherwise false.
 * @return True if the two strings are equal, otherwise false.
 */
bool compareStrings(const std::string& str1, const std::string& str2, bool caseInsensitive)
{
    if(caseInsensitive == true)
    {
        std::string str1Lower = str1;
        std::transform(str1Lower.begin(), str1Lower.end(), str1Lower.begin(), ::tolower);
        std::string str2Lower = str2;
        std::transform(str2Lower.begin(), str2Lower.end(), str2Lower.begin(), ::tolower);

        // Compare the lowercase std::strings: returns true if it is equal
        return str1Lower.compare(str2Lower) == 0;
    }
    else
    {
        return str1.compare(str2) ;
    }
}


/**
 * @brief extractProgramCommands: check and extract the total of commands which are passed to the program.
 * @param numberCommands: Initial number of commands passed to the program.
 * @param commands: List of string passed as commands.
 * @return postive value indacating the number of commands , otherwise, negative value to eraise an error.
 */
int extractProgramCommands(int numberCommands, char* commands[])
{
    if(numberCommands == 1) // No argument
    {
        showHelp();
        return TOOLBOX_FASTBOOT_ERROR_WRONG_PARAM ;
    }

    uint8_t nCmds  =0;
    uint8_t argumentIndex = 1;
    while(argumentIndex < numberCommands)
    {
        if (commands[argumentIndex][0] == '-')
        {
            /* Extract commands */
            argumentsList[nCmds].cmd = commands[argumentIndex];

            /* Extract parameters */
            argumentsList[nCmds].nParams=0;
            if (argumentIndex+1 < numberCommands)
            {
                while ((argumentIndex+1+argumentsList[nCmds].nParams<numberCommands) && (commands[argumentIndex+1+argumentsList[nCmds].nParams][0] != '-') && (argumentsList[nCmds].nParams < MAX_PARAMS_NBR))
                {
                    argumentsList[nCmds].Params[argumentsList[nCmds].nParams] = commands[argumentIndex+1+argumentsList[nCmds].nParams];
                    argumentsList[nCmds].nParams++;
                }
            }
            argumentIndex += argumentsList[nCmds].nParams+1;
            nCmds++;
        }
        else
        {
            displayManager.print(MSG_ERROR, L"Argument error. Use -? for help") ;
            showHelp();
            return TOOLBOX_FASTBOOT_ERROR_WRONG_PARAM;
        }
    }

    /* verify the command syntax */
    bool validCommand = false;
    for (uint8_t cmdIdx=0; cmdIdx < nCmds; cmdIdx++)
    {
        validCommand = false;
        for (int i=0; i < MAX_COMMANDS_NBR; i++)
        {
            if (compareStrings(argumentsList[cmdIdx].cmd , supportedCommandList[i], true) == true)
            {
                validCommand=true;
                break ;
            }
        }

        if (validCommand == false)
        {
            displayManager.print(MSG_ERROR, L"Invalid command : %s", argumentsList[cmdIdx].cmd.data()) ;
            showHelp();
            return TOOLBOX_FASTBOOT_ERROR_WRONG_PARAM;
        }
    }

    return nCmds ;
}

/**
 * @brief showHelp : Display the list of available commands.
 */
void showHelp()
{
    displayManager.print(MSG_GREEN, L"Usage :") ;
    displayManager.print(MSG_NORMAL, L"PRG-TOOLBOX-FB [command_1] [Arguments_1] [[command_2] [Arguments_2]...]\n") ;

    displayManager.print(MSG_NORMAL, L"--help        -h   -?       : Show the help menu.") ;
    displayManager.print(MSG_NORMAL, L"--version          -v       : Display the program version.") ;
    displayManager.print(MSG_NORMAL, L"--list             -l       : Display the list of available Fastboot devices.") ;
    displayManager.print(MSG_NORMAL, L"--serial           -sn      : Select the USB device by serial number.") ;
    displayManager.print(MSG_NORMAL, L"--download         -d       : Prepare the device, flash/update the memory partitions over fastboot mode.") ;
    displayManager.print(MSG_NORMAL, L"       <filePath.tsv>       : TSV file path") ;

    displayManager.print(MSG_NORMAL, L"") ;
}
