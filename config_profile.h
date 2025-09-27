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
#include "util.h"
#include "logger.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <string>
class ConfigProfile {
public:
    ConfigProfile(const rapidjson::Value& profile, int i = 0);

    std::string getName() const { return name; }
    std::string getOdooUrl() const { return odoo_url; }
    std::string getOdooDb() const { return odoo_db; }
    std::string getOdooUsername() const { return odoo_username; }
    std::string getOdooApiKey() const { return odoo_api_key; }
    std::string getIban() const { return iban; }
    int getOdooCompanyId() const { return odoo_company_id; }

private:
    std::string name;
    std::string iban;
    std::string odoo_url;
    std::string odoo_db;
    std::string odoo_username;
    std::string odoo_api_key;
    int odoo_company_id;
};