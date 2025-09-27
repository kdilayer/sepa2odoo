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
#include "util.h"
#include <cstdarg>
#include <cstdio>       
#include <string>   
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <unordered_set>

bool isEuropeanCountryCode(const std::string& code) {
    static const std::unordered_set<std::string> europeanCodes = {
        "AT","BE","BG","HR","CY","CZ","DK","EE","FI","FR","DE","GR","HU","IE","IT",
        "LV","LT","LU","MT","NL","PL","PT","RO","SK","SI","ES","SE","GB","IS","LI",
        "NO","CH","MC","SM","VA","AL","AD","BA","MK","MD","ME","RS","UA","BY"
    };
    if (code.size() != 2) return false;
    std::string upperCode = code;
    std::transform(upperCode.begin(), upperCode.end(), upperCode.begin(), ::toupper);
    return europeanCodes.count(upperCode) > 0;
}
// Trim from the start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Trim from the end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}
// Trim from both ends (in place)
void string_trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}
bool string_equals_ignore_case(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (tolower(a[i]) != tolower(b[i])) return false;
    }
    return true;
}
std::string stringToBigDecimalNumber(const std::string& input, bool insertDecimal) {
    std::string buf = input;
    // Insert decimal point before last two digits if requested
    if (insertDecimal && buf.size() > 2)
        buf.insert(buf.size() - 2, ".");
    // Remove leading zeroes or spaces after the sign
    while (buf.size() > 1 && (buf[1] == '0' || buf[1] == ' ')) {
        buf.erase(1, 1);
    }
    // Replace comma with dot if not inserting decimal
    if (!insertDecimal) {
        std::replace(buf.begin(), buf.end(), ',', '.');
    }
    // Trim whitespace
    buf.erase(0, buf.find_first_not_of(" \t\n\r"));
    buf.erase(buf.find_last_not_of(" \t\n\r") + 1);
    return buf;
}
std::string grabStringFromPositions(const std::string& str, size_t start, size_t end) {
    if (start >= str.size() || end > str.size() || start >= end) return "";
    std::string retval =  str.substr(start, end - start);

    return retval;
}
std::string getTimebuffer() {
    char buffer[20];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm* tm_info = localtime(&tv.tv_sec);
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", tm_info);
    return std::string(buffer);
}
bool moveToFolder(const std::string& filename, const std::string& fromFolder, const std::string& toFolder) {
    std::string newfname = filename+"_"+ getTimebuffer();
    std::string sourcePath = fromFolder + "/" + filename;
    std::string destPath = toFolder + "/" + newfname;

    // Create destination folder if it doesn't exist
    std::filesystem::create_directories(toFolder);

    try {
        std::filesystem::rename(sourcePath, destPath);
        return true; // Move successful
    } catch (const std::filesystem::filesystem_error& e) {
    }
    return true;
}
std::string string_remove_whitespace(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (!std::isspace(c)) {
            result += c;
        }
    }
    return result;
}

std::string string_fmt_money(double value, int decimals, bool use_comma_separator) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << value;
    std::string out= oss.str();
    if (use_comma_separator) {
        string_replaceall(out, ".", ","); // Ensure comma as decimal separator
    }
    return out;
}
std::string getTimestamp(std::string format) {
    //2025-08-26T12:53:54
    std::ostringstream oss;
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    if(format == "DD-MM-YYYY") {
        oss << std::put_time(std::localtime(&in_time_t), "%d-%m-%Y");
        return oss.str();
    }
    else if(format == "YYYYMMDD") {
        oss << std::put_time(std::localtime(&in_time_t), "%Y%m%d");
        return oss.str();
    }
    oss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}
