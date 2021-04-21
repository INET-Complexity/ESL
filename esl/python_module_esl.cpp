/// \file   python_module.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-08-30
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
#include <esl/python_module_esl.hpp>


#ifdef WITH_PYTHON
using namespace boost::python;

// todo: reorganize

#include <esl/simulation/python_module_simulation.hpp>
#include <esl/simulation/model.hpp>
// todo: reorganize










// esl top level
#include <esl/agent.hpp>
#include <esl/exception.hpp>
#include <esl/version.hpp>
#include <esl/quantity.hpp>
using namespace esl;

// esl.computation
#include <esl/computation/python_module_computation.hpp>
#include <esl/computation/block_pool.hpp>
#include <esl/computation/environment.hpp>
#include <esl/computation/timing.hpp>
using esl::computation::agent_timing;


//esl.economics
#include <esl/economics/iso_4217.hpp>
#include <esl/economics/currencies.hpp>
#include <esl/economics/price.hpp>
using namespace esl::economics;

std::string python_currency_code(const iso_4217 &c)
{
    return (std::string() + c.code[0]) + c.code[1];
}

double python_price_to_floating_point(const price &p)
{
    return double(p);
}

//boost::python::list python_company_unique_shareholders(const company &c)
//{
//    boost::python::list result_;
//    for(const auto &s : c.unique_shareholders()){
//        result_.append( esl::simulation::python_module::python_identity(s.digits) );
//    }
//    return result_;
//}


////////////////////////////////////////////////////////////////////////////////
// esl.economics.finance
////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/finance/share_class.hpp>
#include <esl/economics/finance/isin.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
std::string get_isin_code(const finance::isin &i)
{
    return std::string(i.code.data(), i.code.data() + i.code.size());
}


void set_isin_code(finance::isin &i, const std::string &code)
{
    i.code = esl::to_array<0,9,char>(code);
}

////////////////////////////////////////////////////////////////////////////////
// esl.economics.markets.order_book
////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/markets/order_book/basic_book.hpp>
#include <esl/economics/markets/order_book/binary_tree_order_book.hpp>
#include <esl/economics/markets/order_book/static_order_book.hpp>
#include <esl/economics/markets/order_book/matching_engine.hpp>
#include <esl/economics/markets/order_book/python_module_order_book.hpp>
using namespace esl::economics::markets::order_book;


