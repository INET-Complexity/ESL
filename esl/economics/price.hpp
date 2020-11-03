/// \file   price.hpp
///
/// \brief  The monetary value of something, as quoted in an offer, or agreed on in a transaction
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
#ifndef ESL_PRICE_HPP
#define ESL_PRICE_HPP

#include <iomanip>
#include <cmath>

#include <esl/economics/iso_4217.hpp>


namespace esl::economics {
    namespace detail {
        constexpr void assert_equal_currencies(const iso_4217 &first,
                                               const iso_4217 &second)
        {
            if(first != second) {
                throw std::invalid_argument("comparing price of with currencies");
            }
        }
    }

    ///
    /// A price is a quote in terms of monetary value.
    ///
    struct price
    {
        std::int64_t value;

        iso_4217 valuation;

        ///
        /// \todo: this should be redefined as price.from_units or something similar
        ///
        /// \param value
        /// \param valuation
        explicit constexpr price(int64_t value = 0,
                                 iso_4217 valuation = iso_4217())
        : value(value), valuation(valuation)
        {

        }

        ///
        /// \brief  Converts a floating point value by truncation
        ///
        /// \param value_untruncated
        /// \param valuation
        explicit constexpr price(float value_untruncated,
                                 const iso_4217& valuation = iso_4217())
        : price(static_cast<int64_t>(value_untruncated * static_cast<int64_t>(valuation.denominator)), valuation)
        {

        }


        explicit constexpr price(double value_untruncated,
                                 const iso_4217& valuation = iso_4217())
                : price(static_cast<int64_t>(value_untruncated * valuation.denominator), valuation)
        {

        }


        constexpr price(const price &p)
        : price(p.value, p.valuation)
        {

        }

        constexpr price(price &&p) noexcept
        : price(std::move(p.value)
        , std::move(p.valuation))
        {

        }


        constexpr price(double value_untruncated, const price &similar)
            : price(value_untruncated, similar.valuation)
        {

        }


        price &operator = (const price &p)
        {
            value     = p.value;
            valuation = p.valuation;
            return *this;
        }

        [[nodiscard]] constexpr bool operator==(const price &operand) const
        {
            return (value == operand.value) && (valuation == operand.valuation);
        }

        [[nodiscard]] constexpr bool operator!=(const price &operand) const
        {
            return (value != operand.value) || (valuation != operand.valuation);
        }

        [[nodiscard]] constexpr bool operator < (const price &operand) const
        {
            detail::assert_equal_currencies(valuation, operand.valuation);
            return value < operand.value;
        }

        [[nodiscard]] constexpr bool operator<=(const price &operand) const
        {
            return (*this < operand) || (*this == operand);
        }

        [[nodiscard]] constexpr bool operator>(const price &operand) const
        {
            detail::assert_equal_currencies(valuation, operand.valuation);
            return value > operand.value;
        }

        [[nodiscard]] constexpr bool operator>=(const price &operand) const
        {
            return (*this > operand) || (*this == operand);
        }

        [[nodiscard]] constexpr price operator+(const price &operand) const
        {
            assert(valuation == operand.valuation);
            return price(value + operand.value, valuation);
        }

        constexpr price &operator += (const price &operand)
        {
            assert(valuation == operand.valuation);
            value += operand.value;
            return *this;
        }


        [[nodiscard]] constexpr price operator - () const
        {
            return price(-value, valuation);
        }

        [[nodiscard]] constexpr price operator-(const price &operand) const
        {
            assert(valuation == operand.valuation);
            return price(value - operand.value, valuation);
        }

        constexpr price &operator-=(const price &operand)
        {
            assert(valuation == operand.valuation);
            value -= operand.value;
            return *this;
        }

        template<typename integer_t_>
        [[nodiscard]] constexpr price operator*(const integer_t_ &scalar) const
        {
            static_assert(std::is_integral<integer_t_>::value);
            return price(value * scalar, valuation);
        }

