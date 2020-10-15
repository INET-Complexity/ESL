/// \file   currencies.hpp
///
/// \brief  Defines ISO 4217 currencies, with names, as constant expressions.
///
/// \authors    Maarten P. Scholl
/// \date       2018-02-04
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
#ifndef ESL_CURRENCIES_HPP
#define ESL_CURRENCIES_HPP


#include <esl/economics/iso_4217.hpp>

#include <utility>
#include <string>


namespace esl::economics {
    ///
    /// \brief ISO 4217 currencies and codes
    ///
    namespace currencies {

        constexpr iso_4217 AED = iso_4217({'A', 'E', 'D'});
        constexpr iso_4217 AFN = iso_4217({'A', 'F', 'N'});
        constexpr iso_4217 ALL = iso_4217({'A', 'L', 'L'});
        constexpr iso_4217 AMD = iso_4217({'A', 'M', 'D'});
        constexpr iso_4217 ANG = iso_4217({'A', 'N', 'G'});
        constexpr iso_4217 AOA = iso_4217({'A', 'O', 'A'});
        constexpr iso_4217 ARS = iso_4217({'A', 'R', 'S'});
        constexpr iso_4217 AUD = iso_4217({'A', 'U', 'D'});
        constexpr iso_4217 AWG = iso_4217({'A', 'W', 'G'});
        constexpr iso_4217 AZN = iso_4217({'A', 'Z', 'N'});
        constexpr iso_4217 BAM = iso_4217({'B', 'A', 'M'});
        constexpr iso_4217 BBD = iso_4217({'B', 'B', 'D'});
        constexpr iso_4217 BDT = iso_4217({'B', 'D', 'T'});
        constexpr iso_4217 BGN = iso_4217({'B', 'G', 'N'});
        constexpr iso_4217 BHD = iso_4217({'B', 'H', 'D'}, 1000);
        constexpr iso_4217 BIF = iso_4217({'B', 'I', 'F'}, 1);
        constexpr iso_4217 BMD = iso_4217({'B', 'M', 'D'});
        constexpr iso_4217 BND = iso_4217({'B', 'N', 'D'});
        constexpr iso_4217 BOB = iso_4217({'B', 'O', 'B'});
        constexpr iso_4217 BOV = iso_4217({'B', 'O', 'V'});
        constexpr iso_4217 BRL = iso_4217({'B', 'R', 'L'});
        constexpr iso_4217 BSD = iso_4217({'B', 'S', 'D'});
        constexpr iso_4217 BTN = iso_4217({'B', 'T', 'N'});
        constexpr iso_4217 BWP = iso_4217({'B', 'W', 'P'});
        constexpr iso_4217 BYN = iso_4217({'B', 'Y', 'N'});
        constexpr iso_4217 BZD = iso_4217({'B', 'Z', 'D'});
        constexpr iso_4217 CAD = iso_4217({'C', 'A', 'D'});
        constexpr iso_4217 CDF = iso_4217({'C', 'D', 'F'});
        constexpr iso_4217 CHE = iso_4217({'C', 'H', 'E'});
        constexpr iso_4217 CHF = iso_4217({'C', 'H', 'F'});
        constexpr iso_4217 CHW = iso_4217({'C', 'H', 'W'});
        constexpr iso_4217 CLF = iso_4217({'C', 'L', 'F'});
        constexpr iso_4217 CLP = iso_4217({'C', 'L', 'P'}, 1);
        constexpr iso_4217 CNY = iso_4217({'C', 'N', 'Y'});
        constexpr iso_4217 COP = iso_4217({'C', 'O', 'P'});
        constexpr iso_4217 COU = iso_4217({'C', 'O', 'U'});
        constexpr iso_4217 CRC = iso_4217({'C', 'R', 'C'});
        constexpr iso_4217 CUC = iso_4217({'C', 'U', 'C'});
        constexpr iso_4217 CUP = iso_4217({'C', 'U', 'P'});
        constexpr iso_4217 CVE = iso_4217({'C', 'V', 'E'});
        constexpr iso_4217 CZK = iso_4217({'C', 'Z', 'K'});
        constexpr iso_4217 DJF = iso_4217({'D', 'J', 'F'}, 1);
        constexpr iso_4217 DKK = iso_4217({'D', 'K', 'K'});
        constexpr iso_4217 DOP = iso_4217({'D', 'O', 'P'});
        constexpr iso_4217 DZD = iso_4217({'D', 'Z', 'D'});
        constexpr iso_4217 EGP = iso_4217({'E', 'G', 'P'});
        constexpr iso_4217 ERN = iso_4217({'E', 'R', 'N'});
        constexpr iso_4217 ETB = iso_4217({'E', 'T', 'B'});
        constexpr iso_4217 EUR = iso_4217({'E', 'U', 'R'});
        constexpr iso_4217 FJD = iso_4217({'F', 'J', 'D'});
        constexpr iso_4217 FKP = iso_4217({'F', 'K', 'P'});
        constexpr iso_4217 GBP = iso_4217({'G', 'B', 'P'});
        constexpr iso_4217 GEL = iso_4217({'G', 'E', 'L'});
        constexpr iso_4217 GHS = iso_4217({'G', 'H', 'S'});
        constexpr iso_4217 GIP = iso_4217({'G', 'I', 'P'});
        constexpr iso_4217 GMD = iso_4217({'G', 'M', 'D'});
        constexpr iso_4217 GNF = iso_4217({'G', 'N', 'F'}, 1);
        constexpr iso_4217 GTQ = iso_4217({'G', 'T', 'Q'});
        constexpr iso_4217 GYD = iso_4217({'G', 'Y', 'D'});
        constexpr iso_4217 HKD = iso_4217({'H', 'K', 'D'});
        constexpr iso_4217 HNL = iso_4217({'H', 'N', 'L'});
        constexpr iso_4217 HRK = iso_4217({'H', 'R', 'K'});
        constexpr iso_4217 HTG = iso_4217({'H', 'T', 'G'});
        constexpr iso_4217 HUF = iso_4217({'H', 'U', 'F'});
        constexpr iso_4217 IDR = iso_4217({'I', 'D', 'R'});
        constexpr iso_4217 ILS = iso_4217({'I', 'L', 'S'});
        constexpr iso_4217 INR = iso_4217({'I', 'N', 'R'});
        constexpr iso_4217 IQD = iso_4217({'I', 'Q', 'D'}, 1000);
        constexpr iso_4217 IRR = iso_4217({'I', 'R', 'R'});
        constexpr iso_4217 ISK = iso_4217({'I', 'S', 'K'}, 1);
        constexpr iso_4217 JMD = iso_4217({'J', 'M', 'D'});
        constexpr iso_4217 JOD = iso_4217({'J', 'O', 'D'}, 1000);
        constexpr iso_4217 JPY = iso_4217({'J', 'P', 'Y'}, 1);
        constexpr iso_4217 KES = iso_4217({'K', 'E', 'S'});
        constexpr iso_4217 KGS = iso_4217({'K', 'G', 'S'});
        constexpr iso_4217 KHR = iso_4217({'K', 'H', 'R'});
        constexpr iso_4217 KMF = iso_4217({'K', 'M', 'F'}, 1);
        constexpr iso_4217 KPW = iso_4217({'K', 'P', 'W'});
        constexpr iso_4217 KRW = iso_4217({'K', 'R', 'W'}, 1);
        constexpr iso_4217 KWD = iso_4217({'K', 'W', 'D'}, 1000);
        constexpr iso_4217 KYD = iso_4217({'K', 'Y', 'D'});
        constexpr iso_4217 KZT = iso_4217({'K', 'Z', 'T'});
        constexpr iso_4217 LAK = iso_4217({'L', 'A', 'K'});
        constexpr iso_4217 LBP = iso_4217({'L', 'B', 'P'});
        constexpr iso_4217 LKR = iso_4217({'L', 'K', 'R'});
        constexpr iso_4217 LRD = iso_4217({'L', 'R', 'D'});
        constexpr iso_4217 LSL = iso_4217({'L', 'S', 'L'});
        constexpr iso_4217 LYD = iso_4217({'L', 'Y', 'D'}, 1000);
        constexpr iso_4217 MAD = iso_4217({'M', 'A', 'D'});
        constexpr iso_4217 MDL = iso_4217({'M', 'D', 'L'});
        constexpr iso_4217 MGA = iso_4217({'M', 'G', 'A'});
        constexpr iso_4217 MKD = iso_4217({'M', 'K', 'D'});
        constexpr iso_4217 MMK = iso_4217({'M', 'M', 'K'});
        constexpr iso_4217 MNT = iso_4217({'M', 'N', 'T'});
        constexpr iso_4217 MOP = iso_4217({'M', 'O', 'P'});
        constexpr iso_4217 MRU = iso_4217({'M', 'R', 'U'});
        constexpr iso_4217 MUR = iso_4217({'M', 'U', 'R'});
        constexpr iso_4217 MVR = iso_4217({'M', 'V', 'R'});
        constexpr iso_4217 MWK = iso_4217({'M', 'W', 'K'});
        constexpr iso_4217 MXN = iso_4217({'M', 'X', 'N'});
        constexpr iso_4217 MXV = iso_4217({'M', 'X', 'V'});
        constexpr iso_4217 MYR = iso_4217({'M', 'Y', 'R'});
        constexpr iso_4217 MZN = iso_4217({'M', 'Z', 'N'});
        constexpr iso_4217 NAD = iso_4217({'N', 'A', 'D'});
        constexpr iso_4217 NGN = iso_4217({'N', 'G', 'N'});
        constexpr iso_4217 NIO = iso_4217({'N', 'I', 'O'});
        constexpr iso_4217 NOK = iso_4217({'N', 'O', 'K'});
        constexpr iso_4217 NPR = iso_4217({'N', 'P', 'R'});
        constexpr iso_4217 NZD = iso_4217({'N', 'Z', 'D'});
        constexpr iso_4217 OMR = iso_4217({'O', 'M', 'R'}, 1000);
        constexpr iso_4217 PAB = iso_4217({'P', 'A', 'B'});
        constexpr iso_4217 PEN = iso_4217({'P', 'E', 'N'});
        constexpr iso_4217 PGK = iso_4217({'P', 'G', 'K'});
        constexpr iso_4217 PHP = iso_4217({'P', 'H', 'P'});
        constexpr iso_4217 PKR = iso_4217({'P', 'K', 'R'});
        constexpr iso_4217 PLN = iso_4217({'P', 'L', 'N'});
        constexpr iso_4217 PYG = iso_4217({'P', 'Y', 'G'}, 1);
        constexpr iso_4217 QAR = iso_4217({'Q', 'A', 'R'});
        constexpr iso_4217 RON = iso_4217({'R', 'O', 'N'});
        constexpr iso_4217 RSD = iso_4217({'R', 'S', 'D'});
        constexpr iso_4217 RUB = iso_4217({'R', 'U', 'B'});
        constexpr iso_4217 RWF = iso_4217({'R', 'W', 'F'}, 1);
        constexpr iso_4217 SAR = iso_4217({'S', 'A', 'R'});
        constexpr iso_4217 SBD = iso_4217({'S', 'B', 'D'});
        constexpr iso_4217 SCR = iso_4217({'S', 'C', 'R'});
        constexpr iso_4217 SDG = iso_4217({'S', 'D', 'G'});
        constexpr iso_4217 SEK = iso_4217({'S', 'E', 'K'});
        constexpr iso_4217 SGD = iso_4217({'S', 'G', 'D'});
        constexpr iso_4217 SHP = iso_4217({'S', 'H', 'P'});
        constexpr iso_4217 SLL = iso_4217({'S', 'L', 'L'});
        constexpr iso_4217 SOS = iso_4217({'S', 'O', 'S'});
        constexpr iso_4217 SRD = iso_4217({'S', 'R', 'D'});
        constexpr iso_4217 SSP = iso_4217({'S', 'S', 'P'});
        constexpr iso_4217 STN = iso_4217({'S', 'T', 'N'});
        constexpr iso_4217 SVC = iso_4217({'S', 'V', 'C'});
        constexpr iso_4217 SYP = iso_4217({'S', 'Y', 'P'});
        constexpr iso_4217 SZL = iso_4217({'S', 'Z', 'L'});
        constexpr iso_4217 THB = iso_4217({'T', 'H', 'B'});
        constexpr iso_4217 TJS = iso_4217({'T', 'J', 'S'});
        constexpr iso_4217 TMT = iso_4217({'T', 'M', 'T'});
        constexpr iso_4217 TND = iso_4217({'T', 'N', 'D'});
        constexpr iso_4217 TOP = iso_4217({'T', 'O', 'P'});
        constexpr iso_4217 TRY = iso_4217({'T', 'R', 'Y'});
        constexpr iso_4217 TTD = iso_4217({'T', 'T', 'D'});
        constexpr iso_4217 TWD = iso_4217({'T', 'W', 'D'});
        constexpr iso_4217 TZS = iso_4217({'T', 'Z', 'S'});
        constexpr iso_4217 UAH = iso_4217({'U', 'A', 'H'});
        constexpr iso_4217 UGX = iso_4217({'U', 'G', 'X'}, 1);
        constexpr iso_4217 USD = iso_4217({'U', 'S', 'D'});
        constexpr iso_4217 USN = iso_4217({'U', 'S', 'N'});
        constexpr iso_4217 UYI = iso_4217({'U', 'Y', 'I'});
        constexpr iso_4217 UYU = iso_4217({'U', 'Y', 'U'});
        constexpr iso_4217 UZS = iso_4217({'U', 'Z', 'S'});
        constexpr iso_4217 VEF = iso_4217({'V', 'E', 'F'});
        constexpr iso_4217 VND = iso_4217({'V', 'N', 'D'}, 1);
        constexpr iso_4217 VUV = iso_4217({'V', 'U', 'V'}, 1);
        constexpr iso_4217 WST = iso_4217({'W', 'S', 'T'});
        constexpr iso_4217 XAF = iso_4217({'X', 'A', 'F'}, 1);
        constexpr iso_4217 XAG = iso_4217({'X', 'A', 'G'});
        constexpr iso_4217 XAU = iso_4217({'X', 'A', 'U'});
        constexpr iso_4217 XBA = iso_4217({'X', 'B', 'A'});
        constexpr iso_4217 XBB = iso_4217({'X', 'B', 'B'});
        constexpr iso_4217 XBC = iso_4217({'X', 'B', 'C'});
        constexpr iso_4217 XBD = iso_4217({'X', 'B', 'D'});
        constexpr iso_4217 XCD = iso_4217({'X', 'C', 'D'});
        constexpr iso_4217 XDR = iso_4217({'X', 'D', 'R'});
        constexpr iso_4217 XOF = iso_4217({'X', 'O', 'F'}, 1);
        constexpr iso_4217 XPD = iso_4217({'X', 'P', 'D'});
        constexpr iso_4217 XPF = iso_4217({'X', 'P', 'F'}, 1);
        constexpr iso_4217 XPT = iso_4217({'X', 'P', 'T'});
        constexpr iso_4217 XSU = iso_4217({'X', 'S', 'U'});
        // constexpr iso_4217 XTS = iso_4217({'X', 'T', 'S'});
        constexpr iso_4217 XUA = iso_4217({'X', 'U', 'A'});
        // constexpr iso_4217 XXX = iso_4217({'X', 'X', 'X'});
        constexpr iso_4217 YER = iso_4217({'Y', 'E', 'R'});
        constexpr iso_4217 ZAR = iso_4217({'Z', 'A', 'R'});
        constexpr iso_4217 ZMW = iso_4217({'Z', 'M', 'W'});
        constexpr iso_4217 ZWL = iso_4217({'Z', 'W', 'L'});

    }//namespace countries