namespace esl {
//    static boost::shared_ptr<agent> python_construct_agent( object const &o )
//    {
//        boost::python::extract<esl::simulation::python_module::python_identity> e(o);
//        return boost::make_shared<agent>(identity<agent>(e().digits));
//    }

///
/// \brief  Translates C++ exceptions to Python errors, by setting the
///         Python error text equal to the C++ exception message.
///
/// \details    This is a fallback converter for all exceptions,
///             we can design more friendly converters
///
/// \param e
void translate_exception(const exception &e)
{
    PyErr_SetString (   PyExc_RuntimeError
                        //PyExc_UserWarning
                    , e.what()
                    );
}

scope create_scope(const std::string &name)
{
    std::string nested_name = extract<std::string>(scope().attr("__name__") + "." + name);
    object nested_module(handle<>(borrowed(PyImport_AddModule(nested_name.c_str()))));
    scope().attr(name.c_str()) = nested_module;
    scope parent = nested_module;
    return parent;
}

///
/// \brief  Python module structure follows directory structure and
///         C++ namespaces. We use __init__.py for submodules, so we can mix
///         native code and Python-binding specific Python code. Thus, we
///         protect the name of the module with an underscore, and import it
///         from __init__.py
///
BOOST_PYTHON_MODULE(_esl)
{
    class_<esl::exception>("exception", init<std::string>())
        .def("message", &esl::exception::what)
        ;

    register_exception_translator<esl::exception>(translate_exception);

    class_<quantity>(
        "quantity",init<>())
        .def(init<std::uint64_t>())
        .def(self += self)
        .def(self + self)
        .def(self -= self)
        .def(self - self)
        //.def(self *= self)
        .def(self * self)

        .def(self < self)
        .def(self > self)
        .def(self == self)
        .def(self != self)
        .def(self <= self)
        .def(self >= self)
            //.def(self /= self)
            //.def(self / self)
            //.def(self %= self)
            //.def(self % self)
            // conversion to floating point type
        .def(float_(self))
        .def("__repr__", &quantity::representation)
        .def("__str__", &quantity::representation)
        .def(self *= std::uint64_t())
        .def(self * std::uint64_t())
        .def(self / std::uint64_t())
        ;

//        class_<agent>("agent")
//            .def("__init__", boost::python::make_constructor(&python_construct_agent))
//            ;

    def("version", version, "Print the library's version.");

    ////////////////////////////////////////////////////////////////////////////
    // esl.computation
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_computation_ = create_scope("computation");

        class_<computation::block_pool::block<object>>("block"
                                                      , "The block is a unit of memory used by the block_pool allocator, and has fixed size.")
            .def_readwrite("data", &computation::block_pool::block<object>::data)
            .def_readwrite("index", &computation::block_pool::block<object>::index)
            ;

        // computational environment base class with default single thread
        class_<computation::python_environment>( "environment"
                                               , "The environment class runs models: it schedules agents and delivers messages sent between agents.")
            .def("step", &computation::python_environment::step)
            .def("run", &computation::python_environment::run)
            .def("activate", &computation::python_environment::activate)
            .def("deactivate", &computation::python_environment::deactivate)
            .def("before_step", &computation::python_environment::before_step)
            .def("after_step", &computation::python_environment::after_step)
            .def("after_run", &computation::python_environment::after_run)
            .def("activate_agent", &computation::python_environment::activate_agent)
            .def("deactivate_agent", &computation::python_environment::deactivate_agent)
            .def("send_messages", &computation::python_environment::send_messages)
            ;

        // timing information
        class_<computation::agent_timing>("agent_timing", "Contains performance metrics for individual agents.")
            .def_readwrite("messaging", &agent_timing::messaging)
            .def_readwrite("acting", &agent_timing::acting);


        {
            boost::python::scope scope_distributed_ = create_scope("distributed");

        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // esl.data
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_data_ = create_scope("data");



        ////////////////////////////////////////////////////////////////////////
        // esl.format
        ////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_format_ = create_scope("format");

        }

    }


    ////////////////////////////////////////////////////////////////////////////
    // esl.economics
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_economics_ = create_scope("economics");

        class_<iso_4217>("iso_4217")
            .add_property("code", python_currency_code)
            .add_property("denominator", &iso_4217::denominator)
            .add_property("__repr__", python_currency_code)
            .add_property("__str__", python_currency_code)
            ;