        [[nodiscard]] constexpr friend price operator*(const price &p, const uint64_t &operand)
        {
            return price(static_cast<int64_t>(p.value * operand), p.valuation);
        }
;
        [[nodiscard]] constexpr friend price operator*(const uint64_t &operand, const price &p)
        {
            return p * operand;
        }

        [[nodiscard]] explicit operator double() const
        {
            return double(value) / valuation.denominator;
        }

        std::ostream &operator << (std::ostream &o) const
        {
            std::ios_base::fmtflags flags_(o.flags());
            int precision_ = static_cast<int>(ceil(log10(valuation.denominator)));
            o << this->valuation << '(' << std::fixed
              << std::setprecision(precision_) << double(*this) << ')';
            o.flags(flags_);
            return o;
        }

        friend std::ostream &operator << (std::ostream &o, const price &p)
        {
            return p.operator<<(o);
        }

        //friend std::istream &operator>>(std::istream &i, const price &p)
        //{
        //    std::array<char, 3> iso_currency_code_;
        //    i.read(iso_currency_code_.data(), iso_currency_code_.size());
        //    std::int64_t amount_;
        //    i >> amount_;
        //    p = price(amount_, iso_currency_code_);
        //    return i;
        //}



        template<class archive_t>
        void serialize(archive_t &archive, const unsigned int version)
        {
            (void)version;
            //archive &BOOST_SERIALIZATION_NVP(value);
            //archive &boost::serialization::make_nvp(
            //    "valuation", const_cast<iso_4217 &>(valuation));


            std::stringstream stream_;
            stream_ << valuation.code[0];
            stream_ << valuation.code[1];
            stream_ << valuation.code[2];
            stream_ << ' ' << value << '/' << valuation.denominator;

            std::string res_ = stream_.str();

            archive & boost::serialization::make_nvp("price", res_);
        }
    };
}


namespace boost::serialization {
        /*template<typename archive_t>
        inline void save_construct_data( archive_t &archive, const esl::economics::price * t
                                       , const unsigned int file_version)
       {
            (void) file_version;
           (void) archive;
            //archive << boost::serialization::make_nvp("value", t->value);
            //archive << boost::serialization::make_nvp("valuation", t->valuation);

        }*/

        template<typename archive_t>
        inline void load_construct_data(
                archive_t &archive, esl::economics::price * t, const unsigned int file_version
        ){
            (void) file_version;
            std::int64_t value_ = 0;
            archive >> value_;
            esl::economics::iso_4217 code_;
            archive >> code_;
            ::new(t)esl::economics::price(value_, code_);
        }
} // namespace boost::serialization


#ifdef WITH_MPI
namespace boost::mpi {
    template<>
    struct is_mpi_datatype<esl::economics::price>
    : public mpl::true_
    {

    };
}
#endif//WITH_MPI







//#include <esl/economics/currencies.hpp>




//#include <locale>

