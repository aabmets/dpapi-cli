/*
 *   MIT License
 *
 *   Copyright (c) 2024, Mattias Aabmets
 *
 *   The contents of this file are subject to the terms and conditions defined in the License.
 *   You may not use, modify, or distribute this file except in compliance with the License.
 *
 *   SPDX-License-Identifier: MIT
 */

#ifndef METADATA_H
#define METADATA_H

#define PRODUCT_NAME        "dpapi-cli"
#define DESCRIPTION         "Data Protection API Command-Line Interface"
#define COMPANY_NAME        "Mattias Aabmets"
#define COPYRIGHT           "Copyright (C) 2024 Mattias Aabmets"
#define ORIGINAL_FILENAME   "dpapi-cli.exe"

#define VERSION_MAJOR       1
#define VERSION_MINOR       0
#define VERSION_PATCH       2
#define VERSION_BUILD       0

#define STRINGIZE2(s)       #s
#define STRINGIZE(s)        STRINGIZE2(s)

#define SEMVER_MAIN         STRINGIZE(VERSION_MAJOR) \
                                "." STRINGIZE(VERSION_MINOR) \
                                "." STRINGIZE(VERSION_PATCH)
                                
#define SEMVER_BUILD        SEMVER_MAIN "." STRINGIZE(VERSION_BUILD)
#define SEMVER_CSV          VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_BUILD

#endif  // METADATA_H

#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS

#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101

#endif  // APSTUDIO_READONLY_SYMBOLS
#endif  // APSTUDIO_INVOKED
