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
#include <string>
#include <ctime>
#include <algorithm>


#pragma once
#include <string>
#include <vector>

class Pain00100109 {
public:
    struct Payment {
        std::string timestamp;
        std::string execution_date; //2025-09-03 YYYY-MM-DD

        std::string payment_reference;
        std::string payment_memo;
        //double amount;
        std::string amount_str;

        std::string e2e_id;
        std::string msg_id;
        std::string pmt_id;
        std::string ins_id;
	
        std::string currency;
        std::string debtorName;
        std::string debtorIban;
        std::string creditorName;
        std::string creditorIban;
        std::string iso20022_uetr;
        int payment_id = 0;

        std::string BuyerOrganisationName="";
        std::string BuyerOrganisationTaxCode="";
        std::string BuyerStreetName="";
        std::string BuyerTownName="";
        std::string BuyerCountryCode="";
        std::string BuyerPostCodeIdentifier="";
        std::string BuyerOVT="";
        std::string BuyerIntermediator="";

        int BuyerBankAccountId=0;
        std::string BuyerBic="";
        std::string BuyerBankName="";
        std::string BuyerAccountNumber="";
        std::string BuyerBankTownName="";
        std::string BuyerBankCountryCode="";

        std::string SellerOrganisationName="";
        std::string SellerOrganisationTaxCode="";
        std::string SellerStreetName="";
        std::string SellerTownName="";
        std::string SellerCountryCode="";
        std::string SellerPostCodeIdentifier="";
        std::string SellerOVT="";
        std::string SellerIntermediator="";

        std::string SellerBic="";
        std::string SellerBankName="";
        std::string SellerAccountNumber="";
        std::string SellerBankTownName="";
        std::string SellerBankCountryCode="";

        std::string unique_id="";
    };

    Pain00100109() = default;
    std::string generateXml(Payment &payment);

    std::string getTransfer_international(Payment &payment);
    std::string getTransfer_sepa_instant(Payment &payment);
    std::string getTransfer_sepa_payment(Payment &payment);
    std::string getPmtInf(Payment &payment);
    std::string getHeader(Payment &payment);
    std::string getDebitorInfo(Payment &payment);
    std::string generateTimestamp();
    std::string getRemittanceInfo(Payment &payment);

};
