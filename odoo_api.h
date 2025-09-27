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
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <string>
#include <functional>
#include "camt05300102.h"
#include "pain00100109.h"
#include "pain00200110.h"

class OdooAPI {
    bool has_error = false;

    bool convertResultToJson(const xmlrpc_c::value& result, rapidjson::Document &doc);
    bool odooCommand(const std::string& method, const std::string& model, const xmlrpc_c::value_array& domain, xmlrpc_c::value* result);
    int vendorExists_ex(std::string const& taxCode);
    int getCompanyTaxId(std::string taxString);
    int getCompanyCurrencyId(std::string currencyString);
    std::string getCompanyInfoByCompanyId(int companyId, std::string &name, std::string &taxcode, std::string &street, std::string &town, std::string &postCode, std::string &country_code, std::string &ovt, std::string &intermediator, int &bankAccountId, bool is_internal_company = false);
    
    int getFiscalPositionId();

    int createVendorBankAccount(int vendorId, std::string SellerAccountID, std::string SellerBic, std::string SellerAccountName);
    int vendorBankExists(int vendorId, std::string SellerAccountName);
    int createVendorBank(int vendorId, std::string SellerAccountName, std::string SellerBic);
    int findCountryId(std::string countryName);

    //int OdooInvoiceToFinvoice(const rapidjson::Value& entry, FinvoiceInvoice& invoice);
    //bool getInvoiceRowById(rapidjson::Document &doc, int rowId);

    double getCompanyTaxRatePercentById(int taxId);
    bool getBankAccountBic(int partner_bank_id, std::string &bic, std::string &bankName, std::string &accNumber, std::string &bankTownName, std::string &bankCountryCode);
    bool getBankAddress(int bank_id, std::string &bankTownName, std::string &bankCountryCode);

    bool updateDomainField(std::string domain, int domain_id, const std::string fieldName, const std::string fieldValue);
    bool updateDomainField_b(std::string domain, int domain_id, const std::string fieldName, const bool fieldValue);
    std::string getNextByCode();
    int getPaymentIdByOriginalMsgId(const std::string& messageId, std::string &current_x_studio_e2e_error);

public:
    OdooAPI(const std::string& url,
            const std::string& db,
            const std::string& username,
            const std::string& apikey,
            const int companyId=1 /*Your company, the company id that you are operating on*/
    );

    bool hasError() const {
        return has_error;
    }
    bool authenticate();
/*
    int vendorExists(std::string const& taxCode);
    int createVendor(std::string const& taxCode, const FinvoiceInvoice& inv);

    bool vendorBillExists(const std::string& eioInvoiceIdentifier);
    int createVendorBill(const FinvoiceInvoice& inv);
    int processUnsentInvoices(std::function<std::string (std::string odoo_maventa_status, std::string maventa_invoice_identifier, FinvoiceInvoice &invoice, std::vector<FinvoiceAttachment> &attachments, bool &send_confirmed, std::string &send_error_msg)> processInvoiceCallback, int lastHowManyDays=30);
*/
    bool addBankStatement(std::string iban, const Camt05300102& camtentry);
    int getJournalForBankAccount(std::string iban);
    bool bankStatementExists(std::string statement_id);

    std::string getVendorBillDueDate(int vendorBillId, std::string &payment_ref);
    int processUnpaidPayments(std::function<bool (std::string xml_content, std::string e2eid)> processInvoiceCallback);
    bool handlePaymentStatus(const Pain00200110::GroupStatus& groupStatus);
private:
    std::string url_, db_, username_, apikey_;
    int loggedOnCompanyId=0, loggedOnUserId=0;
};