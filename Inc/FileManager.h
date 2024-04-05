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

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <cstring>
#include <vector>
#include <regex>
#include <fstream>
#include <cstdint>

#include"Inc/DisplayManager.h"
#include "Inc/Error.h"

constexpr uint8_t TSV_NB_COLUMNS = 7;

struct partitionInfo
{
    std::string opt;
    std::string phaseID;
    std::string partName;
    std::string partType;
    std::string partIp;
    std::string offset;
    std::string binary;
};

struct fileTSV
{
    std::vector<partitionInfo> partitionsList;
};

class FileManager
{
public:
    static FileManager& getInstance() ;
    int openTsvFile(const std::string &fileName, fileTSV **parsedFile);

private:
    FileManager();
    int parseTsvFile(const std::string tsvFolderPath, std::ifstream *inFile, fileTSV* parsedTSV);
    int splitStdString(std::string str, std::regex, std::vector<std::string>& substrings) ;

    DisplayManager displayManager = DisplayManager::getInstance() ;

};

#endif // FILEMANAGER_H