std::string getLastDigits(const std::string& str) {
    if (str.empty()) return str;
    size_t pos = str.size();
    while (pos > 0 && std::isdigit(str[pos - 1])) {
        --pos;
    }
    return str.substr(pos);
}
std::string generateRandomEpiRef(std::string input) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);

    std::string base = getLastDigits(input);
    if(base == "") {
        //if no input was provided, generate random
        std::ostringstream oss;
        // Generate 9 random digits (leave last for checksum)
        for (int i = 0; i < 9; ++i) {
            oss << dis(gen);
        }
        base = oss.str();
    }
    // Calculate checksum using your generateInvoiceChecksum function
    std::string checksum = generateInvoiceChecksum(base);

    // Append checksum
    std::string epiRef = base + checksum;
    return epiRef;
}
std::string generateInvoiceChecksum(const std::string& input) {
    int counter = 0;
    int totalSum = 0;
    int totalChars = input.size();

    for (int i = totalChars; i >= 1; i--) {
        int multiplier = 1;
        if (counter == 0) {
            multiplier = 7;
            counter++;
        } else if (counter == 1) {
            multiplier = 3;
            counter++;
        } else if (counter == 2) {
            multiplier = 1;
            counter = 0;
        }
        // Convert char to int (assuming input contains only digits)
        int digit = input[i - 1] - '0';
        totalSum += multiplier * digit;
    }
    // Take last digit from totalSum
    int lastDigit = totalSum % 10;
    int checkSum = 10 - lastDigit;
    if (checkSum == 10) checkSum = 0;
    return std::to_string(checkSum);
}
std::string formattedString(const char *format, ...)
{
    std::string strBuffer(128, 0);
    va_list ap, backup_ap;
    va_start(ap, format);
    va_copy(backup_ap, ap);
    auto result = vsnprintf((char *)strBuffer.data(),
                            strBuffer.size(),
                            format,
                            backup_ap);
    va_end(backup_ap);
    if ((result >= 0) && ((std::string::size_type)result < strBuffer.size()))
    {
        strBuffer.resize(result);
    }
    else
    {
        while (true)
        {
            if (result < 0)
            {
                // Older snprintf() behavior. Just try doubling the buffer size
                strBuffer.resize(strBuffer.size() * 2);
            }
            else
            {
                strBuffer.resize(result + 1);
            }

            va_copy(backup_ap, ap);
            auto result = vsnprintf((char *)strBuffer.data(),
                                    strBuffer.size(),
                                    format,
                                    backup_ap);
            va_end(backup_ap);

            if ((result >= 0) &&
                ((std::string::size_type)result < strBuffer.size()))
            {
                strBuffer.resize(result);
                break;
            }
        }
    }
    va_end(ap);
    return strBuffer;
}

std::string ReadFileContent(std::string filename) {
    std::string filecontent;
    FILE *file = fopen(filename.c_str(), "rb");
    if (file == NULL) {
        return filecontent;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    
    // allocate string space and set length
    filecontent.resize(size);

    // read 1*size bytes from sfile into ss
    fread(&filecontent[0], 1, size, file);

    // close the file
    fclose(file);
    return filecontent;
}
bool WriteFileContent(std::string filename, std::string &content, bool overwrite) {

    if(overwrite == false && filename!= "" && file_exists(filename)){
        return false;
    }
    FILE *file = stdout; 
    if(filename != "") 
        file = fopen(filename.c_str(), "wb");
    if (file == NULL) {
        return false;
    }

    size_t written = fwrite(content.c_str(), 1, content.length(), file);
    // close the file
    if(filename != "") 
        fclose(file);

    if(written != content.length())
        return false;

    return true;
}
bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}
std::string timestamp_to_string(int ts_seconds, bool print_hours) {
    std::time_t in_t = ts_seconds;
    std::tm * in_lt = std::localtime(&in_t);
    char inb[64];
    if(print_hours) {
        std::strftime(inb, 64, "%d.%m.%Y %H:%M:%S", in_lt);
    }
    else {
        std::strftime(inb, 64, "%d.%m.%Y", in_lt);
    }
    return std::string(inb);
}


bool string_startswith(const char* haystack, size_t haystackSize, const char* needle, size_t needleSize) {
    if (haystackSize < needleSize) {
        return false;
    }
    return strncmp(haystack, needle, needleSize) == 0;
}

