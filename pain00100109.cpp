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
#include "pain00100109.h"
#include <ctime>
#include <rapidxml.hpp>
#include <fstream>
#include <sstream>
#include "util.h"
#include <random>

using namespace rapidxml;

std::string Pain00100109::generateTimestamp() {
	char buf[20];
	std::time_t t = std::time(nullptr);
	std::tm *tm = gmtime(&t);
#if defined(_WIN32) || defined(_WIN64)
	//localtime_s(&tm, &t);
#else
	//localtime_r(&t, &tm);
#endif
	std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tm);
	return std::string(buf);
}
char countControlCharacter(const std::string& perusosa) {
    int summa = 0;
    std::vector<int> painot = {7, 3, 1};
    int painoIndeksi = 0;

    // Käydään perusosa läpi oikealta vasemmalle
    for (int i = perusosa.length() - 1; i >= 0; --i) {
        int numero = perusosa[i] - '0'; // Muunnetaan merkki numeroksi
        summa += numero * painot[painoIndeksi % 3];
        painoIndeksi++;
    }

    int jannos = summa % 10; // Jakojäännös 10:llä
    if (jannos == 0) {
        return '0';
    } else {
        return (10 - jannos) + '0'; // Palautetaan tarkistusmerkki numerona
    }
}
bool isReferenceNumber(const std::string& ref) {
	if(ref.length() < 4 || ref.length() > 20) {
		return false;
	}
	if(string_startswith(ref, "RF")) {
		//ISO 11649
		if(ref.length() < 5 || ref.length() > 25) {
			return false;
		}
		std::string rearranged = ref.substr(4) + "RF" + ref.substr(2,2);
		std::string numeric_representation;
		for(char c : rearranged) {
			if(isdigit(c)) {
				numeric_representation += c;
			} else if(isalpha(c)) {
				int val = toupper(c) - 'A' + 10;
				numeric_representation += std::to_string(val);
			} else {
				return false; // Invalid character
			}
		}
		// Now perform mod-97 operation
		int remainder = 0;
		for(char c : numeric_representation) {
			remainder = (remainder * 10 + (c - '0')) % 97;
		}
		return remainder == 1;
	}
	for(char c : ref) {
		if(!isdigit(c)) {
			return false;
		}
	}
	std::string perusosa = ref.substr(0, ref.length() - 1);
	char tarkistusmerkki = ref.back();
	char laskettuTarkistusmerkki = countControlCharacter(perusosa);
	return tarkistusmerkki == laskettuTarkistusmerkki;
}
std::string Pain00100109::getRemittanceInfo(Payment &payment) {

	std::string refNumber;
	std::string refMessage;

	std::string memo_tmp = payment.payment_memo;
	if(!memo_tmp.empty() ){
		string_trim(memo_tmp);
		memo_tmp = string_remove_whitespace(memo_tmp);
	}
	if(isReferenceNumber(memo_tmp)) {
		refNumber = memo_tmp;
	}
	else if(isReferenceNumber(payment.payment_reference)) {
		refNumber = payment.payment_reference;
	}
	else {
		refMessage = payment.payment_memo;
		if(refMessage.empty()) {
			refMessage = payment.payment_reference;
		}
		if(refMessage.length() > 100) {
			refMessage = refMessage.substr(0, 100); // Max 100 characters
		}
	}
	if(refNumber.empty()) {
		//use message if no reference number
		return formattedString(R"(
				<RmtInf>
					<Ustrd>%s</Ustrd>
				</RmtInf>	
		)",
		refMessage.c_str());
	}
	return formattedString(R"(
				<RmtInf>
					<Strd>
						<CdtrRefInf>
							<Tp>
								<CdOrPrtry>
									<Cd>SCOR</Cd>
								</CdOrPrtry>
							</Tp>
							<Ref>%s</Ref>
						</CdtrRefInf>
					</Strd>
				</RmtInf>
		)",
	refNumber.c_str());
}