        scope().attr("AED") = currencies::AED;
        scope().attr("AFN") = currencies::AFN;
        scope().attr("ALL") = currencies::ALL;
        scope().attr("AMD") = currencies::AMD;
        scope().attr("ANG") = currencies::ANG;
        scope().attr("AOA") = currencies::AOA;
        scope().attr("ARS") = currencies::ARS;
        scope().attr("AUD") = currencies::AUD;
        scope().attr("AWG") = currencies::AWG;
        scope().attr("AZN") = currencies::AZN;
        scope().attr("BAM") = currencies::BAM;
        scope().attr("BBD") = currencies::BBD;
        scope().attr("BDT") = currencies::BDT;
        scope().attr("BGN") = currencies::BGN;
        scope().attr("BHD") = currencies::BHD;
        scope().attr("BIF") = currencies::BIF;
        scope().attr("BMD") = currencies::BMD;
        scope().attr("BND") = currencies::BND;
        scope().attr("BOB") = currencies::BOB;
        scope().attr("BOV") = currencies::BOV;
        scope().attr("BRL") = currencies::BRL;
        scope().attr("BSD") = currencies::BSD;
        scope().attr("BTN") = currencies::BTN;
        scope().attr("BWP") = currencies::BWP;
        scope().attr("BYN") = currencies::BYN;
        scope().attr("BZD") = currencies::BZD;
        scope().attr("CAD") = currencies::CAD;
        scope().attr("CDF") = currencies::CDF;
        scope().attr("CHE") = currencies::CHE;
        scope().attr("CHF") = currencies::CHF;
        scope().attr("CHW") = currencies::CHW;
        scope().attr("CLF") = currencies::CLF;
        scope().attr("CLP") = currencies::CLP;
        scope().attr("CNY") = currencies::CNY;
        scope().attr("COP") = currencies::COP;
        scope().attr("COU") = currencies::COU;
        scope().attr("CRC") = currencies::CRC;
        scope().attr("CUC") = currencies::CUC;
        scope().attr("CUP") = currencies::CUP;
        scope().attr("CVE") = currencies::CVE;
        scope().attr("CZK") = currencies::CZK;
        scope().attr("DJF") = currencies::DJF;
        scope().attr("DKK") = currencies::DKK;
        scope().attr("DOP") = currencies::DOP;
        scope().attr("DZD") = currencies::DZD;
        scope().attr("EGP") = currencies::EGP;
        scope().attr("ERN") = currencies::ERN;
        scope().attr("ETB") = currencies::ETB;
        scope().attr("EUR") = currencies::EUR;
        scope().attr("FJD") = currencies::FJD;
        scope().attr("FKP") = currencies::FKP;
        scope().attr("GBP") = currencies::GBP;
        scope().attr("GEL") = currencies::GEL;
        scope().attr("GHS") = currencies::GHS;
        scope().attr("GIP") = currencies::GIP;
        scope().attr("GMD") = currencies::GMD;
        scope().attr("GNF") = currencies::GNF;
        scope().attr("GTQ") = currencies::GTQ;
        scope().attr("GYD") = currencies::GYD;
        scope().attr("HKD") = currencies::HKD;
        scope().attr("HNL") = currencies::HNL;
        scope().attr("HRK") = currencies::HRK;
        scope().attr("HTG") = currencies::HTG;
        scope().attr("HUF") = currencies::HUF;
        scope().attr("IDR") = currencies::IDR;
        scope().attr("ILS") = currencies::ILS;
        scope().attr("INR") = currencies::INR;
        scope().attr("IQD") = currencies::IQD;
        scope().attr("IRR") = currencies::IRR;
        scope().attr("ISK") = currencies::ISK;
        scope().attr("JMD") = currencies::JMD;
        scope().attr("JOD") = currencies::JOD;
        scope().attr("JPY") = currencies::JPY;
        scope().attr("KES") = currencies::KES;
        scope().attr("KGS") = currencies::KGS;
        scope().attr("KHR") = currencies::KHR;
        scope().attr("KMF") = currencies::KMF;
        scope().attr("KPW") = currencies::KPW;
        scope().attr("KRW") = currencies::KRW;
        scope().attr("KWD") = currencies::KWD;
        scope().attr("KYD") = currencies::KYD;
        scope().attr("KZT") = currencies::KZT;
        scope().attr("LAK") = currencies::LAK;
        scope().attr("LBP") = currencies::LBP;
        scope().attr("LKR") = currencies::LKR;
        scope().attr("LRD") = currencies::LRD;
        scope().attr("LSL") = currencies::LSL;
        scope().attr("LYD") = currencies::LYD;
        scope().attr("MAD") = currencies::MAD;
        scope().attr("MDL") = currencies::MDL;
        scope().attr("MGA") = currencies::MGA;
        scope().attr("MKD") = currencies::MKD;
        scope().attr("MMK") = currencies::MMK;
        scope().attr("MNT") = currencies::MNT;
        scope().attr("MOP") = currencies::MOP;
        scope().attr("MRU") = currencies::MRU;
        scope().attr("MUR") = currencies::MUR;
        scope().attr("MVR") = currencies::MVR;
        scope().attr("MWK") = currencies::MWK;
        scope().attr("MXN") = currencies::MXN;
        scope().attr("MXV") = currencies::MXV;
        scope().attr("MYR") = currencies::MYR;
        scope().attr("MZN") = currencies::MZN;
        scope().attr("NAD") = currencies::NAD;
        scope().attr("NGN") = currencies::NGN;
        scope().attr("NIO") = currencies::NIO;
        scope().attr("NOK") = currencies::NOK;
        scope().attr("NPR") = currencies::NPR;
        scope().attr("NZD") = currencies::NZD;
        scope().attr("OMR") = currencies::OMR;
        scope().attr("PAB") = currencies::PAB;
        scope().attr("PEN") = currencies::PEN;
        scope().attr("PGK") = currencies::PGK;
        scope().attr("PHP") = currencies::PHP;
        scope().attr("PKR") = currencies::PKR;
        scope().attr("PLN") = currencies::PLN;
        scope().attr("PYG") = currencies::PYG;
        scope().attr("QAR") = currencies::QAR;
        scope().attr("RON") = currencies::RON;
        scope().attr("RSD") = currencies::RSD;
        scope().attr("RUB") = currencies::RUB;
        scope().attr("RWF") = currencies::RWF;
        scope().attr("SAR") = currencies::SAR;
        scope().attr("SBD") = currencies::SBD;
        scope().attr("SCR") = currencies::SCR;
        scope().attr("SDG") = currencies::SDG;
        scope().attr("SEK") = currencies::SEK;
        scope().attr("SGD") = currencies::SGD;
        scope().attr("SHP") = currencies::SHP;
        scope().attr("SLL") = currencies::SLL;
        scope().attr("SOS") = currencies::SOS;
        scope().attr("SRD") = currencies::SRD;
        scope().attr("SSP") = currencies::SSP;
        scope().attr("STN") = currencies::STN;
        scope().attr("SVC") = currencies::SVC;
        scope().attr("SYP") = currencies::SYP;
        scope().attr("SZL") = currencies::SZL;
        scope().attr("THB") = currencies::THB;
        scope().attr("TJS") = currencies::TJS;
        scope().attr("TMT") = currencies::TMT;
        scope().attr("TND") = currencies::TND;
        scope().attr("TOP") = currencies::TOP;
        scope().attr("TRY") = currencies::TRY;
        scope().attr("TTD") = currencies::TTD;
        scope().attr("TWD") = currencies::TWD;
        scope().attr("TZS") = currencies::TZS;
        scope().attr("UAH") = currencies::UAH;
        scope().attr("UGX") = currencies::UGX;
        scope().attr("USD") = currencies::USD;
        scope().attr("USN") = currencies::USN;
        scope().attr("UYI") = currencies::UYI;
        scope().attr("UYU") = currencies::UYU;
        scope().attr("UZS") = currencies::UZS;
        scope().attr("VEF") = currencies::VEF;
        scope().attr("VND") = currencies::VND;
        scope().attr("VUV") = currencies::VUV;
        scope().attr("WST") = currencies::WST;
        scope().attr("XAF") = currencies::XAF;
        scope().attr("XAG") = currencies::XAG;
        scope().attr("XAU") = currencies::XAU;
        scope().attr("XBA") = currencies::XBA;
        scope().attr("XBB") = currencies::XBB;
        scope().attr("XBC") = currencies::XBC;
        scope().attr("XBD") = currencies::XBD;
        scope().attr("XCD") = currencies::XCD;
        scope().attr("XDR") = currencies::XDR;
        scope().attr("XOF") = currencies::XOF;
        scope().attr("XPD") = currencies::XPD;
        scope().attr("XPF") = currencies::XPF;
        scope().attr("XPT") = currencies::XPT;
        scope().attr("XSU") = currencies::XSU;
    // constexpr iso_4217 XTS
        scope().attr("XUA") = currencies::XUA;
    // scope().attr("XXX") = currencies::XXX;
        scope().attr("YER") = currencies::YER;
        scope().attr("ZAR") = currencies::ZAR;
        scope().attr("ZMW") = currencies::ZMW;
        scope().attr("ZWL") = currencies::ZWL;