    namespace detail {
        constexpr size_t unique_hash(iso_4217 c)
        {
            // unique hash
            return             size_t(c.code[0]) - size_t('A')
                        + 26 * size_t(c.code[1]) - size_t('A')
                   + 26 * 26 * size_t(c.code[2]) - size_t('A');
        }
    }  // namespace detail

    template<typename string_t_ = std::string>
    inline string_t_ currency_name(const iso_4217 &c)
    {
        using detail::unique_hash;
        using namespace currencies;

        switch(std::hash<iso_4217>()(c)) {
        case unique_hash(AED):
            return "UAE Dirham";
        case unique_hash(AFN):
            return "Afghani";
        case unique_hash(ALL):
            return "Lek";
        case unique_hash(AMD):
            return "Armenian Dram";
        case unique_hash(ANG):
            return "Netherlands Antillean Guilder";
        case unique_hash(AOA):
            return "Kwanza";
        case unique_hash(ARS):
            return "Argentine Peso";
        case unique_hash(AUD):
            return "Australian Dollar";
        case unique_hash(AWG):
            return "Aruban Florin";
        case unique_hash(AZN):
            return "Azerbaijan Manat";
        case unique_hash(BAM):
            return "Convertible Mark";
        case unique_hash(BBD):
            return "Barbados Dollar";
        case unique_hash(BDT):
            return "Taka";
        case unique_hash(BGN):
            return "Bulgarian Lev";
        case unique_hash(BHD):
            return "Bahraini Dinar";
        case unique_hash(BIF):
            return "Burundi Franc";
        case unique_hash(BMD):
            return "Bermudian Dollar";
        case unique_hash(BND):
            return "Brunei Dollar";
        case unique_hash(BOB):
            return "Boliviano";
        case unique_hash(BOV):
            return "Mvdol";
        case unique_hash(BRL):
            return "Brazilian Real";
        case unique_hash(BSD):
            return "Bahamian Dollar";
        case unique_hash(BTN):
            return "Ngultrum";
        case unique_hash(BWP):
            return "Pula";
        case unique_hash(BYN):
            return "Belarusian Ruble";
        case unique_hash(BZD):
            return "Belize Dollar";
        case unique_hash(CAD):
            return "Canadian Dollar";
        case unique_hash(CDF):
            return "Congolese Franc";
        case unique_hash(CHE):
            return "WIR Euro";
        case unique_hash(CHF):
            return "Swiss Franc";
        case unique_hash(CHW):
            return "WIR Franc";
        case unique_hash(CLF):
            return "Unidad de Fomento";
        case unique_hash(CLP):
            return "Chilean Peso";
        case unique_hash(CNY):
            return "Yuan Renminbi";
        case unique_hash(COP):
            return "Colombian Peso";
        case unique_hash(COU):
            return "Unidad de Valor Real";
        case unique_hash(CRC):
            return "Costa Rican Colon";
        case unique_hash(CUC):
            return "Peso Convertible";
        case unique_hash(CUP):
            return "Cuban Peso";
        case unique_hash(CVE):
            return "Cabo Verde Escudo";
        case unique_hash(CZK):
            return "Czech Koruna";
        case unique_hash(DJF):
            return "Djibouti Franc";
        case unique_hash(DKK):
            return "Danish Krone";
        case unique_hash(DOP):
            return "Dominican Peso";
        case unique_hash(DZD):
            return "Algerian Dinar";
        case unique_hash(EGP):
            return "Egyptian Pound";
        case unique_hash(ERN):
            return "Nakfa";
        case unique_hash(ETB):
            return "Ethiopian Birr";
        case unique_hash(EUR):
            return "Euro";
        case unique_hash(FJD):
            return "Fiji Dollar";
        case unique_hash(FKP):
            return "Falkland Islands Pound";
        case unique_hash(GBP):
            return "Pound Sterling";
        case unique_hash(GEL):
            return "Lari";
        case unique_hash(GHS):
            return "Ghana Cedi";
        case unique_hash(GIP):
            return "Gibraltar Pound";
        case unique_hash(GMD):
            return "Dalasi";
        case unique_hash(GNF):
            return "Guinean Franc";
        case unique_hash(GTQ):
            return "Quetzal";
        case unique_hash(GYD):
            return "Guyana Dollar";
        case unique_hash(HKD):
            return "Hong Kong Dollar";
        case unique_hash(HNL):
            return "Lempira";
        case unique_hash(HRK):
            return "Kuna";
        case unique_hash(HTG):
            return "Gourde";
        case unique_hash(HUF):
            return "Forint";
        case unique_hash(IDR):
            return "Rupiah";
        case unique_hash(ILS):
            return "New Israeli Sheqel";
        case unique_hash(INR):
            return "Indian Rupee";
        case unique_hash(IQD):
            return "Iraqi Dinar";
        case unique_hash(IRR):
            return "Iranian Rial";
        case unique_hash(ISK):
            return "Iceland Krona";
        case unique_hash(JMD):
            return "Jamaican Dollar";
        case unique_hash(JOD):
            return "Jordanian Dinar";
        case unique_hash(JPY):
            return "Yen";
        case unique_hash(KES):
            return "Kenyan Shilling";
        case unique_hash(KGS):
            return "Som";
        case unique_hash(KHR):
            return "Riel";
        case unique_hash(KMF):
            return "Comorian Franc ";
        case unique_hash(KPW):
            return "North Korean Won";
        case unique_hash(KRW):
            return "Won";
        case unique_hash(KWD):
            return "Kuwaiti Dinar";
        case unique_hash(KYD):
            return "Cayman Islands Dollar";
        case unique_hash(KZT):
            return "Tenge";
        case unique_hash(LAK):
            return "Lao Kip";
        case unique_hash(LBP):
            return "Lebanese Pound";
        case unique_hash(LKR):
            return "Sri Lanka Rupee";
        case unique_hash(LRD):
            return "Liberian Dollar";
        case unique_hash(LSL):
            return "Loti";
        case unique_hash(LYD):
            return "Libyan Dinar";
        case unique_hash(MAD):
            return "Moroccan Dirham";
        case unique_hash(MDL):
            return "Moldovan Leu";
        case unique_hash(MGA):
            return "Malagasy Ariary";
        case unique_hash(MKD):
            return "Denar";
        case unique_hash(MMK):
            return "Kyat";
        case unique_hash(MNT):
            return "Tugrik";
        case unique_hash(MOP):
            return "Pataca";
        case unique_hash(MRU):
            return "Ouguiya";
        case unique_hash(MUR):
            return "Mauritius Rupee";
        case unique_hash(MVR):
            return "Rufiyaa";
        case unique_hash(MWK):
            return "Malawi Kwacha";
        case unique_hash(MXN):
            return "Mexican Peso";
        case unique_hash(MXV):
            return "Mexican Unidad de Inversion (UDI)";
        case unique_hash(MYR):
            return "Malaysian Ringgit";
        case unique_hash(MZN):
            return "Mozambique Metical";
        case unique_hash(NAD):
            return "Namibia Dollar";
        case unique_hash(NGN):
            return "Naira";
        case unique_hash(NIO):
            return "Cordoba Oro";
        case unique_hash(NOK):
            return "Norwegian Krone";
        case unique_hash(NPR):
            return "Nepalese Rupee";
        case unique_hash(NZD):
            return "New Zealand Dollar";
        case unique_hash(OMR):
            return "Rial Omani";
        case unique_hash(PAB):
            return "Balboa";
        case unique_hash(PEN):
            return "Sol";
        case unique_hash(PGK):
            return "Kina";
        case unique_hash(PHP):
            return "Philippine Piso";
        case unique_hash(PKR):
            return "Pakistan Rupee";
        case unique_hash(PLN):
            return "Zloty";
        case unique_hash(PYG):
            return "Guarani";
        case unique_hash(QAR):
            return "Qatari Rial";
        case unique_hash(RON):
            return "Romanian Leu";
        case unique_hash(RSD):
            return "Serbian Dinar";
        case unique_hash(RUB):
            return "Russian Ruble";
        case unique_hash(RWF):
            return "Rwanda Franc";
        case unique_hash(SAR):
            return "Saudi Riyal";
        case unique_hash(SBD):
            return "Solomon Islands Dollar";
        case unique_hash(SCR):
            return "Seychelles Rupee";
        case unique_hash(SDG):
            return "Sudanese Pound";
        case unique_hash(SEK):
            return "Swedish Krona";
        case unique_hash(SGD):
            return "Singapore Dollar";
        case unique_hash(SHP):
            return "Saint Helena Pound";
        case unique_hash(SLL):
            return "Leone";
        case unique_hash(SOS):
            return "Somali Shilling";
        case unique_hash(SRD):
            return "Surinam Dollar";
        case unique_hash(SSP):
            return "South Sudanese Pound";
        case unique_hash(STN):
            return "Dobra";
        case unique_hash(SVC):
            return "El Salvador Colon";
        case unique_hash(SYP):
            return "Syrian Pound";
        case unique_hash(SZL):
            return "Lilangeni";
        case unique_hash(THB):
            return "Baht";
        case unique_hash(TJS):
            return "Somoni";
        case unique_hash(TMT):
            return "Turkmenistan New Manat";
        case unique_hash(TND):
            return "Tunisian Dinar";
        case unique_hash(TOP):
            return "Pa’anga";
        case unique_hash(TRY):
            return "Turkish Lira";
        case unique_hash(TTD):
            return "Trinidad and Tobago Dollar";
        case unique_hash(TWD):
            return "New Taiwan Dollar";
        case unique_hash(TZS):
            return "Tanzanian Shilling";
        case unique_hash(UAH):
            return "Hryvnia";
        case unique_hash(UGX):
            return "Uganda Shilling";
        case unique_hash(USD):
            return "US Dollar";
        case unique_hash(USN):
            return "US Dollar (Next day)";
        case unique_hash(UYI):
            return "Uruguay Peso en Unidades Indexadas (URUIURUI)";
        case unique_hash(UYU):
            return "Peso Uruguayo";
        case unique_hash(UZS):
            return "Uzbekistan Sum";
        case unique_hash(VEF):
            return "Bolívar";
        case unique_hash(VND):
            return "Dong";
        case unique_hash(VUV):
            return "Vatu";
        case unique_hash(WST):
            return "Tala";
        case unique_hash(XAF):
            return "CFA Franc BEAC";
        case unique_hash(XAG):
            return "Silver";
        case unique_hash(XAU):
            return "Gold";
        case unique_hash(XBA):
            return "Bond Markets Unit European Composite Unit (EURCO)";
        case unique_hash(XBB):
            return "Bond Markets Unit European Monetary Unit (E.M.U.-6)";
        case unique_hash(XBC):
            return "Bond Markets Unit European Unit of Account 9 (E.U.A.-9)";
        case unique_hash(XBD):
            return "Bond Markets Unit European Unit of Account 17 (E.U.A.-17)";
        case unique_hash(XCD):
            return "East Caribbean Dollar";
        case unique_hash(XDR):
            return "SDR (Special Drawing Right)";
        case unique_hash(XOF):
            return "CFA Franc BCEAO";
        case unique_hash(XPD):
            return "Palladium";
        case unique_hash(XPF):
            return "CFP Franc";
        case unique_hash(XPT):
            return "Platinum";
        case unique_hash(XSU):
            return "Sucre";
        // case unique_hash(XTS):
        //     return "Codes specifically reserved for testing purposes";
        case unique_hash(XUA):
            return "ADB Unit of Account";
        // case unique_hash(XXX):
        //     return "The codes assigned for transactions where no iso_4217 is involved";
        case unique_hash(YER):
            return "Yemeni Rial";
        case unique_hash(ZAR):
            return "Rand";
        case unique_hash(ZMW):
            return "Zambian Kwacha";
        case unique_hash(ZWL):
            return "Zimbabwe Dollar";

        default:
            return (((string_t_() + c.code[0]) + c.code[1]) + c.code[2]);
        }
    }
}
#endif //ESL_CURRENCIES_HPP
