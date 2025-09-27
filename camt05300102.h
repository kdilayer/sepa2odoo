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
#include <string>
#include <vector>
#include <map>

class Camt05300102 {
public:
    struct Transaction {
        public:
        std::string amount;
        std::string currency;
        std::string date;
        std::string description;
        std::string debtor; // Name of the party that sent money
        std::string creditor; // Name of the party that received money
        std::string owner;
        std::string bic;
        std::string type;

        std::string familycd;
        std::string subfamilycd;
        std::string prtrycd;
        std::string instrid;
        std::string endtoendid;

        std::string batchmsgid;
        std::string batchpmtinfid;
        std::string batchnboftxs;

        std::string amtdtlsamt;
        std::string acceptancedate;
        std::string dbtrorigid;
    };
    struct Balance {
        public:
        std::string amount;
        std::string currency;
        std::string date;
        std::string creditline;
        std::string cdtlineincl;
        std::string cdtdbtind;
    };

    Camt05300102() = default;
    bool parse(const std::string& xmlContent);
    const std::vector<Transaction>& getTransactions() const;
    std::string getAccount() const;
    std::string getStatementId() const;
    std::string getDate() const;

    std::vector<Transaction> transactions;
    std::string account;
    std::string statementId;
    std::string date;
    std::string currency;
    std::string owner;
    std::string bic;

    // Opening balance OPBD
    Balance openingBalance;
    // Closing balance CLBD
    Balance closingBalance;
};
