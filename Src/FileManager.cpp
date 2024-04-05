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

#include "Inc/FileManager.h"
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

#include <experimental/filesystem>

FileManager::FileManager()
{

}

FileManager & FileManager::getInstance()
{
    static FileManager instance;
    return instance;
}

/**
 * @brief FileManager::openTsvFile : Open and parse a TSV file containing the list of memory partitions.
 * @param fileName: The TSV file path.
 * @param parsedFile: Output variable to store the parsed file information.
 * @return 0 if the operation is performed successfully, otherwise an error occurred.
 */
int FileManager::openTsvFile(const std::string &fileName, fileTSV **parsedFile)
{
    fileTSV* parsedTSV = NULL;
    std::ifstream inFile(fileName);

    if(inFile.is_open() == false)
    {
        displayManager.print(MSG_ERROR, L"The file does not exist :  %s",  fileName.data());
        return TOOLBOX_FASTBOOT_ERROR_NO_FILE;
    }

    try
    {
        parsedTSV = new fileTSV;
    }
    catch(const std::bad_alloc&)
    {
        displayManager.print(MSG_ERROR, L"Cannot allocate memory to read file : %s",  fileName.data());
        inFile.close();
        return TOOLBOX_FASTBOOT_ERROR_NO_MEM;
    }

    std::string tsvFolderPath = "" ;
    try
    {
        tsvFolderPath = std::experimental::filesystem::path(fileName).parent_path().string() ;
    }
    catch(...)
    {
        delete parsedTSV;
        inFile.close();
        return TOOLBOX_FASTBOOT_ERROR_OTHER;
    }

    if(parseTsvFile(std::move(tsvFolderPath), &inFile, parsedTSV) == 0)
    {
        *parsedFile = parsedTSV;
        inFile.close();
    }
    else
    {
        delete parsedTSV;
        inFile.close();
        return TOOLBOX_FASTBOOT_ERROR_OTHER;
    }

    return 0;
}

/**
 * @brief FileManager::parseTsvFile : The engine part of the methode "openTsvFile"
 * @param tsvFolderPath: The folder that contains the TSV file.
 * @param inFile: The TSV file path.
 * @param parsedTSV: Output variable to store the parsed file information.
 * @return 0 if the operation is performed successfully, otherwise an error occurred.
 */
int FileManager::parseTsvFile(const std::string tsvFolderPath, std::ifstream *inFile, fileTSV* parsedTSV)
{   
    inFile->seekg(0, std::ios::end) ;
    int fSize = inFile->tellg() ;
    if(fSize == 0)
    {
        displayManager.print(MSG_ERROR, L"TSV file is empty !") ;
        return TOOLBOX_FASTBOOT_ERROR_NO_FILE ;
    }
    else
    {
        inFile->seekg(0, std::ios::beg) ;
    }

    while (inFile->eof() == false)
    {
        partitionInfo tempPartition;
        std::string line ;
        std::getline(*inFile, line) ;

        if(line.empty() == true)
            continue;

        if(line.at(0) == '#') /* filter the header which starts with "#" */
            continue;

        std::vector<std::string> infomartionList ;
        try
        {
            std::regex delimiter("\\t+");
            if(splitStdString(std::move(line), std::move(delimiter) , infomartionList))
                return TOOLBOX_FASTBOOT_ERROR_OTHER ;
        }
        catch (const std::regex_error& e)
        {
            displayManager.print(MSG_ERROR, L"Regex error: %s", e.what());
            return TOOLBOX_FASTBOOT_ERROR_NO_MEM;
        }

        if(infomartionList.size() != TSV_NB_COLUMNS )
        {
            displayManager.print(MSG_ERROR, L"TSV file is not conform, it may miss some columns or fields");
            return TOOLBOX_FASTBOOT_ERROR_WRONG_PARAM;
        }

        tempPartition.opt   = infomartionList[0];
        tempPartition.phaseID = infomartionList[1];
        tempPartition.partName = infomartionList[2];
        tempPartition.partType = infomartionList[3];
        tempPartition.partIp = infomartionList[4];
        tempPartition.offset  = infomartionList[5];
        tempPartition.binary =  infomartionList[6];

        if(tempPartition.binary != "none")
        {
            std::ifstream binaryFile(tempPartition.binary);
            if(binaryFile.is_open() == false) // file does not exist
            {
                /* Try to search from the folder that contains the TSV file */
                std::string tmpPath = "" ;
                tmpPath.append(tsvFolderPath).append("/").append(tempPartition.binary) ;
                tempPartition.binary = std::move(tmpPath)  ;

                binaryFile.close();
                binaryFile.open(tempPartition.binary);
                if(binaryFile.is_open() == false)
                {
                    displayManager.print(MSG_ERROR, L"File %s does not exist !", tempPartition.binary.c_str());
                    return TOOLBOX_FASTBOOT_ERROR_WRONG_PARAM;
                }
            }
            tempPartition.binary = "\"" + tempPartition.binary + "\""; //To take into account the paths with white spaces;
        }

        parsedTSV->partitionsList.push_back(tempPartition);
    }

    return TOOLBOX_FASTBOOT_NO_ERROR ;
}

/**
 * @brief FileManager::splitStdString : Split an input string basiong on a specifc format and delimiter.
 * @param str: The input string.
 * @param delimiter: the separator to apply like "\\t+"...
 * @param stringsList: The output variable to store the splited strings.
 * @return 0 if the operation is performed successfully, otherwise an error occurred.
 */
int FileManager::splitStdString(std::string str, std::regex delimiter, std::vector<std::string>& stringsList)
{
    if(str.empty())
    {
        displayManager.print(MSG_ERROR, L"Cannot split string, input is empty") ;
        return TOOLBOX_FASTBOOT_ERROR_WRONG_PARAM;
    }

    std::vector<std::string> substrings(
                std::sregex_token_iterator(str.begin(), str.end(), delimiter, -1),
                std::sregex_token_iterator()
                );

    stringsList = std::move(substrings);

    return TOOLBOX_FASTBOOT_NO_ERROR ;
}