bool string_startswith(const std::string haystack, const std::string needle) {
    return string_startswith((const char*)haystack.c_str(), (size_t) haystack.size(), (const char*)needle.c_str(), (size_t)needle.size());
}
bool string_endswith(const std::string& haystack, const std::string& needle) {
    if (needle.size() > haystack.size())
        return false;
    else
        return (haystack.substr(haystack.size() - needle.size()) == needle);
}
std::string string_trim(const std::string& str, const std::string& whitespace){
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}
void string_replaceall(std::string& source, const std::string& from, const std::string& to)
{
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    newString += source.substr(lastPos);

    source.swap(newString);
}
std::string escape_json(const std::string &s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u"
                  << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(*c);
            } else {
                o << *c;
            }
        }
    }
    return o.str();
}
const std::map<std::string, std::string> CountryNameToCode = {
    {"Afghanistan", "AF"},
    {"Aland Islands", "AX"},
    {"Albania", "AL"},
    {"Algeria", "DZ"},
    {"American Samoa", "AS"},
    {"Andorra", "AD"},
    {"Angola", "AO"},
    {"Anguilla", "AI"},
    {"Antarctica", "AQ"},
    {"Antigua and Barbuda", "AG"},
    {"Argentina", "AR"},
    {"Armenia", "AM"},
    {"Aruba", "AW"},
    {"Australia", "AU"},
    {"Austria", "AT"},
    {"Azerbaijan", "AZ"},
    {"Bahamas", "BS"},
    {"Bahrain", "BH"},
    {"Bangladesh", "BD"},
    {"Barbados", "BB"},
    {"Belarus", "BY"},
    {"Belgium", "BE"},
    {"Belize", "BZ"},
    {"Benin", "BJ"},
    {"Bermuda", "BM"},
    {"Bhutan", "BT"},
    {"Bolivia", "BO"},
    {"Bonaire, Sint Eustatius and Saba", "BQ"},
    {"Bosnia and Herzegovina", "BA"},
    {"Botswana", "BW"},
    {"Bouvet Island", "BV"},
    {"Brazil", "BR"},
    {"British Indian Ocean Territory", "IO"},
    {"Brunei Darussalam", "BN"},
    {"Bulgaria", "BG"},
    {"Burkina Faso", "BF"},
    {"Burundi", "BI"},
    {"Cambodia", "KH"},
    {"Cameroon", "CM"},
    {"Canada", "CA"},
    {"Cape Verde", "CV"},
    {"Cayman Islands", "KY"},
    {"Central African Republic", "CF"},
    {"Chad", "TD"},
    {"Chile", "CL"},
    {"China", "CN"},
    {"Christmas Island", "CX"},
    {"Cocos (Keeling) Islands", "CC"},
    {"Colombia", "CO"},
    {"Comoros", "KM"},
    {"Congo", "CG"},
    {"Congo, Democratic Republic of the", "CD"},
    {"Cook Islands", "CK"},
    {"Costa Rica", "CR"},
    {"Cote d'Ivoire", "CI"},
    {"Croatia", "HR"},
    {"Cuba", "CU"},
    {"Curacao", "CW"},
    {"Cyprus", "CY"},
    {"Czech Republic", "CZ"},
    {"Denmark", "DK"},
    {"Djibouti", "DJ"},
    {"Dominica", "DM"},
    {"Dominican Republic", "DO"},
    {"Ecuador", "EC"},
    {"Egypt", "EG"},
    {"El Salvador", "SV"},
    {"Equatorial Guinea", "GQ"},
    {"Eritrea", "ER"},
    {"Estonia", "EE"},
    {"Eswatini", "SZ"},
    {"Ethiopia", "ET"},
    {"Falkland Islands (Malvinas)", "FK"},
    {"Faroe Islands", "FO"},
    {"Fiji", "FJ"},
    {"Finland", "FI"},
    {"France", "FR"},
    {"French Guiana", "GF"},
    {"French Polynesia", "PF"},
    {"French Southern Territories", "TF"},
    {"Gabon", "GA"},
    {"Gambia", "GM"},
    {"Georgia", "GE"},
    {"Germany", "DE"},
    {"Ghana", "GH"},
    {"Gibraltar", "GI"},
    {"Greece", "GR"},
    {"Greenland", "GL"},
    {"Grenada", "GD"},
    {"Guadeloupe", "GP"},
    {"Guam", "GU"},
    {"Guatemala", "GT"},
    {"Guernsey", "GG"},
    {"Guinea", "GN"},
    {"Guinea-Bissau", "GW"},
    {"Guyana", "GY"},
    {"Haiti", "HT"},
    {"Heard Island and McDonald Islands", "HM"},
    {"Holy See (Vatican City State)", "VA"},
    {"Honduras", "HN"},
    {"Hong Kong", "HK"},
    {"Hungary", "HU"},
    {"Iceland", "IS"},
    {"India", "IN"},
    {"Indonesia", "ID"},
    {"Iran, Islamic Republic of", "IR"},
    {"Iraq", "IQ"},
    {"Ireland", "IE"},
    {"Isle of Man", "IM"},
    {"Israel", "IL"},
    {"Italy", "IT"},
    {"Jamaica", "JM"},
    {"Japan", "JP"},
    {"Jersey", "JE"},
    {"Jordan", "JO"},
    {"Kazakhstan", "KZ"},
    {"Kenya", "KE"},
    {"Kiribati", "KI"},
    {"Korea, Democratic People's Republic of", "KP"},
    {"Korea, Republic of", "KR"},
    {"Kuwait", "KW"},
    {"Kyrgyzstan", "KG"},
    {"Lao People's Democratic Republic", "LA"},
    {"Latvia", "LV"},
    {"Lebanon", "LB"},
    {"Lesotho", "LS"},
    {"Liberia", "LR"},
    {"Libya", "LY"},
    {"Liechtenstein", "LI"},
    {"Lithuania", "LT"},
    {"Luxembourg", "LU"},
    {"Macao", "MO"},
    {"Madagascar", "MG"},
    {"Malawi", "MW"},
    {"Malaysia", "MY"},
    {"Maldives", "MV"},
    {"Mali", "ML"},
    {"Malta", "MT"},
    {"Marshall Islands", "MH"},
    {"Martinique", "MQ"},
    {"Mauritania", "MR"},
    {"Mauritius", "MU"},
    {"Mayotte", "YT"},
    {"Mexico", "MX"},
    {"Micronesia, Federated States of", "FM"},
    {"Moldova, Republic of", "MD"},
    {"Monaco", "MC"},
    {"Mongolia", "MN"},
    {"Montenegro", "ME"},
    {"Montserrat", "MS"},
    {"Morocco", "MA"},
    {"Mozambique", "MZ"},
    {"Myanmar", "MM"},
    {"Namibia", "NA"},
    {"Nauru", "NR"},
    {"Nepal", "NP"},
    {"Netherlands", "NL"},
    {"New Caledonia", "NC"},
    {"New Zealand", "NZ"},
    {"Nicaragua", "NI"},
    {"Niger", "NE"},
    {"Nigeria", "NG"},
    {"Niue", "NU"},
    {"Norfolk Island", "NF"},
    {"North Macedonia", "MK"},
    {"Northern Mariana Islands", "MP"},
    {"Norway", "NO"},
    {"Oman", "OM"},
    {"Pakistan", "PK"},
    {"Palau", "PW"},
    {"Palestine, State of", "PS"},
    {"Panama", "PA"},
    {"Papua New Guinea", "PG"},
    {"Paraguay", "PY"},
    {"Peru", "PE"},
    {"Philippines", "PH"},
    {"Pitcairn", "PN"},
    {"Poland", "PL"},
    {"Portugal", "PT"},
    {"Puerto Rico", "PR"},
    {"Qatar", "QA"},
    {"Reunion", "RE"},
    {"Romania", "RO"},
    {"Russian Federation", "RU"},
    {"Rwanda", "RW"},
    {"Saint Barthelemy", "BL"},
    {"Saint Helena, Ascension and Tristan da Cunha", "SH"},
    {"Saint Kitts and Nevis", "KN"},
    {"Saint Lucia", "LC"},
    {"Saint Martin (French part)", "MF"},
    {"Saint Pierre and Miquelon", "PM"},
    {"Saint Vincent and the Grenadines", "VC"},
    {"Samoa", "WS"},
    {"San Marino", "SM"},
    {"Sao Tome and Principe", "ST"},
    {"Saudi Arabia", "SA"},
    {"Senegal", "SN"},
    {"Serbia", "RS"},
    {"Seychelles", "SC"},
    {"Sierra Leone", "SL"},
    {"Singapore", "SG"},
    {"Sint Maarten (Dutch part)", "SX"},
    {"Slovakia", "SK"},
    {"Slovenia", "SI"},
    {"Solomon Islands", "SB"},
    {"Somalia", "SO"},
    {"South Africa", "ZA"},
    {"South Georgia and the South Sandwich Islands", "GS"},
    {"South Sudan", "SS"},
    {"Spain", "ES"},
    {"Sri Lanka", "LK"},
    {"Sudan", "SD"},
    {"Suriname", "SR"},
    {"Svalbard and Jan Mayen", "SJ"},
    {"Sweden", "SE"},
    {"Switzerland", "CH"},
    {"Syrian Arab Republic", "SY"},
    {"Taiwan, Province of China", "TW"},
    {"Tajikistan", "TJ"},
    {"Tanzania, United Republic of", "TZ"},
    {"Thailand", "TH"},
    {"Timor-Leste", "TL"},
    {"Togo", "TG"},
    {"Tokelau", "TK"},
    {"Tonga", "TO"},
    {"Trinidad and Tobago", "TT"},
    {"Tunisia", "TN"},
    {"Turkey", "TR"},
    {"Turkmenistan", "TM"},
    {"Turks and Caicos Islands", "TC"},
    {"Tuvalu", "TV"},
    {"Uganda", "UG"},
    {"Ukraine", "UA"},
    {"United Arab Emirates", "AE"},
    {"United Kingdom", "GB"},
    {"United States", "US"},
    {"United States Minor Outlying Islands", "UM"},
    {"Uruguay", "UY"},
    {"Uzbekistan", "UZ"},
    {"Vanuatu", "VU"},
    {"Venezuela", "VE"},
    {"Viet Nam", "VN"},
    {"Virgin Islands, British", "VG"},
    {"Virgin Islands, U.S.", "VI"},
    {"Wallis and Futuna", "WF"},
    {"Western Sahara", "EH"},
    {"Yemen", "YE"},
    {"Zambia", "ZM"}
};
std::string getFirstTwoChars(const std::string& str) {
    if (str.size() < 2) return str;
    return str.substr(0, 2);
}

// Returns true if str starts with any of the given country codes (case-insensitive)
bool startsWithCountryCode(const std::string& str) {
    std::string firstTwoChars = getFirstTwoChars(str);
    for (const auto& kv : CountryNameToCode) {
        if (std::equal(kv.second.begin(), kv.second.end(), firstTwoChars.begin(), firstTwoChars.end(),
            [](char a, char b) { return std::tolower(a) == std::tolower(b); })) {
            return true;
        }
    }
    return false;        
}

std::string getCountryCodeFromName(const std::string& name) {
    // Case-insensitive search
    for (const auto& kv : CountryNameToCode) {
        if (std::equal(kv.first.begin(), kv.first.end(), name.begin(), name.end(),
            [](char a, char b) { return std::tolower(a) == std::tolower(b); })) {
            return kv.second;
        }
    }
    return "";
}
std::string getCountryNameForCode(const std::string& code) {
    for (const auto& kv : CountryNameToCode) {
        if (kv.second == code) {
            return kv.first;
        }
    }
    return "";
}