/*

constexpr esl::economics::price operator "" _AED(const char *p)
{

    //std::use_facet< std::numpunct<char> >(std::cout.getloc()).decimal_point()

    enum parser_states{
        integer_part,
        decimal_part
    }state_;


    for(const char *i =p; *i != '\0'; ++i){

    }


    auto x = esl::economics::currencies::AED;

    auto z = std::atof(p);
    return esl::economics::price(123, esl::economics::currencies::AED);
}


"AED" | AED;
"AFN" | AFN;
"ALL" | ALL;
"AMD" | AMD;
"ANG" | ANG;
"AOA" | AOA;
"ARS" | ARS;
"AUD" | AUD;
"AWG" | AWG;
"AZN" | AZN;
"BAM" | BAM;
"BBD" | BBD;
"BDT" | BDT;
"BGN" | BGN;
"BHD" | BHD;
"BIF" | BIF;
"BMD" | BMD;
"BND" | BND;
"BOB" | BOB;
"BOV" | BOV;
"BRL" | BRL;
"BSD" | BSD;
"BTN" | BTN;
"BWP" | BWP;
"BYN" | BYN;
"BZD" | BZD;
"CAD" | CAD;
"CDF" | CDF;
"CHE" | CHE;
"CHF" | CHF;
"CHW" | CHW;
"CLF" | CLF;
"CLP" | CLP;
"CNY" | CNY;
"COP" | COP;
"COU" | COU;
"CRC" | CRC;
"CUC" | CUC;
"CUP" | CUP;
"CVE" | CVE;
"CZK" | CZK;
"DJF" | DJF;
"DKK" | DKK;
"DOP" | DOP;
"DZD" | DZD;
"EGP" | EGP;
"ERN" | ERN;
"ETB" | ETB;
"EUR" | EUR;
"FJD" | FJD;
"FKP" | FKP;
"GBP" | GBP;
"GEL" | GEL;
"GHS" | GHS;
"GIP" | GIP;
"GMD" | GMD;
"GNF" | GNF;
"GTQ" | GTQ;
"GYD" | GYD;
"HKD" | HKD;
"HNL" | HNL;
"HRK" | HRK;
"HTG" | HTG;
"HUF" | HUF;
"IDR" | IDR;
"ILS" | ILS;
"INR" | INR;
"IQD" | IQD;
"IRR" | IRR;
"ISK" | ISK;
"JMD" | JMD;
"JOD" | JOD;
"JPY" | JPY;
"KES" | KES;
"KGS" | KGS;
"KHR" | KHR;
"KMF" | KMF;
"KPW" | KPW;
"KRW" | KRW;
"KWD" | KWD;
"KYD" | KYD;
"KZT" | KZT;
"LAK" | LAK;
"LBP" | LBP;
"LKR" | LKR;
"LRD" | LRD;
"LSL" | LSL;
"LYD" | LYD;
"MAD" | MAD;
"MDL" | MDL;
"MGA" | MGA;
"MKD" | MKD;
"MMK" | MMK;
"MNT" | MNT;
"MOP" | MOP;
"MRU" | MRU;
"MUR" | MUR;
"MVR" | MVR;
"MWK" | MWK;
"MXN" | MXN;
"MXV" | MXV;
"MYR" | MYR;
"MZN" | MZN;
"NAD" | NAD;
"NGN" | NGN;
"NIO" | NIO;
"NOK" | NOK;
"NPR" | NPR;
"NZD" | NZD;
"OMR" | OMR;
"PAB" | PAB;
"PEN" | PEN;
"PGK" | PGK;
"PHP" | PHP;
"PKR" | PKR;
"PLN" | PLN;
"PYG" | PYG;
"QAR" | QAR;
"RON" | RON;
"RSD" | RSD;
"RUB" | RUB;
"RWF" | RWF;
"SAR" | SAR;
"SBD" | SBD;
"SCR" | SCR;
"SDG" | SDG;
"SEK" | SEK;
"SGD" | SGD;
"SHP" | SHP;
"SLL" | SLL;
"SOS" | SOS;
"SRD" | SRD;
"SSP" | SSP;
"STN" | STN;
"SVC" | SVC;
"SYP" | SYP;
"SZL" | SZL;
"THB" | THB;
"TJS" | TJS;
"TMT" | TMT;
"TND" | TND;
"TOP" | TOP;
"TRY" | TRY;
"TTD" | TTD;
"TWD" | TWD;
"TZS" | TZS;
"UAH" | UAH;
"UGX" | UGX;
"USD" | USD;
"USN" | USN;
"UYI" | UYI;
"UYU" | UYU;
"UZS" | UZS;
"VEF" | VEF;
"VND" | VND;
"VUV" | VUV;
"WST" | WST;
"XAF" | XAF;
"XAG" | XAG;
"XAU" | XAU;
"XBA" | XBA;
"XBB" | XBB;
"XBC" | XBC;
"XBD" | XBD;
"XCD" | XCD;
"XDR" | XDR;
"XOF" | XOF;
"XPD" | XPD;
"XPF" | XPF;
"XPT" | XPT;
"XSU" | XSU;
"XUA" | XUA;
"YER" | YER;
"ZAR" | ZAR;
"ZMW" | ZMW;
"ZWL" | ZWL;
*/











#endif//ESL_PRICE_HPP
