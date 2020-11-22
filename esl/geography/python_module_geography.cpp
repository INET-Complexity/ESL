/// \file   python_module_geography.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-11-09
/// \copyright  Copyright 2017-2020 The Institute for New Economic Thinking,
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
#include <esl/geography/python_module_geography.hpp>
#include <esl/geography/countries.hpp>

#ifdef WITH_PYTHON
#include <string>
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
using namespace boost::python;

std::string python_country_code(const esl::geography::iso_3166_1_alpha_2 &c)
{
    return (std::string() + c.code[0]) + c.code[1];
}

BOOST_PYTHON_MODULE(_geography)
{
    class_<esl::geography::iso_3166_1_alpha_2>("iso_3166_1_alpha_2")
        .add_property("code", python_country_code)
        .add_property("__repr__", python_country_code)
        .add_property("__str__", python_country_code)
        ;

    scope().attr("AE") = esl::geography::countries::AE;
    scope().attr("AF") = esl::geography::countries::AF;
    scope().attr("AG") = esl::geography::countries::AG;
    scope().attr("AI") = esl::geography::countries::AI;
    scope().attr("AL") = esl::geography::countries::AL;
    scope().attr("AM") = esl::geography::countries::AM;
    scope().attr("AO") = esl::geography::countries::AO;
    scope().attr("AQ") = esl::geography::countries::AQ;
    scope().attr("AR") = esl::geography::countries::AR;
    scope().attr("AS") = esl::geography::countries::AS;
    scope().attr("AT") = esl::geography::countries::AT;
    scope().attr("AU") = esl::geography::countries::AU;
    scope().attr("AW") = esl::geography::countries::AW;
    scope().attr("AX") = esl::geography::countries::AX;
    scope().attr("AZ") = esl::geography::countries::AZ;
    scope().attr("BA") = esl::geography::countries::BA;
    scope().attr("BB") = esl::geography::countries::BB;
    scope().attr("BD") = esl::geography::countries::BD;
    scope().attr("BE") = esl::geography::countries::BE;
    scope().attr("BF") = esl::geography::countries::BF;
    scope().attr("BG") = esl::geography::countries::BG;
    scope().attr("BH") = esl::geography::countries::BH;
    scope().attr("BI") = esl::geography::countries::BI;
    scope().attr("BJ") = esl::geography::countries::BJ;
    scope().attr("BL") = esl::geography::countries::BL;
    scope().attr("BM") = esl::geography::countries::BM;
    scope().attr("BN") = esl::geography::countries::BN;
    scope().attr("BO") = esl::geography::countries::BO;
    scope().attr("BQ") = esl::geography::countries::BQ;
    scope().attr("BR") = esl::geography::countries::BR;
    scope().attr("BS") = esl::geography::countries::BS;
    scope().attr("BT") = esl::geography::countries::BT;
    scope().attr("BV") = esl::geography::countries::BV;
    scope().attr("BW") = esl::geography::countries::BW;
    scope().attr("BY") = esl::geography::countries::BY;
    scope().attr("BZ") = esl::geography::countries::BZ;
    scope().attr("CA") = esl::geography::countries::CA;
    scope().attr("CC") = esl::geography::countries::CC;
    scope().attr("CD") = esl::geography::countries::CD;
    scope().attr("CF") = esl::geography::countries::CF;
    scope().attr("CG") = esl::geography::countries::CG;
    scope().attr("CH") = esl::geography::countries::CH;
    scope().attr("CI") = esl::geography::countries::CI;
    scope().attr("CK") = esl::geography::countries::CK;
    scope().attr("CL") = esl::geography::countries::CL;
    scope().attr("CM") = esl::geography::countries::CM;
    scope().attr("CN") = esl::geography::countries::CN;
    scope().attr("CO") = esl::geography::countries::CO;
    scope().attr("CR") = esl::geography::countries::CR;
    scope().attr("CU") = esl::geography::countries::CU;
    scope().attr("CV") = esl::geography::countries::CV;
    scope().attr("CW") = esl::geography::countries::CW;
    scope().attr("CX") = esl::geography::countries::CX;
    scope().attr("CY") = esl::geography::countries::CY;
    scope().attr("CZ") = esl::geography::countries::CZ;
    scope().attr("DE") = esl::geography::countries::DE;
    scope().attr("DJ") = esl::geography::countries::DJ;
    scope().attr("DK") = esl::geography::countries::DK;
    scope().attr("DM") = esl::geography::countries::DM;
    scope().attr("DO") = esl::geography::countries::DO;
    scope().attr("DZ") = esl::geography::countries::DZ;
    scope().attr("EC") = esl::geography::countries::EC;
    scope().attr("EE") = esl::geography::countries::EE;
    scope().attr("EG") = esl::geography::countries::EG;
    scope().attr("EH") = esl::geography::countries::EH;
    scope().attr("ER") = esl::geography::countries::ER;
    scope().attr("ES") = esl::geography::countries::ES;
    scope().attr("ET") = esl::geography::countries::ET;
    scope().attr("FI") = esl::geography::countries::FI;
    scope().attr("FJ") = esl::geography::countries::FJ;
    scope().attr("FK") = esl::geography::countries::FK;
    scope().attr("FM") = esl::geography::countries::FM;
    scope().attr("FO") = esl::geography::countries::FO;
    scope().attr("FR") = esl::geography::countries::FR;
    scope().attr("GA") = esl::geography::countries::GA;
    scope().attr("GB") = esl::geography::countries::GB;
    scope().attr("GD") = esl::geography::countries::GD;
    scope().attr("GE") = esl::geography::countries::GE;
    scope().attr("GF") = esl::geography::countries::GF;
    scope().attr("GG") = esl::geography::countries::GG;
    scope().attr("GH") = esl::geography::countries::GH;
    scope().attr("GI") = esl::geography::countries::GI;
    scope().attr("GL") = esl::geography::countries::GL;
    scope().attr("GM") = esl::geography::countries::GM;
    scope().attr("GN") = esl::geography::countries::GN;
    scope().attr("GP") = esl::geography::countries::GP;
    scope().attr("GQ") = esl::geography::countries::GQ;
    scope().attr("GR") = esl::geography::countries::GR;
    scope().attr("GS") = esl::geography::countries::GS;
    scope().attr("GT") = esl::geography::countries::GT;
    scope().attr("GU") = esl::geography::countries::GU;
    scope().attr("GW") = esl::geography::countries::GW;
    scope().attr("GY") = esl::geography::countries::GY;
    scope().attr("HK") = esl::geography::countries::HK;
    scope().attr("HM") = esl::geography::countries::HM;
    scope().attr("HN") = esl::geography::countries::HN;
    scope().attr("HR") = esl::geography::countries::HR;
    scope().attr("HT") = esl::geography::countries::HT;
    scope().attr("HU") = esl::geography::countries::HU;
    scope().attr("ID") = esl::geography::countries::ID;
    scope().attr("IE") = esl::geography::countries::IE;
    scope().attr("IL") = esl::geography::countries::IL;
    scope().attr("IM") = esl::geography::countries::IM;
    scope().attr("IN") = esl::geography::countries::IN;
    scope().attr("IO") = esl::geography::countries::IO;
    scope().attr("IQ") = esl::geography::countries::IQ;
    scope().attr("IR") = esl::geography::countries::IR;
    scope().attr("IS") = esl::geography::countries::IS;
    scope().attr("IT") = esl::geography::countries::IT;
    scope().attr("JE") = esl::geography::countries::JE;
    scope().attr("JM") = esl::geography::countries::JM;
    scope().attr("JO") = esl::geography::countries::JO;
    scope().attr("JP") = esl::geography::countries::JP;
    scope().attr("KE") = esl::geography::countries::KE;
    scope().attr("KG") = esl::geography::countries::KG;
    scope().attr("KH") = esl::geography::countries::KH;
    scope().attr("KI") = esl::geography::countries::KI;
    scope().attr("KM") = esl::geography::countries::KM;
    scope().attr("KN") = esl::geography::countries::KN;
    scope().attr("KP") = esl::geography::countries::KP;
    scope().attr("KR") = esl::geography::countries::KR;
    scope().attr("KW") = esl::geography::countries::KW;
    scope().attr("KY") = esl::geography::countries::KY;
    scope().attr("KZ") = esl::geography::countries::KZ;
    scope().attr("LA") = esl::geography::countries::LA;
    scope().attr("LB") = esl::geography::countries::LB;
    scope().attr("LC") = esl::geography::countries::LC;
    scope().attr("LI") = esl::geography::countries::LI;
    scope().attr("LK") = esl::geography::countries::LK;
    scope().attr("LR") = esl::geography::countries::LR;
    scope().attr("LS") = esl::geography::countries::LS;
    scope().attr("LT") = esl::geography::countries::LT;
    scope().attr("LU") = esl::geography::countries::LU;
    scope().attr("LV") = esl::geography::countries::LV;
    scope().attr("LY") = esl::geography::countries::LY;
    scope().attr("MA") = esl::geography::countries::MA;
    scope().attr("MC") = esl::geography::countries::MC;
    scope().attr("MD") = esl::geography::countries::MD;
    scope().attr("ME") = esl::geography::countries::ME;
    scope().attr("MF") = esl::geography::countries::MF;
    scope().attr("MG") = esl::geography::countries::MG;
    scope().attr("MH") = esl::geography::countries::MH;
    scope().attr("MK") = esl::geography::countries::MK;
    scope().attr("ML") = esl::geography::countries::ML;
    scope().attr("MM") = esl::geography::countries::MM;
    scope().attr("MN") = esl::geography::countries::MN;
    scope().attr("MO") = esl::geography::countries::MO;
    scope().attr("MP") = esl::geography::countries::MP;
    scope().attr("MQ") = esl::geography::countries::MQ;
    scope().attr("MR") = esl::geography::countries::MR;
    scope().attr("MS") = esl::geography::countries::MS;
    scope().attr("MT") = esl::geography::countries::MT;
    scope().attr("MU") = esl::geography::countries::MU;
    scope().attr("MV") = esl::geography::countries::MV;
    scope().attr("MW") = esl::geography::countries::MW;
    scope().attr("MX") = esl::geography::countries::MX;
    scope().attr("MY") = esl::geography::countries::MY;
    scope().attr("MZ") = esl::geography::countries::MZ;
    scope().attr("NA") = esl::geography::countries::NA;
    scope().attr("NC") = esl::geography::countries::NC;
    scope().attr("NE") = esl::geography::countries::NE;
    scope().attr("NF") = esl::geography::countries::NF;
    scope().attr("NG") = esl::geography::countries::NG;
    scope().attr("NI") = esl::geography::countries::NI;
    scope().attr("NL") = esl::geography::countries::NL;
    scope().attr("NO") = esl::geography::countries::NO;
    scope().attr("NP") = esl::geography::countries::NP;
    scope().attr("NR") = esl::geography::countries::NR;
    scope().attr("NU") = esl::geography::countries::NU;
    scope().attr("NZ") = esl::geography::countries::NZ;
    scope().attr("OM") = esl::geography::countries::OM;
    scope().attr("PA") = esl::geography::countries::PA;
    scope().attr("PE") = esl::geography::countries::PE;
    scope().attr("PF") = esl::geography::countries::PF;
    scope().attr("PG") = esl::geography::countries::PG;
    scope().attr("PH") = esl::geography::countries::PH;
    scope().attr("PK") = esl::geography::countries::PK;
    scope().attr("PL") = esl::geography::countries::PL;
    scope().attr("PM") = esl::geography::countries::PM;
    scope().attr("PN") = esl::geography::countries::PN;
    scope().attr("PR") = esl::geography::countries::PR;
    scope().attr("PS") = esl::geography::countries::PS;
    scope().attr("PT") = esl::geography::countries::PT;
    scope().attr("PW") = esl::geography::countries::PW;
    scope().attr("PY") = esl::geography::countries::PY;
    scope().attr("QA") = esl::geography::countries::QA;
    scope().attr("RE") = esl::geography::countries::RE;
    scope().attr("RO") = esl::geography::countries::RO;
    scope().attr("RS") = esl::geography::countries::RS;
    scope().attr("RU") = esl::geography::countries::RU;
    scope().attr("RW") = esl::geography::countries::RW;
    scope().attr("SA") = esl::geography::countries::SA;
    scope().attr("SB") = esl::geography::countries::SB;
    scope().attr("SC") = esl::geography::countries::SC;
    scope().attr("SD") = esl::geography::countries::SD;
    scope().attr("SE") = esl::geography::countries::SE;
    scope().attr("SG") = esl::geography::countries::SG;
    scope().attr("SH") = esl::geography::countries::SH;
    scope().attr("SI") = esl::geography::countries::SI;
    scope().attr("SJ") = esl::geography::countries::SJ;
    scope().attr("SK") = esl::geography::countries::SK;
    scope().attr("SL") = esl::geography::countries::SL;
    scope().attr("SM") = esl::geography::countries::SM;
    scope().attr("SN") = esl::geography::countries::SN;
    scope().attr("SO") = esl::geography::countries::SO;
    scope().attr("SR") = esl::geography::countries::SR;
    scope().attr("SS") = esl::geography::countries::SS;
    scope().attr("ST") = esl::geography::countries::ST;
    scope().attr("SV") = esl::geography::countries::SV;
    scope().attr("SX") = esl::geography::countries::SX;
    scope().attr("SY") = esl::geography::countries::SY;
    scope().attr("SZ") = esl::geography::countries::SZ;
    scope().attr("TC") = esl::geography::countries::TC;
    scope().attr("TD") = esl::geography::countries::TD;
    scope().attr("TF") = esl::geography::countries::TF;
    scope().attr("TG") = esl::geography::countries::TG;
    scope().attr("TH") = esl::geography::countries::TH;
    scope().attr("TJ") = esl::geography::countries::TJ;
    scope().attr("TK") = esl::geography::countries::TK;
    scope().attr("TL") = esl::geography::countries::TL;
    scope().attr("TM") = esl::geography::countries::TM;
    scope().attr("TN") = esl::geography::countries::TN;
    scope().attr("TO") = esl::geography::countries::TO;
    scope().attr("TR") = esl::geography::countries::TR;
    scope().attr("TT") = esl::geography::countries::TT;
    scope().attr("TV") = esl::geography::countries::TV;
    scope().attr("TW") = esl::geography::countries::TW;
    scope().attr("TZ") = esl::geography::countries::TZ;
    scope().attr("UA") = esl::geography::countries::UA;
    scope().attr("UG") = esl::geography::countries::UG;
    scope().attr("UM") = esl::geography::countries::UM;
    scope().attr("US") = esl::geography::countries::US;
    scope().attr("UY") = esl::geography::countries::UY;
    scope().attr("UZ") = esl::geography::countries::UZ;
    scope().attr("VA") = esl::geography::countries::VA;
    scope().attr("VC") = esl::geography::countries::VC;
    scope().attr("VE") = esl::geography::countries::VE;
    scope().attr("VG") = esl::geography::countries::VG;
    scope().attr("VI") = esl::geography::countries::VI;
    scope().attr("VN") = esl::geography::countries::VN;
    scope().attr("VU") = esl::geography::countries::VU;
    scope().attr("WF") = esl::geography::countries::WF;
    scope().attr("WS") = esl::geography::countries::WS;
    scope().attr("YE") = esl::geography::countries::YE;
    scope().attr("YT") = esl::geography::countries::YT;
    scope().attr("ZA") = esl::geography::countries::ZA;
    scope().attr("ZM") = esl::geography::countries::ZM;
    scope().attr("ZW") = esl::geography::countries::ZW;
}
#endif
