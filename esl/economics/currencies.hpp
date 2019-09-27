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


#include <esl/economics/currency.hpp>

#include <utility>
#include <string>


namespace esl::economics {
    ///
    /// \brief ISO 4217 currencies and codes
    ///
    namespace currencies {

        constexpr currency AED = currency({'A', 'E', 'D'});
        constexpr currency AFN = currency({'A', 'F', 'N'});
        constexpr currency ALL = currency({'A', 'L', 'L'});
        constexpr currency AMD = currency({'A', 'M', 'D'});
        constexpr currency ANG = currency({'A', 'N', 'G'});
        constexpr currency AOA = currency({'A', 'O', 'A'});
        constexpr currency ARS = currency({'A', 'R', 'S'});
        constexpr currency AUD = currency({'A', 'U', 'D'});
        constexpr currency AWG = currency({'A', 'W', 'G'});
        constexpr currency AZN = currency({'A', 'Z', 'N'});
        constexpr currency BAM = currency({'B', 'A', 'M'});
        constexpr currency BBD = currency({'B', 'B', 'D'});
        constexpr currency BDT = currency({'B', 'D', 'T'});
        constexpr currency BGN = currency({'B', 'G', 'N'});
        constexpr currency BHD = currency({'B', 'H', 'D'}, 1000);
        constexpr currency BIF = currency({'B', 'I', 'F'}, 1);
        constexpr currency BMD = currency({'B', 'M', 'D'});
        constexpr currency BND = currency({'B', 'N', 'D'});
        constexpr currency BOB = currency({'B', 'O', 'B'});
        constexpr currency BOV = currency({'B', 'O', 'V'});
        constexpr currency BRL = currency({'B', 'R', 'L'});
        constexpr currency BSD = currency({'B', 'S', 'D'});
        constexpr currency BTN = currency({'B', 'T', 'N'});
        constexpr currency BWP = currency({'B', 'W', 'P'});
        constexpr currency BYN = currency({'B', 'Y', 'N'});
        constexpr currency BZD = currency({'B', 'Z', 'D'});
        constexpr currency CAD = currency({'C', 'A', 'D'});
        constexpr currency CDF = currency({'C', 'D', 'F'});
        constexpr currency CHE = currency({'C', 'H', 'E'});
        constexpr currency CHF = currency({'C', 'H', 'F'});
        constexpr currency CHW = currency({'C', 'H', 'W'});
        constexpr currency CLF = currency({'C', 'L', 'F'});
        constexpr currency CLP = currency({'C', 'L', 'P'}, 1);
        constexpr currency CNY = currency({'C', 'N', 'Y'});
        constexpr currency COP = currency({'C', 'O', 'P'});
        constexpr currency COU = currency({'C', 'O', 'U'});
        constexpr currency CRC = currency({'C', 'R', 'C'});
        constexpr currency CUC = currency({'C', 'U', 'C'});
        constexpr currency CUP = currency({'C', 'U', 'P'});
        constexpr currency CVE = currency({'C', 'V', 'E'});
        constexpr currency CZK = currency({'C', 'Z', 'K'});
        constexpr currency DJF = currency({'D', 'J', 'F'}, 1);
        constexpr currency DKK = currency({'D', 'K', 'K'});
        constexpr currency DOP = currency({'D', 'O', 'P'});
        constexpr currency DZD = currency({'D', 'Z', 'D'});
        constexpr currency EGP = currency({'E', 'G', 'P'});
        constexpr currency ERN = currency({'E', 'R', 'N'});
        constexpr currency ETB = currency({'E', 'T', 'B'});
        constexpr currency EUR = currency({'E', 'U', 'R'});
        constexpr currency FJD = currency({'F', 'J', 'D'});
        constexpr currency FKP = currency({'F', 'K', 'P'});
        constexpr currency GBP = currency({'G', 'B', 'P'});
        constexpr currency GEL = currency({'G', 'E', 'L'});
        constexpr currency GHS = currency({'G', 'H', 'S'});
        constexpr currency GIP = currency({'G', 'I', 'P'});
        constexpr currency GMD = currency({'G', 'M', 'D'});
        constexpr currency GNF = currency({'G', 'N', 'F'}, 1);
        constexpr currency GTQ = currency({'G', 'T', 'Q'});
        constexpr currency GYD = currency({'G', 'Y', 'D'});
        constexpr currency HKD = currency({'H', 'K', 'D'});
        constexpr currency HNL = currency({'H', 'N', 'L'});
        constexpr currency HRK = currency({'H', 'R', 'K'});
        constexpr currency HTG = currency({'H', 'T', 'G'});
        constexpr currency HUF = currency({'H', 'U', 'F'});
        constexpr currency IDR = currency({'I', 'D', 'R'});
        constexpr currency ILS = currency({'I', 'L', 'S'});
        constexpr currency INR = currency({'I', 'N', 'R'});
        constexpr currency IQD = currency({'I', 'Q', 'D'}, 1000);
        constexpr currency IRR = currency({'I', 'R', 'R'});
        constexpr currency ISK = currency({'I', 'S', 'K'}, 1);
        constexpr currency JMD = currency({'J', 'M', 'D'});
        constexpr currency JOD = currency({'J', 'O', 'D'}, 1000);
        constexpr currency JPY = currency({'J', 'P', 'Y'}, 1);
        constexpr currency KES = currency({'K', 'E', 'S'});
        constexpr currency KGS = currency({'K', 'G', 'S'});
        constexpr currency KHR = currency({'K', 'H', 'R'});
        constexpr currency KMF = currency({'K', 'M', 'F'}, 1);
        constexpr currency KPW = currency({'K', 'P', 'W'});
        constexpr currency KRW = currency({'K', 'R', 'W'}, 1);
        constexpr currency KWD = currency({'K', 'W', 'D'}, 1000);
        constexpr currency KYD = currency({'K', 'Y', 'D'});
        constexpr currency KZT = currency({'K', 'Z', 'T'});
        constexpr currency LAK = currency({'L', 'A', 'K'});
        constexpr currency LBP = currency({'L', 'B', 'P'});
        constexpr currency LKR = currency({'L', 'K', 'R'});
        constexpr currency LRD = currency({'L', 'R', 'D'});
        constexpr currency LSL = currency({'L', 'S', 'L'});
        constexpr currency LYD = currency({'L', 'Y', 'D'}, 1000);
        constexpr currency MAD = currency({'M', 'A', 'D'});
        constexpr currency MDL = currency({'M', 'D', 'L'});
        constexpr currency MGA = currency({'M', 'G', 'A'});
        constexpr currency MKD = currency({'M', 'K', 'D'});
        constexpr currency MMK = currency({'M', 'M', 'K'});
        constexpr currency MNT = currency({'M', 'N', 'T'});
        constexpr currency MOP = currency({'M', 'O', 'P'});
        constexpr currency MRU = currency({'M', 'R', 'U'});
        constexpr currency MUR = currency({'M', 'U', 'R'});
        constexpr currency MVR = currency({'M', 'V', 'R'});
        constexpr currency MWK = currency({'M', 'W', 'K'});
        constexpr currency MXN = currency({'M', 'X', 'N'});
        constexpr currency MXV = currency({'M', 'X', 'V'});
        constexpr currency MYR = currency({'M', 'Y', 'R'});
        constexpr currency MZN = currency({'M', 'Z', 'N'});
        constexpr currency NAD = currency({'N', 'A', 'D'});
        constexpr currency NGN = currency({'N', 'G', 'N'});
        constexpr currency NIO = currency({'N', 'I', 'O'});
        constexpr currency NOK = currency({'N', 'O', 'K'});
        constexpr currency NPR = currency({'N', 'P', 'R'});
        constexpr currency NZD = currency({'N', 'Z', 'D'});
        constexpr currency OMR = currency({'O', 'M', 'R'}, 1000);
        constexpr currency PAB = currency({'P', 'A', 'B'});
        constexpr currency PEN = currency({'P', 'E', 'N'});
        constexpr currency PGK = currency({'P', 'G', 'K'});
        constexpr currency PHP = currency({'P', 'H', 'P'});
        constexpr currency PKR = currency({'P', 'K', 'R'});
        constexpr currency PLN = currency({'P', 'L', 'N'});
        constexpr currency PYG = currency({'P', 'Y', 'G'}, 1);
        constexpr currency QAR = currency({'Q', 'A', 'R'});
        constexpr currency RON = currency({'R', 'O', 'N'});
        constexpr currency RSD = currency({'R', 'S', 'D'});
        constexpr currency RUB = currency({'R', 'U', 'B'});
        constexpr currency RWF = currency({'R', 'W', 'F'}, 1);
        constexpr currency SAR = currency({'S', 'A', 'R'});
        constexpr currency SBD = currency({'S', 'B', 'D'});
        constexpr currency SCR = currency({'S', 'C', 'R'});
        constexpr currency SDG = currency({'S', 'D', 'G'});
        constexpr currency SEK = currency({'S', 'E', 'K'});
        constexpr currency SGD = currency({'S', 'G', 'D'});
        constexpr currency SHP = currency({'S', 'H', 'P'});
        constexpr currency SLL = currency({'S', 'L', 'L'});
        constexpr currency SOS = currency({'S', 'O', 'S'});
        constexpr currency SRD = currency({'S', 'R', 'D'});
        constexpr currency SSP = currency({'S', 'S', 'P'});
        constexpr currency STN = currency({'S', 'T', 'N'});
        constexpr currency SVC = currency({'S', 'V', 'C'});
        constexpr currency SYP = currency({'S', 'Y', 'P'});
        constexpr currency SZL = currency({'S', 'Z', 'L'});
        constexpr currency THB = currency({'T', 'H', 'B'});
        constexpr currency TJS = currency({'T', 'J', 'S'});
        constexpr currency TMT = currency({'T', 'M', 'T'});
        constexpr currency TND = currency({'T', 'N', 'D'});
        constexpr currency TOP = currency({'T', 'O', 'P'});
        constexpr currency TRY = currency({'T', 'R', 'Y'});
        constexpr currency TTD = currency({'T', 'T', 'D'});
        constexpr currency TWD = currency({'T', 'W', 'D'});
        constexpr currency TZS = currency({'T', 'Z', 'S'});
        constexpr currency UAH = currency({'U', 'A', 'H'});
        constexpr currency UGX = currency({'U', 'G', 'X'}, 1);
        constexpr currency USD = currency({'U', 'S', 'D'});
        constexpr currency USN = currency({'U', 'S', 'N'});
        constexpr currency UYI = currency({'U', 'Y', 'I'});
        constexpr currency UYU = currency({'U', 'Y', 'U'});
        constexpr currency UZS = currency({'U', 'Z', 'S'});
        constexpr currency VEF = currency({'V', 'E', 'F'});
        constexpr currency VND = currency({'V', 'N', 'D'}, 1);
        constexpr currency VUV = currency({'V', 'U', 'V'}, 1);
        constexpr currency WST = currency({'W', 'S', 'T'});
        constexpr currency XAF = currency({'X', 'A', 'F'}, 1);
        constexpr currency XAG = currency({'X', 'A', 'G'});
        constexpr currency XAU = currency({'X', 'A', 'U'});
        constexpr currency XBA = currency({'X', 'B', 'A'});
        constexpr currency XBB = currency({'X', 'B', 'B'});
        constexpr currency XBC = currency({'X', 'B', 'C'});
        constexpr currency XBD = currency({'X', 'B', 'D'});
        constexpr currency XCD = currency({'X', 'C', 'D'});
        constexpr currency XDR = currency({'X', 'D', 'R'});
        constexpr currency XOF = currency({'X', 'O', 'F'}, 1);
        constexpr currency XPD = currency({'X', 'P', 'D'});
        constexpr currency XPF = currency({'X', 'P', 'F'}, 1);
        constexpr currency XPT = currency({'X', 'P', 'T'});
        constexpr currency XSU = currency({'X', 'S', 'U'});
        // constexpr currency XTS = currency({'X', 'T', 'S'});
        constexpr currency XUA = currency({'X', 'U', 'A'});
        // constexpr currency XXX = currency({'X', 'X', 'X'});
        constexpr currency YER = currency({'Y', 'E', 'R'});
        constexpr currency ZAR = currency({'Z', 'A', 'R'});
        constexpr currency ZMW = currency({'Z', 'M', 'W'});
        constexpr currency ZWL = currency({'Z', 'W', 'L'});

    }//namespace countries

    namespace detail {
        constexpr size_t unique_hash(currency c)
        {
            // unique hash
            return             size_t(c.code[0] - 'A')
                        + 26 * size_t(c.code[1] - 'A')
                   + 26 * 26 * size_t(c.code[2] - 'A');
        }
    }  // namespace detail

    template<typename string_t_ = std::string>
    inline string_t_ currency_name(const currency &c)
    {
        using detail::unique_hash;
        using namespace currencies;

        switch(std::hash<currency>()(c)) {
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
        //     return "The codes assigned for transactions where no currency is involved";
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
