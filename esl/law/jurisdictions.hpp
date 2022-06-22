/// \file   jurisdictions.hpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2018-04-28
/// \copyright  Copyright 2017-2019 The Institute for New Economic Thinking, Oxford Martin School, University of Oxford
///
///             Licensed under the Apache License, Version 2.0 (the "License");
///             you may not use this file except in compliance with the License.
///             You may obtain a copy of the License at
///
///                 http://www.apache.org/licenses/LICENSE-2.0
///
///             Unless required by applicable law or agreed to in writing, software
///             distributed under the License is distributed on an "AS IS" BASIS,
///             WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
///             See the License for the specific language governing permissions and
///             limitations under the License.
///
///             You may obtain instructions to fulfill the attribution requirements in CITATION.cff
///
#ifndef ESL_LAW_JURISDICTIONS_HPP
#define ESL_LAW_JURISDICTIONS_HPP

#include <esl/economics/currencies.hpp>
#include <esl/geography/countries.hpp>
#include <esl/law/jurisdiction.hpp>



namespace esl::law {

    namespace jurisdictions {
        constexpr jurisdiction AF = {geography::countries::AF, economics::currencies::AFN};
        constexpr jurisdiction AL = {geography::countries::AL, economics::currencies::ALL};
        constexpr jurisdiction DZ = {geography::countries::DZ, economics::currencies::DZD};
        constexpr jurisdiction AS = {geography::countries::AS, economics::currencies::USD};
        constexpr jurisdiction AD = {geography::countries::AD, economics::currencies::EUR};
        constexpr jurisdiction AO = {geography::countries::AO, economics::currencies::AOA};
        constexpr jurisdiction AI = {geography::countries::AI, economics::currencies::XCD};
        constexpr jurisdiction AG = {geography::countries::AG, economics::currencies::XCD};
        constexpr jurisdiction AR = {geography::countries::AR, economics::currencies::ARS};
        constexpr jurisdiction AM = {geography::countries::AM, economics::currencies::AMD};
        constexpr jurisdiction AW = {geography::countries::AW, economics::currencies::AWG};
        constexpr jurisdiction AU = {geography::countries::AU, economics::currencies::AUD};
        constexpr jurisdiction AT = {geography::countries::AT, economics::currencies::EUR};
        constexpr jurisdiction AZ = {geography::countries::AZ, economics::currencies::AZN};
        constexpr jurisdiction BS = {geography::countries::BS, economics::currencies::BSD};
        constexpr jurisdiction BH = {geography::countries::BH, economics::currencies::BHD};
        constexpr jurisdiction BD = {geography::countries::BD, economics::currencies::BDT};
        constexpr jurisdiction BB = {geography::countries::BB, economics::currencies::BBD};
        constexpr jurisdiction BY = {geography::countries::BY, economics::currencies::BYN};
        constexpr jurisdiction BE = {geography::countries::BE, economics::currencies::EUR};
        constexpr jurisdiction BZ = {geography::countries::BZ, economics::currencies::BZD};
        constexpr jurisdiction BJ = {geography::countries::BJ, economics::currencies::XOF};
        constexpr jurisdiction BM = {geography::countries::BM, economics::currencies::BMD};
        constexpr jurisdiction BT = {geography::countries::BT, economics::currencies::BTN};
        constexpr jurisdiction BO = {geography::countries::BO, economics::currencies::BOB};
        constexpr jurisdiction BQ = {geography::countries::BQ, economics::currencies::USD};
        constexpr jurisdiction BA = {geography::countries::BA, economics::currencies::BAM};
        constexpr jurisdiction BW = {geography::countries::BW, economics::currencies::BWP};
        constexpr jurisdiction BV = {geography::countries::BV, economics::currencies::NOK};
        constexpr jurisdiction BR = {geography::countries::BR, economics::currencies::BRL};
        constexpr jurisdiction IO = {geography::countries::IO, economics::currencies::USD};
        constexpr jurisdiction VG = {geography::countries::VG, economics::currencies::USD};
        constexpr jurisdiction BN = {geography::countries::BN, economics::currencies::BND};
        constexpr jurisdiction BG = {geography::countries::BG, economics::currencies::BGN};
        constexpr jurisdiction BF = {geography::countries::BF, economics::currencies::XOF};
        constexpr jurisdiction BI = {geography::countries::BI, economics::currencies::BIF};
        constexpr jurisdiction CV = {geography::countries::CV, economics::currencies::CVE};
        constexpr jurisdiction KH = {geography::countries::KH, economics::currencies::KHR};
        constexpr jurisdiction CM = {geography::countries::CM, economics::currencies::XAF};
        constexpr jurisdiction CA = {geography::countries::CA, economics::currencies::CAD};
        constexpr jurisdiction KY = {geography::countries::KY, economics::currencies::KYD};
        constexpr jurisdiction CF = {geography::countries::CF, economics::currencies::XAF};
        constexpr jurisdiction TD = {geography::countries::TD, economics::currencies::XAF};
        constexpr jurisdiction CL = {geography::countries::CL, economics::currencies::CLP};
        constexpr jurisdiction CN = {geography::countries::CN, economics::currencies::CNY};
        constexpr jurisdiction HK = {geography::countries::HK, economics::currencies::HKD};
        constexpr jurisdiction MO = {geography::countries::MO, economics::currencies::MOP};
        constexpr jurisdiction CX = {geography::countries::CX, economics::currencies::AUD};
        constexpr jurisdiction CC = {geography::countries::CC, economics::currencies::AUD};
        constexpr jurisdiction CO = {geography::countries::CO, economics::currencies::COP};
        constexpr jurisdiction KM = {geography::countries::KM, economics::currencies::KMF};
        constexpr jurisdiction CG = {geography::countries::CG, economics::currencies::XAF};
        constexpr jurisdiction CK = {geography::countries::CK, economics::currencies::NZD};
        constexpr jurisdiction CR = {geography::countries::CR, economics::currencies::CRC};
        constexpr jurisdiction HR = {geography::countries::HR, economics::currencies::HRK};
        constexpr jurisdiction CU = {geography::countries::CU, economics::currencies::CUP};
        constexpr jurisdiction CW = {geography::countries::CW, economics::currencies::ANG};
        constexpr jurisdiction CY = {geography::countries::CY, economics::currencies::EUR};
        constexpr jurisdiction CZ = {geography::countries::CZ, economics::currencies::CZK};
        constexpr jurisdiction CI = {geography::countries::CI, economics::currencies::XOF};
        constexpr jurisdiction KP = {geography::countries::KP, economics::currencies::KPW};
        constexpr jurisdiction CD = {geography::countries::CD, economics::currencies::CDF};
        constexpr jurisdiction DK = {geography::countries::DK, economics::currencies::DKK};
        constexpr jurisdiction DJ = {geography::countries::DJ, economics::currencies::DJF};
        constexpr jurisdiction DM = {geography::countries::DM, economics::currencies::XCD};
        constexpr jurisdiction DO = {geography::countries::DO, economics::currencies::DOP};
        constexpr jurisdiction EC = {geography::countries::EC, economics::currencies::USD};
        constexpr jurisdiction EG = {geography::countries::EG, economics::currencies::EGP};
        constexpr jurisdiction SV = {geography::countries::SV, economics::currencies::SVC};
        constexpr jurisdiction GQ = {geography::countries::GQ, economics::currencies::XAF};
        constexpr jurisdiction ER = {geography::countries::ER, economics::currencies::ERN};
        constexpr jurisdiction EE = {geography::countries::EE, economics::currencies::EUR};
        constexpr jurisdiction ET = {geography::countries::ET, economics::currencies::ETB};
        constexpr jurisdiction FO = {geography::countries::FO, economics::currencies::DKK};
        constexpr jurisdiction FJ = {geography::countries::FJ, economics::currencies::FJD};
        constexpr jurisdiction FI = {geography::countries::FI, economics::currencies::EUR};
        constexpr jurisdiction FR = {geography::countries::FR, economics::currencies::EUR};
        constexpr jurisdiction GF = {geography::countries::GF, economics::currencies::EUR};
        constexpr jurisdiction PF = {geography::countries::PF, economics::currencies::XPF};
        constexpr jurisdiction TF = {geography::countries::TF, economics::currencies::EUR};
        constexpr jurisdiction GA = {geography::countries::GA, economics::currencies::XAF};
        constexpr jurisdiction GM = {geography::countries::GM, economics::currencies::GMD};
        constexpr jurisdiction GE = {geography::countries::GE, economics::currencies::GEL};
        constexpr jurisdiction DE = {geography::countries::DE, economics::currencies::EUR};
        constexpr jurisdiction GH = {geography::countries::GH, economics::currencies::GHS};
        constexpr jurisdiction GI = {geography::countries::GI, economics::currencies::GIP};
        constexpr jurisdiction GR = {geography::countries::GR, economics::currencies::EUR};
        constexpr jurisdiction GL = {geography::countries::GL, economics::currencies::DKK};
        constexpr jurisdiction GD = {geography::countries::GD, economics::currencies::XCD};
        constexpr jurisdiction GP = {geography::countries::GP, economics::currencies::EUR};
        constexpr jurisdiction GU = {geography::countries::GU, economics::currencies::USD};
        constexpr jurisdiction GT = {geography::countries::GT, economics::currencies::GTQ};
        constexpr jurisdiction GG = {geography::countries::GG, economics::currencies::GBP};
        constexpr jurisdiction GN = {geography::countries::GN, economics::currencies::GNF};
        constexpr jurisdiction GW = {geography::countries::GW, economics::currencies::XOF};
        constexpr jurisdiction GY = {geography::countries::GY, economics::currencies::GYD};
        constexpr jurisdiction HT = {geography::countries::HT, economics::currencies::HTG};
        constexpr jurisdiction HM = {geography::countries::HM, economics::currencies::AUD};
        constexpr jurisdiction VA = {geography::countries::VA, economics::currencies::EUR};
        constexpr jurisdiction HN = {geography::countries::HN, economics::currencies::HNL};
        constexpr jurisdiction HU = {geography::countries::HU, economics::currencies::HUF};
        constexpr jurisdiction IS = {geography::countries::IS, economics::currencies::ISK};
//        constexpr jurisdiction IN = {geography::countries::IN, economics::currencies::INR};
        constexpr jurisdiction ID = {geography::countries::ID, economics::currencies::IDR};
        constexpr jurisdiction IR = {geography::countries::IR, economics::currencies::IRR};
        constexpr jurisdiction IQ = {geography::countries::IQ, economics::currencies::IQD};
        constexpr jurisdiction IE = {geography::countries::IE, economics::currencies::EUR};
        constexpr jurisdiction IM = {geography::countries::IM, economics::currencies::GBP};
        constexpr jurisdiction IL = {geography::countries::IL, economics::currencies::ILS};
        constexpr jurisdiction IT = {geography::countries::IT, economics::currencies::EUR};
        constexpr jurisdiction JM = {geography::countries::JM, economics::currencies::JMD};
        constexpr jurisdiction JP = {geography::countries::JP, economics::currencies::JPY};
        constexpr jurisdiction JE = {geography::countries::JE, economics::currencies::GBP};
        constexpr jurisdiction JO = {geography::countries::JO, economics::currencies::JOD};
        constexpr jurisdiction KZ = {geography::countries::KZ, economics::currencies::KZT};
        constexpr jurisdiction KE = {geography::countries::KE, economics::currencies::KES};
        constexpr jurisdiction KI = {geography::countries::KI, economics::currencies::AUD};
        constexpr jurisdiction KW = {geography::countries::KW, economics::currencies::KWD};
        constexpr jurisdiction KG = {geography::countries::KG, economics::currencies::KGS};
        constexpr jurisdiction LA = {geography::countries::LA, economics::currencies::LAK};
        constexpr jurisdiction LV = {geography::countries::LV, economics::currencies::EUR};
        constexpr jurisdiction LB = {geography::countries::LB, economics::currencies::LBP};
        constexpr jurisdiction LS = {geography::countries::LS, economics::currencies::LSL};
        constexpr jurisdiction LR = {geography::countries::LR, economics::currencies::LRD};
        constexpr jurisdiction LY = {geography::countries::LY, economics::currencies::LYD};
        constexpr jurisdiction LI = {geography::countries::LI, economics::currencies::CHF};
        constexpr jurisdiction LT = {geography::countries::LT, economics::currencies::EUR};
        constexpr jurisdiction LU = {geography::countries::LU, economics::currencies::EUR};
        constexpr jurisdiction MG = {geography::countries::MG, economics::currencies::MGA};
        constexpr jurisdiction MW = {geography::countries::MW, economics::currencies::MWK};
        constexpr jurisdiction MY = {geography::countries::MY, economics::currencies::MYR};
        constexpr jurisdiction MV = {geography::countries::MV, economics::currencies::MVR};
        constexpr jurisdiction ML = {geography::countries::ML, economics::currencies::XOF};
        constexpr jurisdiction MT = {geography::countries::MT, economics::currencies::EUR};
        constexpr jurisdiction MH = {geography::countries::MH, economics::currencies::USD};
        constexpr jurisdiction MQ = {geography::countries::MQ, economics::currencies::EUR};
        constexpr jurisdiction MR = {geography::countries::MR, economics::currencies::MRU};
        constexpr jurisdiction MU = {geography::countries::MU, economics::currencies::MUR};
        constexpr jurisdiction YT = {geography::countries::YT, economics::currencies::EUR};
        constexpr jurisdiction MX = {geography::countries::MX, economics::currencies::MXN};
        constexpr jurisdiction FM = {geography::countries::FM, economics::currencies::USD};
        constexpr jurisdiction MC = {geography::countries::MC, economics::currencies::EUR};
        constexpr jurisdiction MN = {geography::countries::MN, economics::currencies::MNT};
        constexpr jurisdiction ME = {geography::countries::ME, economics::currencies::EUR};
        constexpr jurisdiction MS = {geography::countries::MS, economics::currencies::XCD};
        constexpr jurisdiction MA = {geography::countries::MA, economics::currencies::MAD};
        constexpr jurisdiction MZ = {geography::countries::MZ, economics::currencies::MZN};
        constexpr jurisdiction MM = {geography::countries::MM, economics::currencies::MMK};
        constexpr jurisdiction NA = {geography::countries::NA, economics::currencies::NAD};
        constexpr jurisdiction NR = {geography::countries::NR, economics::currencies::AUD};
        constexpr jurisdiction NP = {geography::countries::NP, economics::currencies::NPR};
        constexpr jurisdiction NL = {geography::countries::NL, economics::currencies::EUR};
        constexpr jurisdiction NC = {geography::countries::NC, economics::currencies::XPF};
        constexpr jurisdiction NZ = {geography::countries::NZ, economics::currencies::NZD};
        constexpr jurisdiction NI = {geography::countries::NI, economics::currencies::NIO};
        constexpr jurisdiction NE = {geography::countries::NE, economics::currencies::XOF};
        constexpr jurisdiction NG = {geography::countries::NG, economics::currencies::NGN};
        constexpr jurisdiction NU = {geography::countries::NU, economics::currencies::NZD};
        constexpr jurisdiction NF = {geography::countries::NF, economics::currencies::AUD};
        constexpr jurisdiction MP = {geography::countries::MP, economics::currencies::USD};
        constexpr jurisdiction NO = {geography::countries::NO, economics::currencies::NOK};
        constexpr jurisdiction OM = {geography::countries::OM, economics::currencies::OMR};
        constexpr jurisdiction PK = {geography::countries::PK, economics::currencies::PKR};
        constexpr jurisdiction PW = {geography::countries::PW, economics::currencies::USD};
        constexpr jurisdiction PA = {geography::countries::PA, economics::currencies::PAB};
        constexpr jurisdiction PG = {geography::countries::PG, economics::currencies::PGK};
        constexpr jurisdiction PY = {geography::countries::PY, economics::currencies::PYG};
        constexpr jurisdiction PE = {geography::countries::PE, economics::currencies::PEN};
        constexpr jurisdiction PH = {geography::countries::PH, economics::currencies::PHP};
        constexpr jurisdiction PN = {geography::countries::PN, economics::currencies::NZD};
        constexpr jurisdiction PL = {geography::countries::PL, economics::currencies::PLN};
        constexpr jurisdiction PT = {geography::countries::PT, economics::currencies::EUR};
        constexpr jurisdiction PR = {geography::countries::PR, economics::currencies::USD};
        constexpr jurisdiction QA = {geography::countries::QA, economics::currencies::QAR};
        constexpr jurisdiction KR = {geography::countries::KR, economics::currencies::KRW};
        constexpr jurisdiction MD = {geography::countries::MD, economics::currencies::MDL};
        constexpr jurisdiction RO = {geography::countries::RO, economics::currencies::RON};
        constexpr jurisdiction RU = {geography::countries::RU, economics::currencies::RUB};
        constexpr jurisdiction RW = {geography::countries::RW, economics::currencies::RWF};
        constexpr jurisdiction RE = {geography::countries::RE, economics::currencies::EUR};
        constexpr jurisdiction BL = {geography::countries::BL, economics::currencies::EUR};
        constexpr jurisdiction SH = {geography::countries::SH, economics::currencies::SHP};
        constexpr jurisdiction KN = {geography::countries::KN, economics::currencies::XCD};
        constexpr jurisdiction LC = {geography::countries::LC, economics::currencies::XCD};
        constexpr jurisdiction MF = {geography::countries::MF, economics::currencies::EUR};
        constexpr jurisdiction PM = {geography::countries::PM, economics::currencies::EUR};
        constexpr jurisdiction VC = {geography::countries::VC, economics::currencies::XCD};
        constexpr jurisdiction WS = {geography::countries::WS, economics::currencies::WST};
        constexpr jurisdiction SM = {geography::countries::SM, economics::currencies::EUR};
        constexpr jurisdiction ST = {geography::countries::ST, economics::currencies::STN};
        constexpr jurisdiction SA = {geography::countries::SA, economics::currencies::SAR};
        constexpr jurisdiction SN = {geography::countries::SN, economics::currencies::XOF};
        constexpr jurisdiction RS = {geography::countries::RS, economics::currencies::RSD};
        constexpr jurisdiction SC = {geography::countries::SC, economics::currencies::SCR};
        constexpr jurisdiction SL = {geography::countries::SL, economics::currencies::SLL};
        constexpr jurisdiction SG = {geography::countries::SG, economics::currencies::SGD};
        constexpr jurisdiction SX = {geography::countries::SX, economics::currencies::ANG};
        constexpr jurisdiction SK = {geography::countries::SK, economics::currencies::EUR};
        constexpr jurisdiction SI = {geography::countries::SI, economics::currencies::EUR};
        constexpr jurisdiction SB = {geography::countries::SB, economics::currencies::SBD};
        constexpr jurisdiction SO = {geography::countries::SO, economics::currencies::SOS};
        constexpr jurisdiction ZA = {geography::countries::ZA, economics::currencies::ZAR};
        constexpr jurisdiction SS = {geography::countries::SS, economics::currencies::SSP};
        constexpr jurisdiction ES = {geography::countries::ES, economics::currencies::EUR};
        constexpr jurisdiction LK = {geography::countries::LK, economics::currencies::LKR};
        constexpr jurisdiction SD = {geography::countries::SD, economics::currencies::SDG};
        constexpr jurisdiction SR = {geography::countries::SR, economics::currencies::SRD};
        constexpr jurisdiction SJ = {geography::countries::SJ, economics::currencies::NOK};
        constexpr jurisdiction SZ = {geography::countries::SZ, economics::currencies::SZL};
        constexpr jurisdiction SE = {geography::countries::SE, economics::currencies::SEK};
        constexpr jurisdiction CH = {geography::countries::CH, economics::currencies::CHF};
        constexpr jurisdiction SY = {geography::countries::SY, economics::currencies::SYP};
        constexpr jurisdiction TJ = {geography::countries::TJ, economics::currencies::TJS};
        constexpr jurisdiction TH = {geography::countries::TH, economics::currencies::THB};
        constexpr jurisdiction MK = {geography::countries::MK, economics::currencies::MKD};
        constexpr jurisdiction TL = {geography::countries::TL, economics::currencies::USD};
        constexpr jurisdiction TG = {geography::countries::TG, economics::currencies::XOF};
        constexpr jurisdiction TK = {geography::countries::TK, economics::currencies::NZD};
        constexpr jurisdiction TO = {geography::countries::TO, economics::currencies::TOP};
        constexpr jurisdiction TT = {geography::countries::TT, economics::currencies::TTD};
        constexpr jurisdiction TN = {geography::countries::TN, economics::currencies::TND};
        constexpr jurisdiction TR = {geography::countries::TR, economics::currencies::TRY};
        constexpr jurisdiction TM = {geography::countries::TM, economics::currencies::TMT};
        constexpr jurisdiction TC = {geography::countries::TC, economics::currencies::USD};
        constexpr jurisdiction TV = {geography::countries::TV, economics::currencies::AUD};
        constexpr jurisdiction UG = {geography::countries::UG, economics::currencies::UGX};
        constexpr jurisdiction UA = {geography::countries::UA, economics::currencies::UAH};
        constexpr jurisdiction AE = {geography::countries::AE, economics::currencies::AED};
        constexpr jurisdiction GB = {geography::countries::GB, economics::currencies::GBP};
        constexpr jurisdiction TZ = {geography::countries::TZ, economics::currencies::TZS};
        constexpr jurisdiction UM = {geography::countries::UM, economics::currencies::USD};
        constexpr jurisdiction VI = {geography::countries::VI, economics::currencies::USD};
        constexpr jurisdiction US = {geography::countries::US, economics::currencies::USD};
        constexpr jurisdiction UY = {geography::countries::UY, economics::currencies::UYU};
        constexpr jurisdiction UZ = {geography::countries::UZ, economics::currencies::UZS};
        constexpr jurisdiction VU = {geography::countries::VU, economics::currencies::VUV};
        constexpr jurisdiction VE = {geography::countries::VE, economics::currencies::VEF};
        constexpr jurisdiction VN = {geography::countries::VN, economics::currencies::VND};
        constexpr jurisdiction WF = {geography::countries::WF, economics::currencies::XPF};
        constexpr jurisdiction EH = {geography::countries::EH, economics::currencies::MAD};
        constexpr jurisdiction YE = {geography::countries::YE, economics::currencies::YER};
        constexpr jurisdiction ZM = {geography::countries::ZM, economics::currencies::ZMW};
        constexpr jurisdiction ZW = {geography::countries::ZW, economics::currencies::ZWL};
        constexpr jurisdiction AX = {geography::countries::AX, economics::currencies::EUR};
    }
}//namespace jurisdictions

#endif//ESL_LAW_JURISDICTIONS_HPP