        class_<price>("price", init<std::int64_t, iso_4217>())
        .add_property("value", &price::value)
        .add_property("valuation", &price::valuation)
            .def("approximate", &price::approximate)
            .def(self == self)
            .def(self != self)
            .def(self  < self)
            .def(self <= self)
            .def(self  > self)
            .def(self >= self)
            .def("__repr__", &price::representation)
            .def("__str__", &price::representation)
            .def("__float__", &python_price_to_floating_point)
            ;




        ////////////////////////////////////////////////////////////////////////
        // esl.economics.accounting
        ////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_accounting_ = create_scope("accounting");

        }

        ////////////////////////////////////////////////////////////////////////
        // esl.economics.finance
        ////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_finance = create_scope("finance");




            class_<finance::isin>("isin", init<geography::iso_3166_1_alpha_2, std::string>())
                .add_property("issuer", &finance::isin::issuer)
                .add_property("code", &get_isin_code, &set_isin_code)
                .def("__repr__", &finance::isin::representation)
                .def("__str__", &finance::isin::representation)
                ;

            class_<finance::share_class>("share_class", init<std::uint8_t, std::uint8_t, float, bool, bool, bool>())
                .add_property("rank", &finance::share_class::rank)
                .add_property("votes", &finance::share_class::votes)
                .add_property("preference", &finance::share_class::preference)
                .add_property("dividend", &finance::share_class::dividend)
                .add_property("cumulative", &finance::share_class::cumulative)
                .add_property("redeemable", &finance::share_class::redeemable)
                .def(self == self)
                .def(self < self)
                ;
            

        }

        ////////////////////////////////////////////////////////////////////////
        // esl.economics.markets
        ////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_markets_ = create_scope("markets");

            ////////////////////////////////////////////////////////////////////
            // esl.economics.markets.impact_function
            ////////////////////////////////////////////////////////////////////
            {
                boost::python::scope scope_impact_function_ = create_scope("impact_function");
            }


            ////////////////////////////////////////////////////////////////////
            // esl.economics.markets.order_book
            ////////////////////////////////////////////////////////////////////
            {
                boost::python::scope scope_order_book_ = create_scope("order_book");

                enum_<execution_report::state_t>("execution_state")
                    .value("invalid", execution_report::state_t::invalid)
                    .value("cancel", execution_report::state_t::cancel)
                    .value("match", execution_report::state_t::match)
                    .value("placement", execution_report::state_t::placement)
                    ;

                class_<execution_report>("execution_report")
                    .def_readwrite("quantity", &execution_report::quantity)
                    .def_readwrite("identifier", &execution_report::identifier)
                    .def_readwrite("side", &execution_report::side)
                    .def_readwrite("limit", &execution_report::limit)
                    .def_readwrite("owner", &execution_report::owner)
                    .def("__repr__", &execution_report::representation)
                    .def("__str__", &execution_report::representation)
                    ;

                ///
                /// \brief Export the abstract base class, so that python users too can
                ///        implement new order books.
                ///
                class_<basic_book, boost::noncopyable>("basic_book", no_init)
                    .def_readwrite("reports", &basic_book::reports)
                    .def("ask", &basic_book::ask)
                    .def("bid", &basic_book::bid)
                    .def("insert", &basic_book::insert)
                    .def("cancel", &basic_book::cancel)
                    .def("display", &basic_book::display)
                    ;

                ///
                /// \brief Export the abstract base class, so that python users too can
                ///        implement new order books.
                ///
                class_<basic_book, boost::noncopyable>("basic_book", no_init)
                    .def_readwrite("reports", &basic_book::reports)
                    .def("ask", &basic_book::ask)
                    .def("bid", &basic_book::bid)
                    .def("insert", &basic_book::insert)
                    .def("cancel", &basic_book::cancel)
                    .def("display", &basic_book::display)
                    ;

                class_<static_order_book, bases<basic_book>>("static_order_book", init<markets::quote, markets::quote, size_t>())
                    .def_readwrite("reports", &basic_book::reports)
                    .def("ask", &basic_book::ask)
                    .def("bid", &basic_book::bid)
                    .def("insert", &basic_book::insert)
                    .def("cancel", &basic_book::cancel)
                    .def("display", &basic_book::display)
                    ;

                class_<binary_tree_order_book, bases<basic_book>>("binary_tree_order_book", init<>())
                    .def_readwrite("reports", &basic_book::reports)
                    .def("ask", &basic_book::ask)
                    .def("bid", &basic_book::bid)
                    .def("insert", &basic_book::insert)
                    .def("cancel", &basic_book::cancel)
                    .def("display", &basic_book::display)
                    ;

                class_<matching_engine>("matching_engine", init<>())
                    .def_readwrite("books", &matching_engine::books)
                    //.def("insert", &basic_book::insert)
                    //.def("cancel", &basic_book::cancel)
                    ;

            }


            ////////////////////////////////////////////////////////////////////
            // esl.economics.markets.walras
            ////////////////////////////////////////////////////////////////////
            {
                boost::python::scope scope_walras_ = create_scope("walras");
            }

        }

    }
}

}//namespace esl

#endif