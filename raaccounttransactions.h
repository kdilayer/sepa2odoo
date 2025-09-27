#pragma once
#include <string>
#include <vector>

class RAAccountTransactions {
public:

    std::string balance;
    std::string currency;
    std::string accountNumber;  
    std::string date; //YYYYMMDD ??
    std::string language;
    std::string time; //HHMM


    struct Transaction {
        std::string id;
        std::string date;
        std::string amount;
        std::string currency;
        std::string description;
        std::string account;
        std::string counterparty;
    };

    RAAccountTransactions() = default;
    bool parse(const std::string& content);
    const std::vector<Transaction>& getTransactions() const;
    void addTransaction(const Transaction& tx);
    void clear();

private:
    std::vector<Transaction> transactions;
};