std::string Pain00100109::getTransfer_sepa_payment(Payment &payment) { 
    //SEPA payment
	//creditor info -> Party to which an amount of money is due. -> Seller
    std::string sellerAccountNumber = string_remove_whitespace(payment.SellerAccountNumber);

    return formattedString(R"(
			<CdtTrfTxInf>
				<PmtId>
					<InstrId>%s</InstrId>
					<EndToEndId>%s</EndToEndId>
				</PmtId>
				<Amt>
					<InstdAmt Ccy="%s">%s</InstdAmt>
				</Amt>
				<ChrgBr>SLEV</ChrgBr>
				<CdtrAgt>
					<FinInstnId>
						<BICFI>%s</BICFI>
					</FinInstnId>
				</CdtrAgt>
				<Cdtr>
					<Nm>%s</Nm>
					<PstlAdr>
						<Ctry>%s</Ctry>
					</PstlAdr>
				</Cdtr>
				<CdtrAcct>
					<Id>
						<IBAN>%s</IBAN>
					</Id>
				</CdtrAcct>
				%s
			</CdtTrfTxInf>
)",
payment.ins_id.c_str(),
payment.e2e_id.c_str(),
payment.currency.c_str(),
payment.amount_str.c_str(),
payment.SellerBic.c_str(),
payment.SellerOrganisationName.c_str(),
payment.SellerCountryCode.c_str(),
sellerAccountNumber.c_str(),
getRemittanceInfo(payment).c_str()


);
}
std::string Pain00100109::getTransfer_sepa_instant(Payment &payment) {
    //SEPA instant credit transfer 

    return formattedString(R"(
			<CdtTrfTxInf>
				<PmtId>
					<InstrId>DEF CCT002</InstrId>
					<EndToEndId>DEF 26062025</EndToEndId>
				</PmtId>
				<PmtTpInf>
					<InstrPrty>NORM</InstrPrty>
					<SvcLvl>
						<Cd>SEPA</Cd>
					</SvcLvl>
					<LclInstrm>
						<Cd>INST</Cd>
					</LclInstrm>
				</PmtTpInf>
				<Amt>
					<InstdAmt Ccy="EUR">2.20</InstdAmt>
				</Amt>
				<ChrgBr>SLEV</ChrgBr>
				<CdtrAgt>
					<FinInstnId>
						<BICFI>OKOYFIHH</BICFI>
					</FinInstnId>
				</CdtrAgt>
				<Cdtr>
					<Nm>Creditor name</Nm>
					<PstlAdr>
						<TwnNm>Town</TwnNm>
						<Ctry>FI</Ctry>
					</PstlAdr>
				</Cdtr>
				<CdtrAcct>
					<Id>
						<IBAN>FI6171307002020202</IBAN>
					</Id>
				</CdtrAcct>
				<RmtInf>
					<Strd>
						<CdtrRefInf>
							<Tp>
								<CdOrPrtry>
									<Cd>SCOR</Cd>
								</CdOrPrtry>
								<Issr>ISO</Issr>
							</Tp>
							<Ref>RF47892182916</Ref>
						</CdtrRefInf>
					</Strd>
				</RmtInf>
			</CdtTrfTxInf>
    )");
}
std::string Pain00100109::getTransfer_international(Payment &payment) {
    //SEPA payment
	//creditor info -> Party to which an amount of money is due. -> Seller
    std::string sellerAccountNumber = string_remove_whitespace(payment.SellerAccountNumber);

	std::string sellerAdrLine = payment.SellerStreetName + " " + payment.SellerTownName;
    std::string ret = formattedString(R"(
			<CdtTrfTxInf>
				<PmtId>
					<InstrId>%s</InstrId>
					<EndToEndId>%s</EndToEndId>
				</PmtId>
				<Amt>
					<InstdAmt Ccy="%s">%s</InstdAmt>
				</Amt>
				<ChrgBr>SLEV</ChrgBr>
				<CdtrAgt>
					<FinInstnId>
						<BICFI>%s</BICFI>
						<Nm>%s</Nm>
						<PstlAdr>
							<TwnNm>%s</TwnNm>
							<Ctry>%s</Ctry>
						</PstlAdr>
					</FinInstnId>
				</CdtrAgt>
				<Cdtr>
					<Nm>%s</Nm>
					<PstlAdr>
						<Ctry>%s</Ctry>
						<AdrLine>%s</AdrLine>
					</PstlAdr>
				</Cdtr>
				<CdtrAcct>
					<Id>
						<Othr>
							<Id>%s</Id>
						</Othr>
					</Id>
				</CdtrAcct>
				%s
			</CdtTrfTxInf>
)",
payment.ins_id.c_str(),
payment.e2e_id.c_str(),
payment.currency.c_str(),
payment.amount_str.c_str(),
payment.SellerBic.c_str(),
payment.SellerBankName.c_str(),
payment.SellerBankTownName.c_str(),
payment.SellerBankCountryCode.c_str(),
payment.SellerOrganisationName.c_str(),

payment.SellerCountryCode.c_str(),
sellerAdrLine.c_str(),
sellerAccountNumber.c_str(),
getRemittanceInfo(payment).c_str()


);
	return ret;
	/*
	// International payment order 
    return formattedString(R"(
			<CdtTrfTxInf>
				<PmtId>
					<InstrId>GHI CCT003</InstrId>
					<EndToEndId>GHI 26062025</EndToEndId>
				</PmtId>
				<PmtTpInf>
					<InstrPrty>NORM</InstrPrty>
					<SvcLvl>
						<Cd>NURG</Cd>
					</SvcLvl>
				</PmtTpInf>
				<Amt>
					<InstdAmt Ccy="GBP">3.30</InstdAmt>
				</Amt>
				<XchgRateInf>
					<CtrctId>FF00123123</CtrctId>
				</XchgRateInf>
				<ChrgBr>SHAR</ChrgBr>
				<CdtrAgt>
					<FinInstnId>
						<BICFI>IRVTUS3N</BICFI>
					</FinInstnId>
				</CdtrAgt>
				<Cdtr>
					<Nm>Creditor name</Nm>
					<PstlAdr>
						<StrtNm>Street name</StrtNm>
						<TwnNm>Town</TwnNm>
						<Ctry>GB</Ctry>
					</PstlAdr>
				</Cdtr>
				<CdtrAcct>
					<Id>
						<Othr>
							<Id>1231231</Id>
						</Othr>
					</Id>
				</CdtrAcct>
				<RmtInf>
					<Ustrd>Message</Ustrd>
				</RmtInf>
			</CdtTrfTxInf>
)");
	*/
}
std::string Pain00100109::getHeader(Payment &payment) {
	

    return formattedString(R"(<GrpHdr>
			<MsgId>%s</MsgId>
			<CreDtTm>%s</CreDtTm>
			<NbOfTxs>1</NbOfTxs>
			<CtrlSum>%s</CtrlSum> 
			<InitgPty>
				<Nm>%s</Nm>
				<PstlAdr>
					<Ctry>%s</Ctry>
				</PstlAdr>
			</InitgPty>
		</GrpHdr>
        )",
        payment.msg_id.c_str(),
        payment.timestamp.c_str(),
        payment.amount_str.c_str(),
        payment.BuyerOrganisationName.c_str(),
        payment.BuyerCountryCode.c_str());
}

