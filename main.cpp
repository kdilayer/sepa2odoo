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
/*


INSTALL 

    sudo mkdir /var/log/sepa2odoo
    sudo nano /etc/toolsconf/sepa2odoo.conf

    sudo chown -R toolsusr:toolsusr /var/log/sepa2odoo
    sudo chown -R toolsusr:toolsusr /etc/toolsconf/sepa2odoo.conf


    sudo nano /etc/crontab

    test:
    sudo -u toolsusr /usr/sbin/sepa2odoo -c /etc/toolsconf/sepa2odoo.conf

35 6    * * *   toolsusr /usr/sbin/sepa2odoo -c /etc/toolsconf/sepa2odoo.conf >> /var/log/sepa2odoo/sepa2odoo.log
5 15    * * *   toolsusr /usr/sbin/sepa2odoo -c /etc/toolsconf/sepa2odoo.conf >> /var/log/sepa2odoo/sepa2odoo.log

*/

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include <iostream>
#include <map>
#include "odoo_api.h"


#include "version_num.h"
#include "build_defs.h"
#include <unistd.h> 

#include "util.h"
#include <rapidjson/error/en.h>
#include "config_profile.h"

#include <filesystem>
#include "camt05300102.h"
#include "pain00200110.h"
#include "pain00100109.h"
#include "raaccounttransactions.h"
INITIALIZE_EASYLOGGINGPP

using namespace std;
void doHelp(const char *app, const char* ver){
     fprintf(stderr,
            "Usage: %s [OPTION]...\n"
            "Input data can be piped or use -i option. Output can be a file or stdout.\n"
            "Example: %s -h\n"
            "\n"
             " -h            Print out this help\n"
             "\n\n"
            "%s version %s\n",
            app,
            app,
            app, ver);
}
std::string buildTime = formattedString(
    "%c%c.%c%c.%c%c%c%c-%c%c:%c%c:%c%c", 
    BUILD_DAY_CH0,  BUILD_DAY_CH1,
    BUILD_MONTH_CH0, BUILD_MONTH_CH1,
    BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3,
    BUILD_HOUR_CH0, BUILD_HOUR_CH1,
    BUILD_MIN_CH0,  BUILD_MIN_CH1,
        BUILD_SEC_CH0, BUILD_SEC_CH1
    );
