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
#include <fstream>
#include <sstream>
#include <rapidxml.hpp>
#include <rapidxpath.hpp>
#include "camt05300102.h"
#include "logger.h"
using namespace rapidxml;

xml_node<>* xml_doc_get_node(xml_node<>* doc, const std::string &xpath) {
    std::vector<std::string> parts;
    size_t start = 0, end = 0;
    while ((end = xpath.find('/', start)) != std::string::npos) {
        if (end > start)
            parts.push_back(xpath.substr(start, end - start));
        start = end + 1;
    }
    if (start < xpath.size())
        parts.push_back(xpath.substr(start));

    if(!doc) {
        return nullptr;
    }
    xml_node<>* current = doc->first_node(parts[0].c_str());
    if(current == nullptr)
        return nullptr;
    for(int i=1; i < parts.size(); i++) {
        std::string part = parts[i];
        // Subsequent parts are children
        current = current->first_node(part.c_str());
        if(current == nullptr)
            break;
    }
    return current;
}

bool Camt05300102::parse(const std::string& xmlContent) {
    transactions.clear();
    account.clear();
    statementId.clear();
    date.clear();
    
    std::vector<char> xml_copy(xmlContent.begin(), xmlContent.end());
    xml_copy.push_back('\0');
    
    try {
        xml_document<> doc;
        doc.parse<0>(&xml_copy[0]);
        
        // Get account
        if(!doc.first_node("Document")) {
            LOG(ERROR) << "Invalid XML format: Missing Document node";
            return false;
        }
        auto* acctNode = doc.first_node("Document")->first_node("BkToCstmrStmt")->first_node("Stmt")->first_node("Acct")->first_node("Id")->first_node("IBAN");
        if (acctNode) account = acctNode->value();
        // Get statement id
        auto* stmtIdNode = doc.first_node("Document")->first_node("BkToCstmrStmt")->first_node("Stmt")->first_node("Id");
        if (stmtIdNode) statementId = stmtIdNode->value();
        // Get date
        auto* dateNode = doc.first_node("Document")->first_node("BkToCstmrStmt")->first_node("Stmt")->first_node("CreDtTm");
        if (dateNode) date = dateNode->value();

        // Get Currency
        auto* currNode = doc.first_node("Document")->first_node("BkToCstmrStmt")->first_node("Stmt")->first_node("Acct")->first_node("Ccy");
        if (currNode) currency = currNode->value();
        // Get Owner
        auto* ownerNode = doc.first_node("Document")->first_node("BkToCstmrStmt")->first_node("Stmt")->first_node("Acct")->first_node("Ownr")->first_node("Nm");
        if (ownerNode) owner = ownerNode->value();
        //Get BIC
        auto* bicNode = doc.first_node("Document")->first_node("BkToCstmrStmt")->first_node("Stmt")->first_node("Acct")->first_node("Svcr")->first_node("FinInstnId")->first_node("BIC");
        if (bicNode) bic = bicNode->value();

        // Get Balance
        for (auto* balanceNode = doc.first_node("Document")->first_node("BkToCstmrStmt")->first_node("Stmt")->first_node("Bal"); balanceNode; balanceNode = balanceNode->next_sibling("Bal")) {
            std::string amount;
            std::string currency;
            std::string date;
            std::string creditline;
            std::string cdtlineincl;
            std::string type;

            auto* typeNode = balanceNode->first_node("Tp")->first_node("CdOrPrtry")->first_node("Cd");
            if (typeNode) type = typeNode->value();

            auto* amtNode = balanceNode->first_node("Amt");
            if (amtNode) {
                amount = amtNode->value();
                auto* curr = amtNode->first_attribute("Ccy");
                currency = curr ? curr->value() : "";
            }
            auto* bookDt = balanceNode->first_node("Dt")->first_node("Dt");
            if (bookDt) date = bookDt->value();
            auto* cdtline = balanceNode->first_node("CdtLine")->first_node("Amt");;
            if (cdtline) creditline = cdtline->value();
            auto* cdtlineinclv = balanceNode->first_node("CdtLine")->first_node("Incl");;
            if (cdtlineinclv) cdtlineincl = cdtlineinclv->value();

            std::string cdtDbtInd="";
            auto* cdtDbtIndNode = balanceNode->first_node("CdtDbtInd");
            if (cdtDbtIndNode) cdtDbtInd = cdtDbtIndNode->value();

            if(type == "OPBD") {
                openingBalance.amount = amount;
                openingBalance.currency = currency;
                openingBalance.date = date;
                openingBalance.creditline = creditline;
                openingBalance.cdtlineincl = cdtlineincl;
                openingBalance.cdtdbtind = cdtDbtInd;
            } else if(type == "CLBD") {
                closingBalance.amount = amount;
                closingBalance.currency = currency;
                closingBalance.date = date;
                closingBalance.creditline = creditline;
                closingBalance.cdtlineincl = cdtlineincl;
                closingBalance.cdtdbtind = cdtDbtInd;
            }
        }
        
        // Transactions
        for (auto* entry = doc.first_node("Document")->first_node("BkToCstmrStmt")->first_node("Stmt")->first_node("Ntry"); entry; entry = entry->next_sibling("Ntry")) {
            Transaction t;
            auto* amtNode = entry->first_node("Amt");
            if (amtNode) {
                t.amount = amtNode->value();
                auto* curr = amtNode->first_attribute("Ccy");
                t.currency = curr ? curr->value() : "";
            }
            auto* cdtDbtInd = entry->first_node("CdtDbtInd");
            if (cdtDbtInd)  t.type = cdtDbtInd->value();
            
            //booking date or value date ??
            auto* bookDt = entry->first_node("BookgDt");
            if (bookDt && bookDt->first_node("Dt")) t.date = bookDt->first_node("Dt")->value();

            if(auto nn = xml_doc_get_node(entry, "BkTxCd/Domn/Fmly/Cd")) t.familycd = nn->value();

            if(auto nn = xml_doc_get_node(entry, "BkTxCd/Domn/Fmly/SubFmlyCd")) t.subfamilycd = nn->value();

            if(auto nn = xml_doc_get_node(entry, "BkTxCd/Prtry/Cd")) t.prtrycd = nn->value();

            auto* txDtls = entry->first_node("NtryDtls");
            if (txDtls) {
                auto* btch = txDtls->first_node("Btch");
                if (btch) {
                    //MsgId
                    t.batchmsgid = btch->first_node("MsgId") ? btch->first_node("MsgId")->value() : "";
                    //PmtInfId
                    t.batchpmtinfid = btch->first_node("PmtInfId") ? btch->first_node("PmtInfId")->value() : "";
                    //NbOfTxs
                    t.batchnboftxs = btch->first_node("NbOfTxs") ? btch->first_node("NbOfTxs")->value() : "";
                }
                auto* tx = txDtls->first_node("TxDtls");
                if (tx) {
                    auto* refs = tx->first_node("Refs");

                    if (refs && refs->first_node("InstrId")) t.instrid = refs->first_node("InstrId")->value();
                    if (refs && refs->first_node("EndToEndId")) t.endtoendid = refs->first_node("EndToEndId")->value();

                    auto* amtdtls = tx->first_node("AmtDtls");
                    if (amtdtls && amtdtls->first_node("TxAmt")->first_node("Amt")) t.amtdtlsamt = amtdtls->first_node("TxAmt")->first_node("Amt")->value();

                    auto* rltdts = tx->first_node("RltdDts");
                    if (rltdts && rltdts->first_node("AccptncDtTm")) t.acceptancedate = rltdts->first_node("AccptncDtTm")->value();

                    auto* rmtInf = tx->first_node("RmtInf");
                    if (rmtInf && rmtInf->first_node("Ustrd")) t.description = rmtInf->first_node("Ustrd")->value();
                    auto* rltdPties = tx->first_node("RltdPties");
                    if (rltdPties) {
                        auto* dbtr = rltdPties->first_node("Dbtr");
                        if (dbtr && dbtr->first_node("Nm")) t.debtor = dbtr->first_node("Nm")->value();

                        if(auto nn = xml_doc_get_node(dbtr, "Id/OrgId/Othr/Id")) t.dbtrorigid = nn->value();

                        auto* cdtr = rltdPties->first_node("Cdtr");
                        if (cdtr && cdtr->first_node("Nm")) t.creditor = cdtr->first_node("Nm")->value();
                    }
                }
            }
            //LOG(INFO) << "Transaction: " << t.amount << " " << t.currency << " " << t.date << " " << t.type << " Desc: " << t.description;
            transactions.push_back(t);
        }
        return true;
    } catch (...) {
        LOG(ERROR) << "Exception while parsing camt.053.001.02 XML content";
        return false;
    }
}

const std::vector<Camt05300102::Transaction>& Camt05300102::getTransactions() const {
    return transactions;
}

std::string Camt05300102::getAccount() const {
    return account;
}

std::string Camt05300102::getStatementId() const {
    return statementId;
}

std::string Camt05300102::getDate() const {
    return date;
}
