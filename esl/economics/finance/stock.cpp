/// \file   stock.cpp
///
/// \brief  
///
/// \authors    maarten
/// \date       2019-06-10
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

#include <esl/economics/finance/stock.hpp>

#include <esl/economics/finance/company.hpp>

namespace esl::economics::finance {

    ///
    /// \return     A fictional code from company details (any matches to the real world are coincidental).
    ///             There can be collisions in the space of 9^12 possible ISIN codes per country.
    // constexpr
    isin create_isin(geography::country issuer, const identity<company> &c,
                     share s)
    {
        constexpr std::array<char, 36> table_ = {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
            'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
            'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};


        // size_t integer_ = hash<identity<company>>()(c);
        // hash_combine(integer_ , hash<share>()(s)) ; // not constexpr

        // we guarantee up to 1000 different issues per company
        size_t integer_ = 0;
        if(!c.digits.empty()) {
            integer_ = c.digits.back() * 1000;
        }

        integer_ += s.rank;

        std::array<char, 9> code_ = {};

        for(size_t i = 0; i < code_.size(); ++i) {
            code_[code_.size() - i - 1] = table_[integer_ % table_.size()];
            integer_ /= table_.size();
        }

        return isin(issuer, code_);
    }

    identity<law::property>
    create_share_identifier(identity<company> company_identifier, share details)
    {
        std::vector<std::uint64_t> result_;
        for(auto d : company_identifier.digits) {
            result_.push_back(d);
        }
        result_.push_back(details.rank);
        return identity<law::property>(result_);
    }

    stock::stock() : security()
    {

    }

    stock::stock(company &issuer, const share &details)
    : stock::stock(issuer.create<property>(),
                   issuer.primary_jurisdiction.sovereign,
                   (identity<company>)issuer, details)
    {

    }

    stock::stock(const identity<property> &pi, geography::country c,
                 const identity<company> &i, const share &s)
    : stock(pi, i, s, create_isin(c, i, s))
    {

    }


    stock::stock(const identity<property> &pi,
                 const identity<company> &company_identifier,
                 const share &details, const isin &code)
    : property(pi)
    , security(pi, code)
    , company_identifier(company_identifier)
    , details(details)
    {}
}


#ifdef WITH_PYTHON
#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(stock)
{
    class_<esl::economics::finance::stock>
        ( "stock", init<>())
        .def(init<esl::economics::company&, const esl::economics::finance::share &>())
        .def(init<esl::identity<esl::law::property>, esl::geography::country, esl::identity<esl::economics::company>, esl::economics::finance::share>())
        .def(init<esl::identity<esl::law::property>, esl::identity<esl::economics::company>, esl::economics::finance::share, esl::economics::finance::isin>())
        .def("name", &esl::economics::finance::stock::name)
        .def(self < self)
        ;
}

#endif  // WITH_PYTHON