std::string Pain00100109::getDebitorInfo(Payment &payment) {

    std::string buyerOrgScheme = payment.BuyerOrganisationTaxCode;
    std::string f2 = getFirstTwoChars(buyerOrgScheme);
    if(isalpha(f2[0])) { //international version like FIXXX
        buyerOrgScheme = buyerOrgScheme.substr(2);
    }
    string_replaceall(buyerOrgScheme, "-", "");
    buyerOrgScheme="0"+buyerOrgScheme;


    std::string dbtraccount = payment.BuyerAccountNumber;
    dbtraccount = string_remove_whitespace(dbtraccount);
	/*
	removed 
				<PstlAdr>
					<Ctry>%s</Ctry>
				</PstlAdr>

	
	*/
    return formattedString(R"(
            <Dbtr>
				<Nm>%s</Nm>
				<Id>
					<OrgId>
						<Othr>
							<Id>%s</Id>
							<SchmeNm>
								<Cd>BANK</Cd>
							</SchmeNm>
						</Othr>
					</OrgId>
				</Id>
			</Dbtr>
			<DbtrAcct>
				<Id>
					<IBAN>%s</IBAN>
				</Id>
			</DbtrAcct>
			<DbtrAgt>
				<FinInstnId>
					<BICFI>%s</BICFI>
				</FinInstnId>
			</DbtrAgt>
)",
    payment.BuyerOrganisationName.c_str(),
   // payment.BuyerCountry.c_str(),
buyerOrgScheme.c_str(),
dbtraccount.c_str(),
payment.BuyerBic.c_str()

);
}
// Convert a date string in format YYYY-MM-DD to a time_t timestamp (UTC)
time_t dateStringToTimestamp(const std::string& dateStr) {
	if (dateStr.size() != 10 || dateStr[4] != '-' || dateStr[7] != '-') return -1;
	std::tm tm = {};
	tm.tm_year = std::stoi(dateStr.substr(0,4)) - 1900;
	tm.tm_mon  = std::stoi(dateStr.substr(5,2)) - 1;
	tm.tm_mday = std::stoi(dateStr.substr(8,2));
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	tm.tm_isdst = -1;
	return timegm(&tm); // Use timegm for UTC, mktime for local time
}

