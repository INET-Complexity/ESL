/// \file   currencies.cpp
///
/// \brief  Exports the currencies to python.
///
/// \authors    Maarten P. Scholl
/// \date       2018-02-04
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking,
///             Oxford Martin School, University of Oxford
///
///             Licensed under the Apache License, Version 2.0 (the "License");
///             you may not use this file except in compliance with the License.
///             You may obtain a copy of the License at
///
///                 http://www.apache.org/licenses/LICENSE-2.0
///
///             Unless required by applicable law or agreed to in writing,
///             software distributed under the License is distributed on an "AS
///             IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
///             express or implied. See the License for the specific language
///             governing permissions and limitations under the License.
///
///             You may obtain instructions to fulfill the attribution
///             requirements in CITATION.cff
///
#include <esl/economics/currencies.hpp>

#ifdef WITH_PYTHON
#include <boost/python.hpp>


using namespace boost::python;
BOOST_PYTHON_MODULE(currencies)
{
    scope().attr("AED") = esl::economics::currencies::AED;
    scope().attr("AFN") = esl::economics::currencies::AFN;
    scope().attr("ALL") = esl::economics::currencies::ALL;
    scope().attr("AMD") = esl::economics::currencies::AMD;
    scope().attr("ANG") = esl::economics::currencies::ANG;
    scope().attr("AOA") = esl::economics::currencies::AOA;
    scope().attr("ARS") = esl::economics::currencies::ARS;
    scope().attr("AUD") = esl::economics::currencies::AUD;
    scope().attr("AWG") = esl::economics::currencies::AWG;
    scope().attr("AZN") = esl::economics::currencies::AZN;
    scope().attr("BAM") = esl::economics::currencies::BAM;
    scope().attr("BBD") = esl::economics::currencies::BBD;
    scope().attr("BDT") = esl::economics::currencies::BDT;
    scope().attr("BGN") = esl::economics::currencies::BGN;
    scope().attr("BHD") = esl::economics::currencies::BHD;
    scope().attr("BIF") = esl::economics::currencies::BIF;
    scope().attr("BMD") = esl::economics::currencies::BMD;
    scope().attr("BND") = esl::economics::currencies::BND;
    scope().attr("BOB") = esl::economics::currencies::BOB;
    scope().attr("BOV") = esl::economics::currencies::BOV;
    scope().attr("BRL") = esl::economics::currencies::BRL;
    scope().attr("BSD") = esl::economics::currencies::BSD;
    scope().attr("BTN") = esl::economics::currencies::BTN;
    scope().attr("BWP") = esl::economics::currencies::BWP;
    scope().attr("BYN") = esl::economics::currencies::BYN;
    scope().attr("BZD") = esl::economics::currencies::BZD;
    scope().attr("CAD") = esl::economics::currencies::CAD;
    scope().attr("CDF") = esl::economics::currencies::CDF;
    scope().attr("CHE") = esl::economics::currencies::CHE;
    scope().attr("CHF") = esl::economics::currencies::CHF;
    scope().attr("CHW") = esl::economics::currencies::CHW;
    scope().attr("CLF") = esl::economics::currencies::CLF;
    scope().attr("CLP") = esl::economics::currencies::CLP;
    scope().attr("CNY") = esl::economics::currencies::CNY;
    scope().attr("COP") = esl::economics::currencies::COP;
    scope().attr("COU") = esl::economics::currencies::COU;
    scope().attr("CRC") = esl::economics::currencies::CRC;
    scope().attr("CUC") = esl::economics::currencies::CUC;
    scope().attr("CUP") = esl::economics::currencies::CUP;
    scope().attr("CVE") = esl::economics::currencies::CVE;
    scope().attr("CZK") = esl::economics::currencies::CZK;
    scope().attr("DJF") = esl::economics::currencies::DJF;
    scope().attr("DKK") = esl::economics::currencies::DKK;
    scope().attr("DOP") = esl::economics::currencies::DOP;
    scope().attr("DZD") = esl::economics::currencies::DZD;
    scope().attr("EGP") = esl::economics::currencies::EGP;
    scope().attr("ERN") = esl::economics::currencies::ERN;
    scope().attr("ETB") = esl::economics::currencies::ETB;
    scope().attr("EUR") = esl::economics::currencies::EUR;
    scope().attr("FJD") = esl::economics::currencies::FJD;
    scope().attr("FKP") = esl::economics::currencies::FKP;
    scope().attr("GBP") = esl::economics::currencies::GBP;
    scope().attr("GEL") = esl::economics::currencies::GEL;
    scope().attr("GHS") = esl::economics::currencies::GHS;
    scope().attr("GIP") = esl::economics::currencies::GIP;
    scope().attr("GMD") = esl::economics::currencies::GMD;
    scope().attr("GNF") = esl::economics::currencies::GNF;
    scope().attr("GTQ") = esl::economics::currencies::GTQ;
    scope().attr("GYD") = esl::economics::currencies::GYD;
    scope().attr("HKD") = esl::economics::currencies::HKD;
    scope().attr("HNL") = esl::economics::currencies::HNL;
    scope().attr("HRK") = esl::economics::currencies::HRK;
    scope().attr("HTG") = esl::economics::currencies::HTG;
    scope().attr("HUF") = esl::economics::currencies::HUF;
    scope().attr("IDR") = esl::economics::currencies::IDR;
    scope().attr("ILS") = esl::economics::currencies::ILS;
    scope().attr("INR") = esl::economics::currencies::INR;
    scope().attr("IQD") = esl::economics::currencies::IQD;
    scope().attr("IRR") = esl::economics::currencies::IRR;
    scope().attr("ISK") = esl::economics::currencies::ISK;
    scope().attr("JMD") = esl::economics::currencies::JMD;
    scope().attr("JOD") = esl::economics::currencies::JOD;
    scope().attr("JPY") = esl::economics::currencies::JPY;
    scope().attr("KES") = esl::economics::currencies::KES;
    scope().attr("KGS") = esl::economics::currencies::KGS;
    scope().attr("KHR") = esl::economics::currencies::KHR;
    scope().attr("KMF") = esl::economics::currencies::KMF;
    scope().attr("KPW") = esl::economics::currencies::KPW;
    scope().attr("KRW") = esl::economics::currencies::KRW;
    scope().attr("KWD") = esl::economics::currencies::KWD;
    scope().attr("KYD") = esl::economics::currencies::KYD;
    scope().attr("KZT") = esl::economics::currencies::KZT;
    scope().attr("LAK") = esl::economics::currencies::LAK;
    scope().attr("LBP") = esl::economics::currencies::LBP;
    scope().attr("LKR") = esl::economics::currencies::LKR;
    scope().attr("LRD") = esl::economics::currencies::LRD;
    scope().attr("LSL") = esl::economics::currencies::LSL;
    scope().attr("LYD") = esl::economics::currencies::LYD;
    scope().attr("MAD") = esl::economics::currencies::MAD;
    scope().attr("MDL") = esl::economics::currencies::MDL;
    scope().attr("MGA") = esl::economics::currencies::MGA;
    scope().attr("MKD") = esl::economics::currencies::MKD;
    scope().attr("MMK") = esl::economics::currencies::MMK;
    scope().attr("MNT") = esl::economics::currencies::MNT;
    scope().attr("MOP") = esl::economics::currencies::MOP;
    scope().attr("MRU") = esl::economics::currencies::MRU;
    scope().attr("MUR") = esl::economics::currencies::MUR;
    scope().attr("MVR") = esl::economics::currencies::MVR;
    scope().attr("MWK") = esl::economics::currencies::MWK;
    scope().attr("MXN") = esl::economics::currencies::MXN;
    scope().attr("MXV") = esl::economics::currencies::MXV;
    scope().attr("MYR") = esl::economics::currencies::MYR;
    scope().attr("MZN") = esl::economics::currencies::MZN;
    scope().attr("NAD") = esl::economics::currencies::NAD;
    scope().attr("NGN") = esl::economics::currencies::NGN;
    scope().attr("NIO") = esl::economics::currencies::NIO;
    scope().attr("NOK") = esl::economics::currencies::NOK;
    scope().attr("NPR") = esl::economics::currencies::NPR;
    scope().attr("NZD") = esl::economics::currencies::NZD;
    scope().attr("OMR") = esl::economics::currencies::OMR;
    scope().attr("PAB") = esl::economics::currencies::PAB;
    scope().attr("PEN") = esl::economics::currencies::PEN;
    scope().attr("PGK") = esl::economics::currencies::PGK;
    scope().attr("PHP") = esl::economics::currencies::PHP;
    scope().attr("PKR") = esl::economics::currencies::PKR;
    scope().attr("PLN") = esl::economics::currencies::PLN;
    scope().attr("PYG") = esl::economics::currencies::PYG;
    scope().attr("QAR") = esl::economics::currencies::QAR;
    scope().attr("RON") = esl::economics::currencies::RON;
    scope().attr("RSD") = esl::economics::currencies::RSD;
    scope().attr("RUB") = esl::economics::currencies::RUB;
    scope().attr("RWF") = esl::economics::currencies::RWF;
    scope().attr("SAR") = esl::economics::currencies::SAR;
    scope().attr("SBD") = esl::economics::currencies::SBD;
    scope().attr("SCR") = esl::economics::currencies::SCR;
    scope().attr("SDG") = esl::economics::currencies::SDG;
    scope().attr("SEK") = esl::economics::currencies::SEK;
    scope().attr("SGD") = esl::economics::currencies::SGD;
    scope().attr("SHP") = esl::economics::currencies::SHP;
    scope().attr("SLL") = esl::economics::currencies::SLL;
    scope().attr("SOS") = esl::economics::currencies::SOS;
    scope().attr("SRD") = esl::economics::currencies::SRD;
    scope().attr("SSP") = esl::economics::currencies::SSP;
    scope().attr("STN") = esl::economics::currencies::STN;
    scope().attr("SVC") = esl::economics::currencies::SVC;
    scope().attr("SYP") = esl::economics::currencies::SYP;
    scope().attr("SZL") = esl::economics::currencies::SZL;
    scope().attr("THB") = esl::economics::currencies::THB;
    scope().attr("TJS") = esl::economics::currencies::TJS;
    scope().attr("TMT") = esl::economics::currencies::TMT;
    scope().attr("TND") = esl::economics::currencies::TND;
    scope().attr("TOP") = esl::economics::currencies::TOP;
    scope().attr("TRY") = esl::economics::currencies::TRY;
    scope().attr("TTD") = esl::economics::currencies::TTD;
    scope().attr("TWD") = esl::economics::currencies::TWD;
    scope().attr("TZS") = esl::economics::currencies::TZS;
    scope().attr("UAH") = esl::economics::currencies::UAH;
    scope().attr("UGX") = esl::economics::currencies::UGX;
    scope().attr("USD") = esl::economics::currencies::USD;
    scope().attr("USN") = esl::economics::currencies::USN;
    scope().attr("UYI") = esl::economics::currencies::UYI;
    scope().attr("UYU") = esl::economics::currencies::UYU;
    scope().attr("UZS") = esl::economics::currencies::UZS;
    scope().attr("VEF") = esl::economics::currencies::VEF;
    scope().attr("VND") = esl::economics::currencies::VND;
    scope().attr("VUV") = esl::economics::currencies::VUV;
    scope().attr("WST") = esl::economics::currencies::WST;
    scope().attr("XAF") = esl::economics::currencies::XAF;
    scope().attr("XAG") = esl::economics::currencies::XAG;
    scope().attr("XAU") = esl::economics::currencies::XAU;
    scope().attr("XBA") = esl::economics::currencies::XBA;
    scope().attr("XBB") = esl::economics::currencies::XBB;
    scope().attr("XBC") = esl::economics::currencies::XBC;
    scope().attr("XBD") = esl::economics::currencies::XBD;
    scope().attr("XCD") = esl::economics::currencies::XCD;
    scope().attr("XDR") = esl::economics::currencies::XDR;
    scope().attr("XOF") = esl::economics::currencies::XOF;
    scope().attr("XPD") = esl::economics::currencies::XPD;
    scope().attr("XPF") = esl::economics::currencies::XPF;
    scope().attr("XPT") = esl::economics::currencies::XPT;
    scope().attr("XSU") = esl::economics::currencies::XSU;
    scope().attr("XUA") = esl::economics::currencies::XUA;
    scope().attr("YER") = esl::economics::currencies::YER;
    scope().attr("ZAR") = esl::economics::currencies::ZAR;
    scope().attr("ZMW") = esl::economics::currencies::ZMW;
    scope().attr("ZWL") = esl::economics::currencies::ZWL;
}

#endif