std::string versionNumber= formattedString("%c.%c-%s%s", 
    VERSION_MAJOR_INIT, 
    VERSION_MINOR_INIT, 
    buildTime.c_str(),
    #ifdef _DEBUG
        "d"
    #else
        "r"
    #endif
);
std::vector<std::string> getFilesToUpload(std::string dir_path) {
    std::vector<std::string> files;
    
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().filename());
        }
    }
    return files;
}
std::string getUntilUnderscore(const std::string& input, size_t maxChars) {
    size_t pos = input.find('_');
    size_t end = (pos != std::string::npos && pos < maxChars) ? pos : maxChars;
    return input.substr(0, end);
}
std::string getSepaFileType(const std::string& filename) {
    // Implement your logic to determine the SEPA file type based on the filename
    // For now, let's return a dummy value
    return getUntilUnderscore(filename, 10);
}
std::string serverName = "sepa2odoo";
int main(int argc, char *argv[]) {
    int c = 0;
    std::string configFile = "sepa2odoo.conf";
    while ((c = getopt (argc, argv, "c:h")) != -1) {
        switch (c){
            case 'c':
                configFile = optarg;
                break;
            case 'h':
                doHelp(serverName.c_str(), versionNumber.c_str());
                return -1;
            continue;
        
        }
    }   
    if (optind != argc)    {
        fprintf(stderr, "A non option was supplied\n");
        doHelp(serverName.c_str(), versionNumber.c_str());
        return -1;
    }  
    if(configFile == "") {
        fprintf(stderr, "Config file not specified\n");
        doHelp(serverName.c_str(), versionNumber.c_str());
        return -1;  
    }
    LOG(INFO) << "Starting " << serverName << " version " << versionNumber; 
    #ifdef _DEBUG
        configFile += ".debug";
    #endif

    std::string config = ReadFileContent(configFile);

    rapidjson::Document layout;
    rapidjson::ParseResult iok = layout.Parse(config.c_str());
    if(iok) {
        LOG(INFO) << "Config file " << configFile << " loaded successfully";
    } else {
        LOG(ERROR) << "Failed to parse config file " << configFile << ": "
                   << rapidjson::GetParseError_En(layout.GetParseError())
                   << " (offset " << layout.GetErrorOffset() << ")";
        // Optionally, print the line number:
        size_t offset = layout.GetErrorOffset();
        size_t line = 1;
        for (size_t i = 0; i < offset && i < config.size(); ++i) {
            if (config[i] == '\n') ++line;
        }
        LOG(ERROR) << "Parse error occurred at line: " << line;
        return -1;
    }
    if (!layout.IsObject() || !layout.HasMember("profiles") || !layout["profiles"].IsArray()) {
        LOG(ERROR) << "Invalid config file format: 'profiles' array not found";
        return -1;
    }
    const rapidjson::Value& profiles = layout["profiles"];
    if (!layout.IsObject() || !layout.HasMember("working_folder") || !layout["working_folder"].IsString()) {
        LOG(ERROR) << "Invalid config file format: 'working_folder' not found";
        return -1;
    }
    std::string workingFolder = layout["working_folder"].GetString();
    if(workingFolder.empty()) {
        LOG(ERROR) << "Working folder not found in config";
        return -1;
    }
    
    //get sepa files account statements and upload them to Odoo
    
    std::vector<std::string> sepafiles = getFilesToUpload(workingFolder+"/incoming");
    for(int i=0; i < sepafiles.size(); i++) {
        const std::string& fname = sepafiles[i];
        std::string type = getSepaFileType  (fname);
        bool file_processed = false;
        /*
        if(type == "RA") {
            std::string raContent = ReadFileContent(workingFolder+"/incoming/"+fname);
            RAAccountTransactions raentry;
            if (raentry.parse(raContent)) {
                // Process raentry
                for (rapidjson::SizeType i = 0; i < profiles.Size(); ++i) {
                    const rapidjson::Value& profile = profiles[i];
                    ConfigProfile configProfile(profile, i);            
                    if(configProfile.getIban() == "") {
                        // Commit changes
                        LOG(INFO) << configProfile.getName() << ": sepa file RA; account statement " << fname;
                        OdooAPI odooApi(
                            configProfile.getOdooUrl(),
                            configProfile.getOdooDb(),
                            configProfile.getOdooUsername(),
                            configProfile.getOdooApiKey(),
                            configProfile.getOdooCompanyId()
                        );
                        if(odooApi.authenticate()){
                            //LOG(INFO) << "Odoo authentication successful for profile " << i  << ": " << configProfile.getName();

                            if(true) { //odooApi.addAccountStatement(configProfile.getIban(), raentry)) {
                                LOG(INFO) << "Account statement handled successfully for profile " << i << ": " << configProfile.getName() << ", removing file...";

                                //move this file to processed folder
                                moveToFolder(fname, workingFolder+"/incoming", workingFolder+"/processed");
                                continue;
                            }
                            else {
                                LOG(ERROR) << "Failed to add account statement for profile " << i << ": " << configProfile.getName();
                            }

                        } else {
                            LOG(ERROR) << "Odoo authentication failed for profile " << i << ": " << configProfile.getName();
                        }
                    }
                }
            }
            else {
                LOG(ERROR) << "Failed to parse RA entry: " << fname;
            }
        }
        */
        if(type == "XT") {
            std::string camtContent = ReadFileContent(workingFolder+"/incoming/"+fname);
            Camt05300102 camtentry;
            
            if (camtentry.parse(camtContent)) {
                //run trough the profiles and commit changes
                for (rapidjson::SizeType i = 0; i < profiles.Size(); ++i) {
                    const rapidjson::Value& profile = profiles[i];
                    ConfigProfile configProfile(profile, i);
                    if(configProfile.getIban() == camtentry.getAccount()) {
                        // Commit changes
                        LOG(INFO) << configProfile.getName() << ": sepa file XT; bank statement " << fname;
                        //LOG(INFO) << "Committing changes for profile " << configProfile.getName() << " and account " << camtentry.getAccount();
                        OdooAPI odooApi(
                            configProfile.getOdooUrl(),
                            configProfile.getOdooDb(),
                            configProfile.getOdooUsername(),
                            configProfile.getOdooApiKey(),
                            configProfile.getOdooCompanyId()
                        );
                        if(odooApi.authenticate()){
                            //LOG(INFO) << "Odoo authentication successful for profile " << i  << ": " << configProfile.getName();

                            if(odooApi.addBankStatement(configProfile.getIban(), camtentry)) {
                                LOG(INFO) << "Bank statement handled successfully for profile " << i << ": " << configProfile.getName() << ", removing file...";

                                //move this file to processed folder
                                moveToFolder(fname, workingFolder+"/incoming", workingFolder+"/processed");
                                file_processed = true;
                            }
                            else {
                                LOG(ERROR) << "Failed to add bank statement for profile " << i << ": " << configProfile.getName();
                            }

                        } else {
                            LOG(ERROR) << "Odoo authentication failed for profile " << i << ": " << configProfile.getName();
                        }
                    }
                }
                
            }
            else {
                LOG(ERROR) << "Failed to parse camt.053.001.02 file: " << fname;
            }
        }
        if(type == "XP") {
            std::string painContent = ReadFileContent(workingFolder+"/incoming/"+fname);
            Pain00200110 painentry;
            Pain00200110::GroupStatus groupStatus;
            if (painentry.parse(painContent, groupStatus)) {
                // Process painentry
                for (rapidjson::SizeType i = 0; i < profiles.Size(); ++i) {
                    const rapidjson::Value& profile = profiles[i];
                    ConfigProfile configProfile(profile, i);
                    
                    if( true /*configProfile.getName() == groupStatus.initgPtyNm*/ ) { //<- check cannot be performed
                        OdooAPI odooApi(
                            configProfile.getOdooUrl(),
                            configProfile.getOdooDb(),
                            configProfile.getOdooUsername(),
                            configProfile.getOdooApiKey(),
                            configProfile.getOdooCompanyId()
                        );
                        if(odooApi.authenticate()){
                            //LOG(INFO) << "Odoo authentication successful for profile " << i  << ": " << configProfile.getName();
                            if(odooApi.handlePaymentStatus(groupStatus)) {
                                LOG(INFO) << configProfile.getName() << ": processed feedback for payment ("<<fname<<"): " << groupStatus.orgnlMsgId << ": " << groupStatus.grpSts;
                                //we are done with this file so we can now remove it
                                moveToFolder(fname, workingFolder+"/incoming", workingFolder+"/processed");
                                file_processed = true;
                            }

                        } else {
                            LOG(ERROR) << "Odoo authentication failed for profile " << i << ": " << configProfile.getName();
                        }
                    }
                }
            }
        }

        if(file_processed == false) {
            if(type == "XP" || type == "XT" || type == "RA") {
                LOG(INFO) << "Either XP, XT or RA but not recognized so move to not_processed: " << fname;
                moveToFolder(fname, workingFolder+"/incoming", workingFolder+"/not_processed");
            }
            else {
                LOG(INFO) << "Unrecognized file type: " << fname << " move to not_processed";
                moveToFolder(fname, workingFolder+"/incoming", workingFolder+"/not_processed");
            }
        }
    }
    
    //handle odoo payments 
    for (rapidjson::SizeType i = 0; i < profiles.Size(); ++i) {
        const rapidjson::Value& profile = profiles[i];

        ConfigProfile configProfile(profile, i);

        //LOG(INFO) << "Profile " << i << ": " << configProfile.getName();
        OdooAPI odooApi(
            configProfile.getOdooUrl(),
            configProfile.getOdooDb(),
            configProfile.getOdooUsername(),
            configProfile.getOdooApiKey(),
            configProfile.getOdooCompanyId()
        );
        if(odooApi.authenticate()){
            //LOG(INFO) << "Odoo authentication successful for profile " << i  << ": " << configProfile.getName();

        } else {
            LOG(ERROR) << "Odoo authentication failed for profile " << i << ": " << configProfile.getName();
        }
        int paymentsProcessed = odooApi.processUnpaidPayments(
            [&](std::string xml_content, std::string e2eid) -> bool {
                // Write the XML content to a file
                std::string outFolder = workingFolder+"/outgoing";
                return WriteFileContent(outFolder+"/XL_"+e2eid+".xml", xml_content, true);
            }
        );
        LOG(INFO) << "Processed " << paymentsProcessed << " payments for profile " << i << ": " << configProfile.getName();

    }
        
}
