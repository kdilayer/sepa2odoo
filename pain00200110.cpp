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

#include "pain00200110.h"
#include <rapidxml.hpp>
#include <fstream>
#include <sstream>

/*

EXAMPLE FEEDBACK XML

<?xml version='1.0' encoding='UTF-8'?>
<Document
	xmlns="urn:iso:std:iso:20022:tech:xsd:pain.002.001.10"
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
	<CstmrPmtStsRpt>
		<GrpHdr>
			<MsgId>07847577_1756915574024</MsgId>
			<CreDtTm>2025-09-03T19:06:14.024+03:00</CreDtTm>
			<InitgPty>
				<Nm>Comtec SMD Oy</Nm>
			</InitgPty>
			<DbtrAgt>
				<FinInstnId>
					<BICFI>ITELFIHH</BICFI>
				</FinInstnId>
			</DbtrAgt>
		</GrpHdr>
		<OrgnlGrpInfAndSts>
			<OrgnlMsgId>WSSEPA-MSGID-27557</OrgnlMsgId>
			<OrgnlMsgNmId>pain.001.001.09</OrgnlMsgNmId>
			<GrpSts>ACTC</GrpSts>
		</OrgnlGrpInfAndSts>
	</CstmrPmtStsRpt>
</Document>


*/
using namespace rapidxml;

bool Pain00200110::parseGroupStatus(const std::string& xmlContent, GroupStatus& status) {
    std::vector<char> xml_copy(xmlContent.begin(), xmlContent.end());
    xml_copy.push_back('\0');
    try {
        xml_document<> doc;
        doc.parse<0>(&xml_copy[0]);
        auto* cstmrPmtStsRpt = doc.first_node("Document") ? doc.first_node("Document")->first_node("CstmrPmtStsRpt") : nullptr;
        if (!cstmrPmtStsRpt) return false;
        auto* grpHdr = cstmrPmtStsRpt->first_node("GrpHdr");
        if (grpHdr) {
            auto* msgId = grpHdr->first_node("MsgId");
            if (msgId) status.msgId = msgId->value();
            auto* creDtTm = grpHdr->first_node("CreDtTm");
            if (creDtTm) status.creDtTm = creDtTm->value();
            auto* initgPty = grpHdr->first_node("InitgPty");
            if (initgPty && initgPty->first_node("Nm")) status.initgPtyNm = initgPty->first_node("Nm")->value();
            auto* dbtrAgt = grpHdr->first_node("DbtrAgt");
            if (dbtrAgt && dbtrAgt->first_node("FinInstnId") && dbtrAgt->first_node("FinInstnId")->first_node("BICFI"))
                status.bicfi = dbtrAgt->first_node("FinInstnId")->first_node("BICFI")->value();
        }
        auto* orgnlGrpInf = cstmrPmtStsRpt->first_node("OrgnlGrpInfAndSts");
        if (orgnlGrpInf) {
            auto* orgnlMsgId = orgnlGrpInf->first_node("OrgnlMsgId");
            if (orgnlMsgId) status.orgnlMsgId = orgnlMsgId->value();
            auto* orgnlMsgNmId = orgnlGrpInf->first_node("OrgnlMsgNmId");
            if (orgnlMsgNmId) status.orgnlMsgNmId = orgnlMsgNmId->value();
            auto* grpSts = orgnlGrpInf->first_node("GrpSts");
            if (grpSts) status.grpSts = grpSts->value();

            auto* stsRsnInf = orgnlGrpInf->first_node("StsRsnInf");
            if(stsRsnInf) {
                auto* rsn = stsRsnInf->first_node("Rsn");
                if(rsn) {
                    auto* prtry = rsn->first_node("Prtry");
                    if(prtry) {
                        // Proprietary reason for status
                        std::string reason = prtry->value();
                        status.addInfo = reason;
                    }
                }
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}
bool Pain00200110::parse(const std::string& xmlContent, GroupStatus &groupStatus) {
    
    if(parseGroupStatus(xmlContent, groupStatus)) {
        return true;
    }

    return false;

}
