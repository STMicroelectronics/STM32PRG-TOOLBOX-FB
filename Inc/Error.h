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

#ifndef ERROR_H
#define ERROR_H

enum ToolboxError {
    /** Success (no error) */
    TOOLBOX_FASTBOOT_NO_ERROR = 0,

    /** Device not connected */
    TOOLBOX_FASTBOOT_ERROR_NOT_CONNECTED = -1,

    /** Device not found */
    TOOLBOX_FASTBOOT_ERROR_NO_DEVICE = -2,

    /** Device connection error */
    TOOLBOX_FASTBOOT_ERROR_CONNECTION = -3,

    /** No such file  */
    TOOLBOX_FASTBOOT_ERROR_NO_FILE = -4,

    /** Operation not supported or unimplemented on this interface */
    TOOLBOX_FASTBOOT_ERROR_NOT_SUPPORTED = -5,

    /** Interface not supported or unimplemented on this plateform */
    TOOLBOX_FASTBOOT_ERROR_INTERFACE_NOT_SUPPORTED = -6,

    /** Insufficient memory */
    TOOLBOX_FASTBOOT_ERROR_NO_MEM = -7,

    /** Wrong parameters */
    TOOLBOX_FASTBOOT_ERROR_WRONG_PARAM = -8,

    /** Memory read failure */
    TOOLBOX_FASTBOOT_ERROR_READ = -9,

    /** Memory write failure */
    TOOLBOX_FASTBOOT_ERROR_WRITE = -10,

    /** File format not supported for this kind of device */
    TOOLBOX_FASTBOOT_ERROR_UNSUPPORTED_FILE_FORMAT = -11,

    /** Other error */
    TOOLBOX_FASTBOOT_ERROR_OTHER = -99,
};

#endif // ERROR_H
