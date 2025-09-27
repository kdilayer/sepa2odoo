/*
 * Copyright (c) 2025 @https://github.com/kdilayer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#pragma once

#include "config_app.h"

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <chrono>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include <vector>
#include <string>

#include <iostream>
#include <string>
#include <stdlib.h>

#include "logger.h"

bool isEuropeanCountryCode(const std::string& code);
std::string formattedString(const char *format, ...);
std::string ReadFileContent(std::string filename);
bool file_exists (const std::string& name);
bool WriteFileContent(std::string filename, std::string &content, bool overwrite=false);
std::string timestamp_to_string(int ts_seconds, bool print_hours = false);
bool string_startswith(const char* haystack, size_t haystackSize, const char* needle, size_t needleSize);
bool string_startswith(const std::string haystack, const std::string needle);
bool string_endswith(const std::string& haystack, const std::string& needle);
bool string_equals_ignore_case(const std::string& a, const std::string& b);
std::string string_remove_whitespace(const std::string& str);
std::string string_trim(const std::string& str, const std::string& whitespace);
void string_trim(std::string &s);
std::string string_fmt_money(double value, int decimals=3, bool use_comma_separator=false);
void string_replaceall(std::string& source, const std::string& from, const std::string& to);
std::string escape_json(const std::string &s);
std::string getCountryCodeFromName(const std::string& name);
bool startsWithCountryCode(const std::string& str);
std::string getFirstTwoChars(const std::string& str);
std::string getCountryNameForCode(const std::string& code);
std::string generateRandomEpiRef(std::string input);
std::string generateInvoiceChecksum(const std::string& input);
std::string getTimestamp(std::string format="");
bool moveToFolder(const std::string& filename, const std::string& fromFolder, const std::string& toFolder);
std::string grabStringFromPositions(const std::string& str, size_t start, size_t end);
std::string stringToBigDecimalNumber(const std::string& input, bool insertDecimal = false);