std::string Pain00100109::getPmtInf(Payment &payment) {

	// Ensure execution date is not in the past
	std::string execution_date = payment.execution_date;
	time_t ex_date = dateStringToTimestamp(execution_date); //format YYYY-MM-DD
	time_t now = time(nullptr);

	if(ex_date < now) {
		// If execution date is in the past, set it to today
		std::tm *tm_now = gmtime(&now);
		char buf[11];
		std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_now);
		execution_date = std::string(buf);
	}
	/*
	//note changed from cd to code as instructed in 
	https://samlink.fi/app/uploads/2024/02/C2PSP-Lahtevat-maksut-pain.001.001.09-WEB_final.pdf
	<SvcLvl>
		<Cd>SEPA</Cd>
	</SvcLvl>

	removed 
			<PmtTpInf>
				<InstrPrty>NORM</InstrPrty>
				<SvcLvl>
					<Code>SEPA</Code>
				</SvcLvl>
			</PmtTpInf>
 
	*/
   	return formattedString(R"(
            <PmtInfId>%s</PmtInfId>
			<PmtMtd>TRF</PmtMtd>
			<CtrlSum>%s</CtrlSum>
			<ReqdExctnDt>
				<Dt>%s</Dt>
			</ReqdExctnDt>
        )",
    payment.pmt_id.c_str(),
    payment.amount_str.c_str(),
    execution_date.c_str());
}
std::string generateRandomHexString(size_t length = 10) {
	static const char hex_chars[] = "0123456789ABCDEF";
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 15);
	std::string result;
	result.reserve(length);
	for (size_t i = 0; i < length; ++i) {
		result += hex_chars[dis(gen)];
	}
	return result;
}

std::string Pain00100109::generateXml(Payment &payment) {

	if(payment.currency.empty())
		payment.currency = "EUR"; 

    if(payment.unique_id.empty()) {
        payment.unique_id = generateRandomHexString(10); //std::to_string(time(nullptr));
    }
	payment.e2e_id = "WSE2EID" + payment.unique_id;
    payment.pmt_id = "WSPMTID" + payment.unique_id;
	payment.ins_id = "WSINSID" + payment.unique_id;
	payment.msg_id = "WSMSGID" + payment.unique_id;

    if(payment.timestamp.empty()) {
        payment.timestamp = generateTimestamp(); //"2025-09-03T15:53:58"
    }
    std::string transfer = getTransfer_sepa_payment(payment);

	if(string_equals_ignore_case(payment.currency, "EUR") == false) {
		//std::cout <<"NORMAL \n"<<transfer<<"\n";
		transfer = getTransfer_international(payment);
		//std::cout <<"INTERNATIONAL \n"<<transfer<<"\n";
	}
    std::string formattedXml = formattedString(R"(<Document
	xmlns="urn:iso:std:iso:20022:tech:xsd:pain.001.001.09"
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="urn:iso:std:iso:20022:tech:xsd:pain.001.001.09 pain.001.001.09.xsd">
	<CstmrCdtTrfInitn>
		%s
		<PmtInf>
			%s
			%s
			%s
		</PmtInf>
	</CstmrCdtTrfInitn>
</Document>)",
getHeader(payment).c_str() ,
getPmtInf(payment).c_str(),
getDebitorInfo(payment).c_str(),
transfer.c_str());

    //LOG(DEBUG) << "Generated XML: " << formattedXml;
    return formattedXml;
}
