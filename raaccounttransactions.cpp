#include "raaccounttransactions.h"
#include <sstream>
#include "util.h"


std::string makeUniqueFileIdAktia(std::map<std::string, int> table, std::string reference, std::string date, std::string amount) {
    std::string ustr = "" + reference + "" + date + "" + amount;
    if (table[ustr] == 0) {
        //does not exist in table yet
    } else {
        //exists in table
        int counter = 1;
        while (1) {
            //println "exists in table; set new ustr: " + ustr
            ustr = "" + reference + "" + date + "" + amount + "_" + std::to_string(counter++);
            if (table[ustr] == 0) {
                break;
            }
        }
    }
    table[ustr] = 1;
    return ustr;
}
bool RAAccountTransactions::parse(const std::string& content) {
    std::map<std::string, int> table;

    balance = stringToBigDecimalNumber(grabStringFromPositions(content, 53, 68));
    if (grabStringFromPositions(content, 68, 69) == "-") {
        balance = "-"+balance;
    }

    //handleAccountBalance(connection, balance);
    bool processedAny = false;
    std::istringstream iss(content);
    std::string line;
    int count = 0;

    while (std::getline(iss, line)) {
        // Remove trailing carriage return if present
        if (!line.empty() && line.back() == '\r') line.pop_back();
        // Logging stub: devLog("Line (" + std::to_string(count) + "): " + line);
        processedAny = true;
        if (count == 0) { // Header row
            // Logging stub: Currency, Account number, Date
            currency = grabStringFromPositions(line, 24, 25) == "1" ? "Eur" : "unknown";
            accountNumber = grabStringFromPositions(line, 25, 39);
            date = grabStringFromPositions(line, 39, 45);
            // devLog stubs can be added here
        } else if (count == 1) { // Balance row
            language = grabStringFromPositions(line, 0, 1);
            time = grabStringFromPositions(line, 1, 5);
            std::string balanceStr = grabStringFromPositions(line, 6, 21);
            std::string sign = grabStringFromPositions(line, 21, 22);
            std::string balagain = stringToBigDecimalNumber(balanceStr, false);
            if (sign == "-") balagain = "-" + balagain;
            // Logging stub: devLog("Balance: " + balance);
        } else { // Transaction row
            if (line.size() > 2 && grabStringFromPositions(line, 0, 1) == "0") {
                std::string code = grabStringFromPositions(line, 0, 1);
                std::string date = grabStringFromPositions(line, 1, 7); //ddMMyy
                std::string tosnum = grabStringFromPositions(line, 7, 9);
                std::string curr = grabStringFromPositions(line, 10, 11);
                std::string vientiselite = grabStringFromPositions(line, 11, 14);
                std::string amountStr = grabStringFromPositions(line, 14, 31);
                std::string amount = stringToBigDecimalNumber(amountStr, false);
                bool TILISIIRTO = (vientiselite == "017");
                std::string sign = grabStringFromPositions(line, 30, 31);
                if (sign == "-") amount = "-" + amount;
                std::string reference = grabStringFromPositions(line, 31, 51);
                std::string receiver = grabStringFromPositions(line, 51, 71);
                receiver.erase(0, receiver.find_first_not_of(" \t\n\r"));
                receiver.erase(receiver.find_last_not_of(" \t\n\r") + 1);
                if (receiver == "") {
                    receiver = "connection.bankName";
                }
                std::string vientiselite2 = grabStringFromPositions(line, 71, 83);

                std::string fileId = makeUniqueFileIdAktia(table, reference, date, amount);
                string_trim(receiver);
                
                if (reference[0] == '0') { //real ref number
                    //remove leading zeors from ref
                    reference.erase(0, reference.find_first_not_of('0'));
                    if (reference == "") reference = "0";
                } else {
                    //remove leading and trailing whitespace
                    string_trim(reference);
                }
                
                /*
                if (amount <= 0) {
                    handleWithDrawal(connection, receiver, date, amount, fileId, reference, "")
                } else if (TILISIIRTO) {
                    //we get the deposits from "OP"
                    //logger.debug("SKIPPED DEPOSIT (" +connection.bankBIC + "/"+connection.bankIBAN+"): date: "+date+", from: \'"+ owner+ "\', amount: \'"+amount +"\', fileid: \'"+fileId+"\'"  )
                    handleDeposit(connection, receiver, date, amount, fileId, reference, "")
                }*/

               //transactions.push_back({receiver, date, amount, fileId, reference});
            }
        }
        ++count;
    }
    return processedAny;
    /*
    transactions.clear();
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        Transaction tx;
        std::istringstream ls(line);
        // Example: id,date,amount,currency,description,account,counterparty
        std::getline(ls, tx.id, ',');
        std::getline(ls, tx.date, ',');
        std::getline(ls, tx.amount, ',');
        std::getline(ls, tx.currency, ',');
        std::getline(ls, tx.description, ',');
        std::getline(ls, tx.account, ',');
        std::getline(ls, tx.counterparty, ',');
        transactions.push_back(tx);
    }
    return !transactions.empty();
    */    
}

const std::vector<RAAccountTransactions::Transaction>& RAAccountTransactions::getTransactions() const {
    return transactions;
}

void RAAccountTransactions::addTransaction(const Transaction& tx) {
    transactions.push_back(tx);
}

void RAAccountTransactions::clear() {
    transactions.clear();
}
