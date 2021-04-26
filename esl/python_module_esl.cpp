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


////////////////////////////////////////////////////////////////////////////////
// _utility
////////////////////////////////////////////////////////////////////////////////
template<typename object_t_>
void do_release(typename boost::shared_ptr<object_t_> const &, object_t_ *)
{
}

template<typename object_t_>
typename std::shared_ptr<object_t_> to_std(typename boost::shared_ptr<object_t_> const& p)
{
    return std::shared_ptr<object_t_>(p.get(), [p](auto &&PH1)
    {
        return do_release<object_t_>(p, std::forward<decltype(PH1)>(PH1));
    });

}

template<typename object_t_>
boost::shared_ptr<object_t_> to_boost(std::shared_ptr<object_t_>& ptr)
{
    return boost::shared_ptr<object_t_>(ptr.get(), [ptr](object_t_ *) mutable
    {
        ptr.reset();
    });
}




// todo: reorganize

#include <esl/simulation/python_module_simulation.hpp>
#include <esl/simulation/model.hpp>
// todo: reorganize



////////////////////////////////////////////////////////////////////////////////
// esl top level
////////////////////////////////////////////////////////////////////////////////
#include <esl/agent.hpp>
#include <esl/exception.hpp>
#include <esl/version.hpp>
#include <esl/quantity.hpp>
using namespace esl;

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

////////////////////////////////////////////////////////////////////////////////
// esl.computation
////////////////////////////////////////////////////////////////////////////////
#include <esl/computation/python_module_computation.hpp>
#include <esl/computation/block_pool.hpp>
#include <esl/computation/environment.hpp>
#include <esl/computation/timing.hpp>
using esl::computation::agent_timing;


////////////////////////////////////////////////////////////////////////////////
//esl.economics
////////////////////////////////////////////////////////////////////////////////
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
// esl.economics.markets
////////////////////////////////////////////////////////////////////////////////

#include <esl/economics/markets/quote.hpp>
#include <esl/economics/markets/iso_10383.hpp>
using namespace esl::economics::markets;


price quote_helper_get_price(const quote &q)
{
    auto p = std::get_if<price>(&q.type);
    return *p;
}


void quote_helper_set_price(quote &q, price p)
{
    q.type = p;
}

double quote_helper_operator_double(const quote &q)
{
    return double(q);
}


//float quote_helper_operator_float(const quote &q)
//{
//    return float(q);
//}


boost::shared_ptr<quote> construct_quote_from_price(price p)
{
    return boost::make_shared<quote>(p);
}


boost::shared_ptr<quote> construct_quote_from_exchange_rate(const exchange_rate &e)
{
    return boost::make_shared<quote>(e);
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

////////////////////////////////////////////////////////////////////////////////
// esl.economics.markets.walras
////////////////////////////////////////////////////////////////////////////////
//#include <esl/economics/markets/walras/python_module_walras.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
#include <esl/economics/markets/walras/tatonnement.hpp>
using namespace esl::economics::markets::walras;


class python_differentiable_order_message
    : public esl::economics::markets::walras::differentiable_order_message
        , public wrapper<esl::economics::markets::walras::differentiable_order_message>
{
public:
    python_differentiable_order_message(
        const esl::simulation::python_module::python_identity &sender       = esl::simulation::python_module::python_identity(),
        const esl::simulation::python_module::python_identity& recipient     = esl::simulation::python_module::python_identity(),
        esl::simulation::time_point sent         = esl::simulation::time_point(),
        esl::simulation::time_point received     = esl::simulation::time_point())
        : esl::economics::markets::walras::differentiable_order_message
              ( esl::reinterpret_identity_cast<esl::agent>(sender)
                  , esl::reinterpret_identity_cast<esl::agent>(recipient)
                  , sent
                  , received)
        , wrapper<esl::economics::markets::walras::differentiable_order_message>()
    {

    }

    // the default implementation is to demand zero for all
    [[nodiscard]] std::map<esl::identity<esl::law::property>, esl::variable> excess_demand(const std::map<
        esl::identity<esl::law::property>
        , std::tuple<esl::economics::markets::quote, esl::variable>> &quotes) const override
    {
        dict quotes_;

        for(const auto &[i, v]: quotes){
            auto t = make_tuple(std::get<0>(v), esl::value(std::get<1>(v)));
            quotes_.setdefault(i, t);
        }

        // specify the type, so that the return value is converted to python::object
        object return_value_ = get_override("excess_demand")(quotes_);
        dict excess_ = extract<dict>(return_value_);
        auto keys = list(excess_.keys());
        auto values = list(excess_.values());

        std::map<esl::identity<esl::law::property>, esl::variable> result_;

        for(int i = 0; i < len(keys); ++i) {
            extract<esl::identity<esl::law::property>> extractor(keys[i]);
            extract<double> value_extractor(values[i]);
            if(extractor.check() && value_extractor.check()) {
                auto key   = extractor();
                auto value = value_extractor();

                result_.emplace(key, esl::variable(value));
            }
        }

        return result_;
    }
};


///
/// \brief  Excess demand model wrapper
///
class python_excess_demand_model
: public esl::economics::markets::tatonnement ::excess_demand_model
, public wrapper<esl::economics::markets::tatonnement ::excess_demand_model>
{
public:
    explicit python_excess_demand_model(esl::law::property_map<quote> initial_quotes)
        : excess_demand_model(std::move(initial_quotes))
        , wrapper<esl::economics::markets::tatonnement ::excess_demand_model>()
    {

    }
};

///
/// \brief  Helper funciton
///
/// \param e
/// \return
dict clear_market(python_excess_demand_model *e)
{
    auto quotes_ = e->compute_clearing_quotes();
    dict result_;

    if(quotes_.has_value()){
        //LOG(trace) << "clear_market has_value" << std::endl;
        for(const auto &[k,v]: quotes_.value()){
            //LOG(trace) << k <<" = " << v << std::endl;
            result_.setdefault(k, v);
        }
    }

    return result_;
}


///
/// \brief  Constructor wrapper that deals with the Python-supplied mapping
///
/// \param  Python dict with property base pointers as keys, and quote s as value
/// \return
boost::shared_ptr<python_excess_demand_model> excess_demand_model_python_constructor(const dict &d)
{
    esl::law::property_map<quote> pm;

    list keys = list(d.keys());
    list values = list(d.values());

    for (int i = 0; i < len(keys); ++i) {
        extract<boost::shared_ptr<esl::law::property>> extractor(keys[i]);
        extract<quote> value_extractor(values[i]);
        if (extractor.check() && value_extractor.check()){
            auto key = extractor();
            auto value = value_extractor();

            pm.try_emplace(to_std(key), value);
        }
    }

    auto model_ = boost::make_shared<python_excess_demand_model>(pm);
    model_->methods =
        { python_excess_demand_model::derivative_free_root
        , python_excess_demand_model::derivative_free_minimization};

    return model_;
}

/*
list get_differentiable_order_message(const excess_demand_model* e)
{

    boost::shared_ptr<walras::differentiable_order_message>
    e->excess_demand_functions_
}*/


typedef std::vector<boost::shared_ptr<walras::differentiable_order_message>> messages_t;

///
/// \brief  converts the list of demand messages to Python
///
/// \param e
/// \return
messages_t get_excess_demand_functions(const tatonnement::excess_demand_model &e)
{
    std::vector<boost::shared_ptr<walras::differentiable_order_message>> result_;
    for(auto m: e.excess_demand_functions_){
        result_.push_back(to_boost(m));
    }
    return result_;
}

///
/// \brief  accepts Python list back to
///
/// \param e
/// \param l
void set_excess_demand_functions(tatonnement::excess_demand_model &e, const list& l)
{
    std::vector<boost::shared_ptr<walras::differentiable_order_message>> result_;
    e.excess_demand_functions_.clear();
    for(int i = 0; i < len(l); ++i){
        extract<boost::shared_ptr<python_differentiable_order_message>> extractor(l[i]);
        e.excess_demand_functions_.push_back(to_std(extractor()));
    }
}

////////////////////////////////////////////////////////////////////////////////
// esl.geography
////////////////////////////////////////////////////////////////////////////////
///
/// \brief
/// \param c
/// \return
std::string python_country_code(const esl::geography::iso_3166_1_alpha_2 &c)
{
    return (std::string() + c.code[0]) + c.code[1];
}

////////////////////////////////////////////////////////////////////////////////
// esl.interaction
////////////////////////////////////////////////////////////////////////////////
//? #include <boost/serialization/export.hpp>
//? BOOST_CLASS_EXPORT(esl::interaction::python_message);

# include <boost/python/suite/indexing/indexing_suite.hpp>
# include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <esl/interaction/python_module_interaction.hpp>
#include <esl/interaction/transfer.hpp>
using namespace esl::interaction;




///
/// \brief  Takes a python function object and wraps it, so we can call it
///         from C++
///
/// \details    See communicator.hpp for callback_handle type definition
///
///
/// \param f The python function that handles the message
/// \return
communicator::callback_handle make_callback_handle(boost::python::object f)
{
    //  std::function<simulation::time_point( message_t
    //                                      , simulation::time_interval
    //                                      , std::seed_seq &
    //                                      )>;

    return [&]( communicator::message_t m
        , simulation::time_interval i
        , std::seed_seq &s
    )
    {
        auto result_ = f(m, i, s);
        return boost::python::extract<simulation::time_point>(result_);
    };
}

///
/// \brief  Since the main class is a template, we must expose a
///         non-template to python.
///
class python_message
    : public message<python_message, library_message_code<0x1u>()>
{
public:
    // this helps the linker resolve the message code
    constexpr const static message_code python_code = code;

    template<class archive_t>
    void serialize(archive_t &archive, const unsigned int version)
    {
        (void)version;
        //BOOST_SERIALIZATION_BASE_OBJECT_NVP(message);
        archive &boost::serialization::make_nvp("message",
                                                boost::serialization::base_object<message<python_message, library_message_code<0x1u>()>>(
                                                    *this));

    }
};








template <class container_t_, bool NoProxy, class DerivedPolicies>
class multimap_indexing_suite;

namespace detail
{
    template <class container_t_, bool NoProxy>
    class final_map_derived_policies
        : public multimap_indexing_suite<container_t_, NoProxy, final_map_derived_policies<container_t_, NoProxy> > {};
}


template < typename multimap_t_
    , bool NoProxy = false
    , typename DerivedPolicies = ::detail::final_map_derived_policies<multimap_t_, NoProxy> >
class multimap_indexing_suite
    : public indexing_suite<multimap_t_, DerivedPolicies
        , NoProxy
        , true
        , typename multimap_t_::value_type::second_type
        , typename multimap_t_::key_type
        , typename multimap_t_::key_type
    >
{
public:

    typedef typename multimap_t_::value_type value_type;
    typedef typename multimap_t_::value_type::second_type data_type;
    typedef typename multimap_t_::key_type key_type;
    typedef typename multimap_t_::key_type index_type;
    typedef typename multimap_t_::size_type size_type;
    typedef typename multimap_t_::difference_type difference_type;

    template <class Class> static void extension_def(Class& cl)
    {
        //  Wrap the map's element (value_type)
        std::string name_ = "multimap_indexing_suite_";
        object class_name(cl.attr("__name__"));
        extract<std::string> class_name_extractor(class_name);
        name_ += class_name_extractor();
        name_ += "_entry";

        typedef typename boost::mpl::if_<
            boost::mpl::and_<boost::is_class<data_type>, boost::mpl::bool_<!NoProxy> >
            , return_internal_reference<>
            , default_call_policies
        >::type get_data_return_policy;

        class_<value_type>(name_.c_str())
            .def("__repr__", &DerivedPolicies::representation)
            .def("data", &DerivedPolicies::get_data, get_data_return_policy())
            .def("key", &DerivedPolicies::get_key)
            ;
    }

    static object representation(typename multimap_t_::value_type const &e)
    {
        return "(%s, %s)" % boost::python::make_tuple(e.first, e.second);
    }

    static
    typename boost::mpl::if_<
        boost::mpl::and_<boost::is_class<data_type>, boost::mpl::bool_<!NoProxy> >
        , data_type &
        , data_type
    >::type
    get_data(typename multimap_t_::value_type &e)
    {
        return e.second;
    }

    static typename multimap_t_::key_type
    get_key(typename multimap_t_::value_type &e)
    {
        return e.first;
    }

    static data_type &get_item(multimap_t_ &container, index_type i_)
    {
        typename multimap_t_::iterator i = container.find(i_);
        if (i == container.end()){
            PyErr_SetString(PyExc_KeyError, "Invalid key");
            throw_error_already_set();
        }
        return i->second;
    }

    static void set_item(multimap_t_ &container, index_type i, data_type const& v)
    {
        for(auto &[k,value_]: container){
            if(k == i){
                value_ = v;
            }
        }
    }

    static void
    delete_item(multimap_t_ &container, index_type i)
    {
        container.erase(i);
    }

    static size_t
    size(multimap_t_ &container)
    {
        return container.size();
    }

    static bool
    contains(multimap_t_ &container, key_type const &key)
    {
        return container.find(key) != container.end();
    }

    static bool
    compare_index(multimap_t_ &container, index_type a, index_type b)
    {
        return container.key_comp()(a, b);
    }

    static index_type
    convert_index(multimap_t_ &container, PyObject *i_)
    {
        (void) container;
        extract<key_type const&> i(i_);
        if (i.check()){
            return i();
        }else{
            extract<key_type> i2(i_);
            if (i2.check()) {
                return i2();
            }
        }

        PyErr_SetString(PyExc_TypeError, "Invalid index type");
        throw_error_already_set();
        return index_type();
    }
};

///
/// \rbrief
///
/// \param c
/// \param o
void send_message_python(communicator &c, boost::python::object o)
{
    (void)c;
    (void)o;
    extract<const communicator::message_t &> extractor_(o);

}

////////////////////////////////////////////////////////////////////////////////
// esl.law
////////////////////////////////////////////////////////////////////////////////
#include <esl/law/legal_person.hpp>
#include <esl/law/natural_person.hpp>
#include <esl/law/property.hpp>
#include <esl/law/organization.hpp>
#include <esl/law/government.hpp>
#include <esl/law/jurisdictions.hpp>
#include <esl/law/legal_entity.hpp>
using namespace esl::law;

std::string python_legal_entity_local(esl::law::legal_entity &e)
{
    std::stringstream stream_;
    stream_.write(e.local.data(), e.local.size());
    return stream_.str();
}

std::string python_legal_entity_code(esl::law::legal_entity &e)
{
    std::stringstream stream_;
    stream_.write(e.code.data(), e.code.size());
    return stream_.str();
}

std::string python_legal_entity_checksum(esl::law::legal_entity &e)
{
    std::stringstream stream_;
    auto t = e.checksum();
    stream_<< std::get<0>(t) << std::get<1>(t);
    return stream_.str();
}


typedef esl::identity<esl::law::property> property_identity;

boost::shared_ptr<property_identity> convert_digit_list2(const boost::python::list &list)
{
    std::vector<uint64_t> result_;
    for(boost::python::ssize_t i = 0; i < boost::python::len(list); ++i) {
        result_.push_back(boost::python::extract<std::uint64_t>(list[i]));
    }
    return boost::make_shared<property_identity>(result_);
}

///
/// \brief  This is needed because we need to deal with the default argument
/// \param p
/// \return
std::string python_represent_property_identity(const esl::identity<esl::law::property> &p)
{
    return p.representation();
}


size_t python_property_identity_hash(const esl::identity<esl::law::property> &p)
{
    return std::hash<esl::identity<esl::law::property>>()(p);
}











////////////////////////////////////////////////////////////////////////////////
// esl.simulation
////////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<esl::simulation::python_module::python_identity>
convert_digit_list(const boost::python::list &list)
{
    std::vector<uint64_t> result_;
    for(boost::python::ssize_t i = 0; i < boost::python::len(list); ++i) {
        result_.push_back(boost::python::extract<std::uint64_t>(list[i]));
    }
    return boost::make_shared<esl::simulation::python_module::python_identity>(result_);
}



BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( python_identity_representation_overload
                                      , esl::simulation::python_module::python_identity::representation
                                      , 0
                                      , 1);

size_t python_identity_hash(const esl::simulation::python_module::python_identity &p)
{
    return std::hash<esl::simulation::python_module::python_identity>()(p);
}





////////////////////////////////////////////////////////////////////////////////
// esl.simulation.parameter
////////////////////////////////////////////////////////////////////////////////
#include <esl/simulation/parameter/parametrization.hpp>

using namespace esl::simulation::parameter;

template<typename parameter_t_>
std::optional<boost::python::object> pack(std::shared_ptr<parameter_base> parameter)
{
    auto value_ =
        std::dynamic_pointer_cast<constant<parameter_t_>>(parameter);

    if(value_) {
        return boost::python::object(parameter_t_(*value_));
    }
    return {};
}

boost::python::object get_helper(parametrization &p, const std::string &name)
{
    auto parameter_ = p.values.find(name);

    if(p.values.end() == parameter_) {
        throw esl::exception("parametrization[" + name + "]");
    }

    auto result_ = pack<double>(parameter_->second);
    if(result_){
        return result_.value();
    }

    result_ = pack<std::int64_t>(parameter_->second);
    if(result_){
        return result_.value();
    }

    result_ = pack<std::uint64_t>(parameter_->second);
    if(result_){
        return result_.value();
    }

    throw esl::exception("parametrization[" + name + "]");
}













////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
////////////////////////////////////////////////////////////////////////////////


namespace esl {


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
        .def("message", &esl::exception::what);

    register_exception_translator<esl::exception>(translate_exception);

    class_<quantity>("quantity", init<>())
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
        .def(self / std::uint64_t());

    //        class_<agent>("agent")
    //            .def("__init__", boost::python::make_constructor(&python_construct_agent))
    //            ;

    def("version", version, "Print the library's version.");

    ////////////////////////////////////////////////////////////////////////////
    // esl.computation
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_computation_ = create_scope("_computation");

        class_<computation::block_pool::block<object>>(
            "block", "The block is a unit of memory used by the block_pool allocator, and has fixed size.")
            .def_readwrite("data",
                           &computation::block_pool::block<object>::data)
            .def_readwrite("index",
                           &computation::block_pool::block<object>::index);

        // computational environment base class with default single thread
        class_<computation::python_environment>(
            "environment", "The environment class runs models: it schedules agents and delivers messages sent between agents.")
            .def("step", &computation::python_environment::step)
            .def("run", &computation::python_environment::run)
            .def("activate", &computation::python_environment::activate)
            .def("deactivate", &computation::python_environment::deactivate)
            .def("before_step", &computation::python_environment::before_step)
            .def("after_step", &computation::python_environment::after_step)
            .def("after_run", &computation::python_environment::after_run)
            .def("activate_agent",
                 &computation::python_environment::activate_agent)
            .def("deactivate_agent",
                 &computation::python_environment::deactivate_agent)
            .def("send_messages",
                 &computation::python_environment::send_messages);

        // timing information
        class_<computation::agent_timing>(
            "agent_timing",
            "Contains performance metrics for individual agents.")
            .def_readwrite("messaging", &agent_timing::messaging)
            .def_readwrite("acting", &agent_timing::acting);


        {
            boost::python::scope scope_distributed_ =
                create_scope("_distributed");
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // esl.data
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_data_ = create_scope("_data");


        ////////////////////////////////////////////////////////////////////////
        // esl.format
        ////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_format_ = create_scope("_format");
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // esl.economics
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_economics_ = create_scope("_economics");

        class_<iso_4217>("iso_4217")
            .add_property("code", python_currency_code)
            .add_property("denominator", &iso_4217::denominator)
            .add_property("__repr__", python_currency_code)
            .add_property("__str__", python_currency_code);

        ////////////////////////////////////////////////////////////////////////////
        // esl.economics.currencies
        ////////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_currencies_ = create_scope("_currencies");
            scope().attr("AED")                    = currencies::AED;
            scope().attr("AFN")                    = currencies::AFN;
            scope().attr("ALL")                    = currencies::ALL;
            scope().attr("AMD")                    = currencies::AMD;
            scope().attr("ANG")                    = currencies::ANG;
            scope().attr("AOA")                    = currencies::AOA;
            scope().attr("ARS")                    = currencies::ARS;
            scope().attr("AUD")                    = currencies::AUD;
            scope().attr("AWG")                    = currencies::AWG;
            scope().attr("AZN")                    = currencies::AZN;
            scope().attr("BAM")                    = currencies::BAM;
            scope().attr("BBD")                    = currencies::BBD;
            scope().attr("BDT")                    = currencies::BDT;
            scope().attr("BGN")                    = currencies::BGN;
            scope().attr("BHD")                    = currencies::BHD;
            scope().attr("BIF")                    = currencies::BIF;
            scope().attr("BMD")                    = currencies::BMD;
            scope().attr("BND")                    = currencies::BND;
            scope().attr("BOB")                    = currencies::BOB;
            scope().attr("BOV")                    = currencies::BOV;
            scope().attr("BRL")                    = currencies::BRL;
            scope().attr("BSD")                    = currencies::BSD;
            scope().attr("BTN")                    = currencies::BTN;
            scope().attr("BWP")                    = currencies::BWP;
            scope().attr("BYN")                    = currencies::BYN;
            scope().attr("BZD")                    = currencies::BZD;
            scope().attr("CAD")                    = currencies::CAD;
            scope().attr("CDF")                    = currencies::CDF;
            scope().attr("CHE")                    = currencies::CHE;
            scope().attr("CHF")                    = currencies::CHF;
            scope().attr("CHW")                    = currencies::CHW;
            scope().attr("CLF")                    = currencies::CLF;
            scope().attr("CLP")                    = currencies::CLP;
            scope().attr("CNY")                    = currencies::CNY;
            scope().attr("COP")                    = currencies::COP;
            scope().attr("COU")                    = currencies::COU;
            scope().attr("CRC")                    = currencies::CRC;
            scope().attr("CUC")                    = currencies::CUC;
            scope().attr("CUP")                    = currencies::CUP;
            scope().attr("CVE")                    = currencies::CVE;
            scope().attr("CZK")                    = currencies::CZK;
            scope().attr("DJF")                    = currencies::DJF;
            scope().attr("DKK")                    = currencies::DKK;
            scope().attr("DOP")                    = currencies::DOP;
            scope().attr("DZD")                    = currencies::DZD;
            scope().attr("EGP")                    = currencies::EGP;
            scope().attr("ERN")                    = currencies::ERN;
            scope().attr("ETB")                    = currencies::ETB;
            scope().attr("EUR")                    = currencies::EUR;
            scope().attr("FJD")                    = currencies::FJD;
            scope().attr("FKP")                    = currencies::FKP;
            scope().attr("GBP")                    = currencies::GBP;
            scope().attr("GEL")                    = currencies::GEL;
            scope().attr("GHS")                    = currencies::GHS;
            scope().attr("GIP")                    = currencies::GIP;
            scope().attr("GMD")                    = currencies::GMD;
            scope().attr("GNF")                    = currencies::GNF;
            scope().attr("GTQ")                    = currencies::GTQ;
            scope().attr("GYD")                    = currencies::GYD;
            scope().attr("HKD")                    = currencies::HKD;
            scope().attr("HNL")                    = currencies::HNL;
            scope().attr("HRK")                    = currencies::HRK;
            scope().attr("HTG")                    = currencies::HTG;
            scope().attr("HUF")                    = currencies::HUF;
            scope().attr("IDR")                    = currencies::IDR;
            scope().attr("ILS")                    = currencies::ILS;
            scope().attr("INR")                    = currencies::INR;
            scope().attr("IQD")                    = currencies::IQD;
            scope().attr("IRR")                    = currencies::IRR;
            scope().attr("ISK")                    = currencies::ISK;
            scope().attr("JMD")                    = currencies::JMD;
            scope().attr("JOD")                    = currencies::JOD;
            scope().attr("JPY")                    = currencies::JPY;
            scope().attr("KES")                    = currencies::KES;
            scope().attr("KGS")                    = currencies::KGS;
            scope().attr("KHR")                    = currencies::KHR;
            scope().attr("KMF")                    = currencies::KMF;
            scope().attr("KPW")                    = currencies::KPW;
            scope().attr("KRW")                    = currencies::KRW;
            scope().attr("KWD")                    = currencies::KWD;
            scope().attr("KYD")                    = currencies::KYD;
            scope().attr("KZT")                    = currencies::KZT;
            scope().attr("LAK")                    = currencies::LAK;
            scope().attr("LBP")                    = currencies::LBP;
            scope().attr("LKR")                    = currencies::LKR;
            scope().attr("LRD")                    = currencies::LRD;
            scope().attr("LSL")                    = currencies::LSL;
            scope().attr("LYD")                    = currencies::LYD;
            scope().attr("MAD")                    = currencies::MAD;
            scope().attr("MDL")                    = currencies::MDL;
            scope().attr("MGA")                    = currencies::MGA;
            scope().attr("MKD")                    = currencies::MKD;
            scope().attr("MMK")                    = currencies::MMK;
            scope().attr("MNT")                    = currencies::MNT;
            scope().attr("MOP")                    = currencies::MOP;
            scope().attr("MRU")                    = currencies::MRU;
            scope().attr("MUR")                    = currencies::MUR;
            scope().attr("MVR")                    = currencies::MVR;
            scope().attr("MWK")                    = currencies::MWK;
            scope().attr("MXN")                    = currencies::MXN;
            scope().attr("MXV")                    = currencies::MXV;
            scope().attr("MYR")                    = currencies::MYR;
            scope().attr("MZN")                    = currencies::MZN;
            scope().attr("NAD")                    = currencies::NAD;
            scope().attr("NGN")                    = currencies::NGN;
            scope().attr("NIO")                    = currencies::NIO;
            scope().attr("NOK")                    = currencies::NOK;
            scope().attr("NPR")                    = currencies::NPR;
            scope().attr("NZD")                    = currencies::NZD;
            scope().attr("OMR")                    = currencies::OMR;
            scope().attr("PAB")                    = currencies::PAB;
            scope().attr("PEN")                    = currencies::PEN;
            scope().attr("PGK")                    = currencies::PGK;
            scope().attr("PHP")                    = currencies::PHP;
            scope().attr("PKR")                    = currencies::PKR;
            scope().attr("PLN")                    = currencies::PLN;
            scope().attr("PYG")                    = currencies::PYG;
            scope().attr("QAR")                    = currencies::QAR;
            scope().attr("RON")                    = currencies::RON;
            scope().attr("RSD")                    = currencies::RSD;
            scope().attr("RUB")                    = currencies::RUB;
            scope().attr("RWF")                    = currencies::RWF;
            scope().attr("SAR")                    = currencies::SAR;
            scope().attr("SBD")                    = currencies::SBD;
            scope().attr("SCR")                    = currencies::SCR;
            scope().attr("SDG")                    = currencies::SDG;
            scope().attr("SEK")                    = currencies::SEK;
            scope().attr("SGD")                    = currencies::SGD;
            scope().attr("SHP")                    = currencies::SHP;
            scope().attr("SLL")                    = currencies::SLL;
            scope().attr("SOS")                    = currencies::SOS;
            scope().attr("SRD")                    = currencies::SRD;
            scope().attr("SSP")                    = currencies::SSP;
            scope().attr("STN")                    = currencies::STN;
            scope().attr("SVC")                    = currencies::SVC;
            scope().attr("SYP")                    = currencies::SYP;
            scope().attr("SZL")                    = currencies::SZL;
            scope().attr("THB")                    = currencies::THB;
            scope().attr("TJS")                    = currencies::TJS;
            scope().attr("TMT")                    = currencies::TMT;
            scope().attr("TND")                    = currencies::TND;
            scope().attr("TOP")                    = currencies::TOP;
            scope().attr("TRY")                    = currencies::TRY;
            scope().attr("TTD")                    = currencies::TTD;
            scope().attr("TWD")                    = currencies::TWD;
            scope().attr("TZS")                    = currencies::TZS;
            scope().attr("UAH")                    = currencies::UAH;
            scope().attr("UGX")                    = currencies::UGX;
            scope().attr("USD")                    = currencies::USD;
            scope().attr("USN")                    = currencies::USN;
            scope().attr("UYI")                    = currencies::UYI;
            scope().attr("UYU")                    = currencies::UYU;
            scope().attr("UZS")                    = currencies::UZS;
            scope().attr("VEF")                    = currencies::VEF;
            scope().attr("VND")                    = currencies::VND;
            scope().attr("VUV")                    = currencies::VUV;
            scope().attr("WST")                    = currencies::WST;
            scope().attr("XAF")                    = currencies::XAF;
            scope().attr("XAG")                    = currencies::XAG;
            scope().attr("XAU")                    = currencies::XAU;
            scope().attr("XBA")                    = currencies::XBA;
            scope().attr("XBB")                    = currencies::XBB;
            scope().attr("XBC")                    = currencies::XBC;
            scope().attr("XBD")                    = currencies::XBD;
            scope().attr("XCD")                    = currencies::XCD;
            scope().attr("XDR")                    = currencies::XDR;
            scope().attr("XOF")                    = currencies::XOF;
            scope().attr("XPD")                    = currencies::XPD;
            scope().attr("XPF")                    = currencies::XPF;
            scope().attr("XPT")                    = currencies::XPT;
            scope().attr("XSU")                    = currencies::XSU;
            scope().attr("XUA") = currencies::XUA;
            // scope().attr("XXX") = currencies::XXX;
            scope().attr("YER") = currencies::YER;
            scope().attr("ZAR") = currencies::ZAR;
            scope().attr("ZMW") = currencies::ZMW;
            scope().attr("ZWL") = currencies::ZWL;
        }

        class_<price>("price", init<std::int64_t, iso_4217>())
            .add_property("value", &price::value)
            .add_property("valuation", &price::valuation)
            .def("approximate", &price::approximate)
            .def(self == self)
            .def(self != self)
            .def(self < self)
            .def(self <= self)
            .def(self > self)
            .def(self >= self)
            .def("__repr__", &price::representation)
            .def("__str__", &price::representation)
            .def("__float__", &python_price_to_floating_point);


        ////////////////////////////////////////////////////////////////////////
        // esl.economics.accounting
        ////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_accounting_ = create_scope("_accounting");
        }

        ////////////////////////////////////////////////////////////////////////
        // esl.economics.finance
        ////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_finance = create_scope("_finance");

            class_<finance::isin>(
                "isin", init<geography::iso_3166_1_alpha_2, std::string>())
                .add_property("issuer", &finance::isin::issuer)
                .add_property("code", &get_isin_code, &set_isin_code)
                .def("__repr__", &finance::isin::representation)
                .def("__str__", &finance::isin::representation);

            class_<finance::share_class>(
                "share_class",
                init<std::uint8_t, std::uint8_t, float, bool, bool, bool>())
                .add_property("rank", &finance::share_class::rank)
                .add_property("votes", &finance::share_class::votes)
                .add_property("preference", &finance::share_class::preference)
                .add_property("dividend", &finance::share_class::dividend)
                .add_property("cumulative", &finance::share_class::cumulative)
                .add_property("redeemable", &finance::share_class::redeemable)
                .def(self == self)
                .def(self < self);
        }

        ////////////////////////////////////////////////////////////////////////
        // esl.economics.markets
        ////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_markets_ = create_scope("_markets");

            class_<iso_10383>("iso_10383", init<std::string>())
                .def("__repr__", &iso_10383::representation)
                .def(self_ns::str(self_ns::self))
                .def(self == self)
                .def(self != self)
                .def(self < self)
                .def(self > self)
                .def(self <= self)
                .def(self >= self);

            class_<quote, boost::noncopyable, boost::shared_ptr<quote>>("quote",
                                                                        no_init)
                .def("__init__", boost::python::make_constructor(
                                     construct_quote_from_price))
                .def("__init__", boost::python::make_constructor(
                                     construct_quote_from_exchange_rate))
                .add_property("price", &quote_helper_get_price,
                              &quote_helper_set_price)
                .def_readwrite("lot", &quote::lot)

                .def(self == self)
                .def(self != self)
                .def(self < self)
                .def(self > self)
                .def(self <= self)
                .def(self >= self)

                .def("__float__", &quote_helper_operator_double)

                .def("__repr__", &quote::representation)
                .def("__str__", &quote::representation);

            implicitly_convertible<quote, double>();

            class_<ticker>(
                "ticker",
                init<identity<law::property>, identity<law::property>>())
                .def_readwrite("base", &ticker::base)
                .def_readwrite("quote", &ticker::quote)
                .def(self == self)
                .def(self != self)
                .def(self < self)
                .def(self > self)
                .def(self <= self)
                .def(self >= self);

            enum_<indication>("indication")
                .value("firm", indication::firm)
                .value("indicative", indication::indicative);


            ////////////////////////////////////////////////////////////////////
            // esl.economics.markets.impact_function
            ////////////////////////////////////////////////////////////////////
            {
                boost::python::scope scope_impact_function_ =
                    create_scope("_impact_function");
            }


            ////////////////////////////////////////////////////////////////////
            // esl.economics.markets.order_book
            ////////////////////////////////////////////////////////////////////
            {
                boost::python::scope scope_order_book_ =
                    create_scope("_order_book");

                enum_<execution_report::state_t>("execution_state")
                    .value("invalid", execution_report::state_t::invalid)
                    .value("cancel", execution_report::state_t::cancel)
                    .value("match", execution_report::state_t::match)
                    .value("placement", execution_report::state_t::placement);

                class_<execution_report>("execution_report")
                    .def_readwrite("quantity", &execution_report::quantity)
                    .def_readwrite("identifier", &execution_report::identifier)
                    .def_readwrite("side", &execution_report::side)
                    .def_readwrite("limit", &execution_report::limit)
                    .def_readwrite("owner", &execution_report::owner)
                    .def("__repr__", &execution_report::representation)
                    .def("__str__", &execution_report::representation);

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
                    .def("display", &basic_book::display);

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
                    .def("display", &basic_book::display);

                class_<static_order_book, bases<basic_book>>(
                    "static_order_book",
                    init<markets::quote, markets::quote, size_t>())
                    .def_readwrite("reports", &basic_book::reports)
                    .def("ask", &basic_book::ask)
                    .def("bid", &basic_book::bid)
                    .def("insert", &basic_book::insert)
                    .def("cancel", &basic_book::cancel)
                    .def("display", &basic_book::display);

                class_<binary_tree_order_book, bases<basic_book>>(
                    "binary_tree_order_book", init<>())
                    .def_readwrite("reports", &basic_book::reports)
                    .def("ask", &basic_book::ask)
                    .def("bid", &basic_book::bid)
                    .def("insert", &basic_book::insert)
                    .def("cancel", &basic_book::cancel)
                    .def("display", &basic_book::display);

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
                boost::python::scope scope_walras_ = create_scope("_walras");

                enum_<tatonnement::excess_demand_model::solver>("solver")
                    // TODO: implement automatic differentiation for Python
                    //.value("root", excess_demand_model::root)
                    //.value("minimization", excess_demand_model::minimization)
                    .value(
                        "derivative_free_root",
                        tatonnement::excess_demand_model::derivative_free_root)
                    .value("derivative_free_minimization",
                           tatonnement::excess_demand_model::
                               derivative_free_minimization)
                    .export_values();

                class_<python_differentiable_order_message, boost::noncopyable>(
                    "differentiable_order_message",
                    init<esl::simulation::python_module::python_identity,
                         esl::simulation::python_module::python_identity,
                         esl::simulation::time_point,
                         esl::simulation::time_point>())
                    .add_property(
                        "supply",
                        &walras::differentiable_order_message::supply);

                // expose vector of messages to Python
                class_<messages_t>("messages_t")
                    .def(vector_indexing_suite<messages_t>());

                class_<python_excess_demand_model, boost::noncopyable>(
                    "excess_demand_model", no_init)  // non-trivial constructor
                    .def("__init__",
                         make_constructor(
                             &excess_demand_model_python_constructor))
                    .def_readwrite(
                        "circuit_breaker",
                        &tatonnement::excess_demand_model::circuit_breaker)
                    .def_readwrite("methods",
                                   &tatonnement::excess_demand_model::methods)
                    .def_readwrite("quotes",
                                   &tatonnement::excess_demand_model::quotes)
                    .def("compute_clearing_quotes", &clear_market)
                    .add_property("excess_demand_functions",
                                  &get_excess_demand_functions,
                                  &set_excess_demand_functions);
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // esl.geography
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_geography_ = create_scope("_geography");

        class_<esl::geography::iso_3166_1_alpha_2>("iso_3166_1_alpha_2")
            .add_property("code", python_country_code)
            .def("__repr__",
                 &esl::geography::iso_3166_1_alpha_2::representation)
            .def("__str__",
                 &esl::geography::iso_3166_1_alpha_2::representation);

        {
            boost::python::scope scope_countries_ = create_scope("_countries");

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
    }


    ////////////////////////////////////////////////////////////////////////////
    // esl.interaction
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_interaction_ = create_scope("_interaction");

        def("make_callback_handle", &make_callback_handle);

        class_<communicator::callback_handle>("callback_handle");

        class_<communicator::callback_t>("callback_t")
            .def_readwrite("function", &communicator::callback_t::function)
            .def_readwrite("description",
                           &communicator::callback_t::description)
            .def_readwrite("message", &communicator::callback_t::message)
            .def_readwrite("file", &communicator::callback_t::file)
            .def_readwrite("line", &communicator::callback_t::line)
            //.def("__call__", &communicator::callback_t::operator ())
            ;

        enum_<communicator::scheduling>("scheduling")
            .value("in_order", communicator::scheduling::in_order)
            .value("random", communicator::scheduling::random);

        class_<communicator::inbox_t>("inbox_t").def(
            multimap_indexing_suite<communicator::inbox_t>());


        class_<communicator::outbox_t>("outbox_t")
            .def(
                boost::python::vector_indexing_suite<communicator::outbox_t>());

        class_<communicator>("communicator")
            .def("send_message", send_message_python)
            .def_readwrite("inbox", &communicator::inbox)
            .def_readwrite("outbox", &communicator::outbox);

        class_<header>("header",
                       init<message_code, identity<agent>, identity<agent>,
                            simulation::time_point, simulation::time_point>())
            .def(init<message_code, identity<agent>, identity<agent>,
                      simulation::time_point>())
            .def(init<message_code, identity<agent>, identity<agent>>())
            .def(init<message_code, identity<agent>>())
            .def(init<message_code>())
            .def(init<message_code>())
            .def_readwrite("type", &header::type)
            .def_readwrite("sender", &header::sender)
            .def_readwrite("recipient", &header::recipient)
            .def_readwrite("sent", &header::sent)
            .def_readwrite("received", &header::received);

        class_<python_message, bases<header>>("message").def_readonly(
            "code", &python_message::python_code);
    }

    ////////////////////////////////////////////////////////////////////////////
    // esl.law
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_law_ = create_scope("_law");

        class_<esl::law::legal_entity>("legal_entity", init<std::string>())
            .add_property("local", &python_legal_entity_local)
            .add_property("code", &python_legal_entity_code)
            .def("checksum", &python_legal_entity_checksum)
            ;


        class_<esl::law::jurisdiction>("jurisdiction", init<geography::iso_3166_1_alpha_2, economics::iso_4217>())
            .add_property("sovereign", &esl::law::jurisdiction::sovereign)
            .add_property("tender", &esl::law::jurisdiction::tender)
            ;


        class_<identity<property>>( "property_identity")
            .def("__init__", make_constructor(convert_digit_list2))

            .def("__str__", &python_represent_property_identity)
            .def("__repr__", &python_represent_property_identity)

            .def(self == self)
            .def(self != self)
            .def(self < self)
            .def(self <= self)
            .def(self > self)
            .def(self >= self)

            .def("__hash__", &python_property_identity_hash)
            ;

        class_< property
            //, bases<entity<property>>
        >( "property", init<identity<property>>())
            .def("name", &property::name)
            .add_property("identifier", &property::identifier)
            ;

        class_<natural_person>
            ( "natural_person", init<esl::geography::iso_3166_1_alpha_2>())
            .def_readonly("nationality", &natural_person::nationality)
            ;



        {
            boost::python::scope scope_jurisdictions_ = create_scope("_jurisdictions");

            scope().attr("AF") = esl::law::jurisdictions::AF;
            scope().attr("AL") = esl::law::jurisdictions::AL;
            scope().attr("DZ") = esl::law::jurisdictions::DZ;
            scope().attr("AS") = esl::law::jurisdictions::AS;
            //    scope().attr("AD") = esl::law::jurisdictions::AD;
            scope().attr("AO") = esl::law::jurisdictions::AO;
            scope().attr("AI") = esl::law::jurisdictions::AI;
            scope().attr("AG") = esl::law::jurisdictions::AG;
            scope().attr("AR") = esl::law::jurisdictions::AR;
            scope().attr("AM") = esl::law::jurisdictions::AM;
            scope().attr("AW") = esl::law::jurisdictions::AW;
            scope().attr("AU") = esl::law::jurisdictions::AU;
            scope().attr("AT") = esl::law::jurisdictions::AT;
            scope().attr("AZ") = esl::law::jurisdictions::AZ;
            scope().attr("BS") = esl::law::jurisdictions::BS;
            scope().attr("BH") = esl::law::jurisdictions::BH;
            scope().attr("BD") = esl::law::jurisdictions::BD;
            scope().attr("BB") = esl::law::jurisdictions::BB;
            scope().attr("BY") = esl::law::jurisdictions::BY;
            scope().attr("BE") = esl::law::jurisdictions::BE;
            scope().attr("BZ") = esl::law::jurisdictions::BZ;
            scope().attr("BJ") = esl::law::jurisdictions::BJ;
            scope().attr("BM") = esl::law::jurisdictions::BM;
            scope().attr("BT") = esl::law::jurisdictions::BT;
            scope().attr("BO") = esl::law::jurisdictions::BO;
            scope().attr("BQ") = esl::law::jurisdictions::BQ;
            scope().attr("BA") = esl::law::jurisdictions::BA;
            scope().attr("BW") = esl::law::jurisdictions::BW;
            scope().attr("BV") = esl::law::jurisdictions::BV;
            scope().attr("BR") = esl::law::jurisdictions::BR;
            scope().attr("IO") = esl::law::jurisdictions::IO;
            scope().attr("VG") = esl::law::jurisdictions::VG;
            scope().attr("BN") = esl::law::jurisdictions::BN;
            scope().attr("BG") = esl::law::jurisdictions::BG;
            scope().attr("BF") = esl::law::jurisdictions::BF;
            scope().attr("BI") = esl::law::jurisdictions::BI;
            scope().attr("CV") = esl::law::jurisdictions::CV;
            scope().attr("KH") = esl::law::jurisdictions::KH;
            scope().attr("CM") = esl::law::jurisdictions::CM;
            scope().attr("CA") = esl::law::jurisdictions::CA;
            scope().attr("KY") = esl::law::jurisdictions::KY;
            scope().attr("CF") = esl::law::jurisdictions::CF;
            scope().attr("TD") = esl::law::jurisdictions::TD;
            scope().attr("CL") = esl::law::jurisdictions::CL;
            scope().attr("CN") = esl::law::jurisdictions::CN;
            scope().attr("HK") = esl::law::jurisdictions::HK;
            scope().attr("MO") = esl::law::jurisdictions::MO;
            scope().attr("CX") = esl::law::jurisdictions::CX;
            scope().attr("CC") = esl::law::jurisdictions::CC;
            scope().attr("CO") = esl::law::jurisdictions::CO;
            scope().attr("KM") = esl::law::jurisdictions::KM;
            scope().attr("CG") = esl::law::jurisdictions::CG;
            scope().attr("CK") = esl::law::jurisdictions::CK;
            scope().attr("CR") = esl::law::jurisdictions::CR;
            scope().attr("HR") = esl::law::jurisdictions::HR;
            scope().attr("CU") = esl::law::jurisdictions::CU;
            scope().attr("CW") = esl::law::jurisdictions::CW;
            scope().attr("CY") = esl::law::jurisdictions::CY;
            scope().attr("CZ") = esl::law::jurisdictions::CZ;
            scope().attr("CI") = esl::law::jurisdictions::CI;
            scope().attr("KP") = esl::law::jurisdictions::KP;
            scope().attr("CD") = esl::law::jurisdictions::CD;
            scope().attr("DK") = esl::law::jurisdictions::DK;
            scope().attr("DJ") = esl::law::jurisdictions::DJ;
            scope().attr("DM") = esl::law::jurisdictions::DM;
            scope().attr("DO") = esl::law::jurisdictions::DO;
            scope().attr("EC") = esl::law::jurisdictions::EC;
            scope().attr("EG") = esl::law::jurisdictions::EG;
            scope().attr("SV") = esl::law::jurisdictions::SV;
            scope().attr("GQ") = esl::law::jurisdictions::GQ;
            scope().attr("ER") = esl::law::jurisdictions::ER;
            scope().attr("EE") = esl::law::jurisdictions::EE;
            scope().attr("ET") = esl::law::jurisdictions::ET;
            scope().attr("FO") = esl::law::jurisdictions::FO;
            scope().attr("FJ") = esl::law::jurisdictions::FJ;
            scope().attr("FI") = esl::law::jurisdictions::FI;
            scope().attr("FR") = esl::law::jurisdictions::FR;
            scope().attr("GF") = esl::law::jurisdictions::GF;
            scope().attr("PF") = esl::law::jurisdictions::PF;
            scope().attr("TF") = esl::law::jurisdictions::TF;
            scope().attr("GA") = esl::law::jurisdictions::GA;
            scope().attr("GM") = esl::law::jurisdictions::GM;
            scope().attr("GE") = esl::law::jurisdictions::GE;
            scope().attr("DE") = esl::law::jurisdictions::DE;
            scope().attr("GH") = esl::law::jurisdictions::GH;
            scope().attr("GI") = esl::law::jurisdictions::GI;
            scope().attr("GR") = esl::law::jurisdictions::GR;
            scope().attr("GL") = esl::law::jurisdictions::GL;
            scope().attr("GD") = esl::law::jurisdictions::GD;
            scope().attr("GP") = esl::law::jurisdictions::GP;
            scope().attr("GU") = esl::law::jurisdictions::GU;
            scope().attr("GT") = esl::law::jurisdictions::GT;
            scope().attr("GG") = esl::law::jurisdictions::GG;
            scope().attr("GN") = esl::law::jurisdictions::GN;
            scope().attr("GW") = esl::law::jurisdictions::GW;
            scope().attr("GY") = esl::law::jurisdictions::GY;
            scope().attr("HT") = esl::law::jurisdictions::HT;
            scope().attr("HM") = esl::law::jurisdictions::HM;
            scope().attr("VA") = esl::law::jurisdictions::VA;
            scope().attr("HN") = esl::law::jurisdictions::HN;
            scope().attr("HU") = esl::law::jurisdictions::HU;
            scope().attr("IS") = esl::law::jurisdictions::IS;
            scope().attr("IN") = esl::law::jurisdictions::IN;
            scope().attr("ID") = esl::law::jurisdictions::ID;
            scope().attr("IR") = esl::law::jurisdictions::IR;
            scope().attr("IQ") = esl::law::jurisdictions::IQ;
            scope().attr("IE") = esl::law::jurisdictions::IE;
            scope().attr("IM") = esl::law::jurisdictions::IM;
            scope().attr("IL") = esl::law::jurisdictions::IL;
            scope().attr("IT") = esl::law::jurisdictions::IT;
            scope().attr("JM") = esl::law::jurisdictions::JM;
            scope().attr("JP") = esl::law::jurisdictions::JP;
            scope().attr("JE") = esl::law::jurisdictions::JE;
            scope().attr("JO") = esl::law::jurisdictions::JO;
            scope().attr("KZ") = esl::law::jurisdictions::KZ;
            scope().attr("KE") = esl::law::jurisdictions::KE;
            scope().attr("KI") = esl::law::jurisdictions::KI;
            scope().attr("KW") = esl::law::jurisdictions::KW;
            scope().attr("KG") = esl::law::jurisdictions::KG;
            scope().attr("LA") = esl::law::jurisdictions::LA;
            scope().attr("LV") = esl::law::jurisdictions::LV;
            scope().attr("LB") = esl::law::jurisdictions::LB;
            scope().attr("LS") = esl::law::jurisdictions::LS;
            scope().attr("LR") = esl::law::jurisdictions::LR;
            scope().attr("LY") = esl::law::jurisdictions::LY;
            scope().attr("LI") = esl::law::jurisdictions::LI;
            scope().attr("LT") = esl::law::jurisdictions::LT;
            scope().attr("LU") = esl::law::jurisdictions::LU;
            scope().attr("MG") = esl::law::jurisdictions::MG;
            scope().attr("MW") = esl::law::jurisdictions::MW;
            scope().attr("MY") = esl::law::jurisdictions::MY;
            scope().attr("MV") = esl::law::jurisdictions::MV;
            scope().attr("ML") = esl::law::jurisdictions::ML;
            scope().attr("MT") = esl::law::jurisdictions::MT;
            scope().attr("MH") = esl::law::jurisdictions::MH;
            scope().attr("MQ") = esl::law::jurisdictions::MQ;
            //   scope().attr("MR") = esl::law::jurisdictions::MR;
            scope().attr("MU") = esl::law::jurisdictions::MU;
            scope().attr("YT") = esl::law::jurisdictions::YT;
            scope().attr("MX") = esl::law::jurisdictions::MX;
            scope().attr("FM") = esl::law::jurisdictions::FM;
            scope().attr("MC") = esl::law::jurisdictions::MC;
            scope().attr("MN") = esl::law::jurisdictions::MN;
            scope().attr("ME") = esl::law::jurisdictions::ME;
            scope().attr("MS") = esl::law::jurisdictions::MS;
            scope().attr("MA") = esl::law::jurisdictions::MA;
            scope().attr("MZ") = esl::law::jurisdictions::MZ;
            scope().attr("MM") = esl::law::jurisdictions::MM;
            scope().attr("NA") = esl::law::jurisdictions::NA;
            scope().attr("NR") = esl::law::jurisdictions::NR;
            scope().attr("NP") = esl::law::jurisdictions::NP;
            scope().attr("NL") = esl::law::jurisdictions::NL;
            scope().attr("NC") = esl::law::jurisdictions::NC;
            scope().attr("NZ") = esl::law::jurisdictions::NZ;
            scope().attr("NI") = esl::law::jurisdictions::NI;
            scope().attr("NE") = esl::law::jurisdictions::NE;
            scope().attr("NG") = esl::law::jurisdictions::NG;
            scope().attr("NU") = esl::law::jurisdictions::NU;
            scope().attr("NF") = esl::law::jurisdictions::NF;
            scope().attr("MP") = esl::law::jurisdictions::MP;
            scope().attr("NO") = esl::law::jurisdictions::NO;
            scope().attr("OM") = esl::law::jurisdictions::OM;
            scope().attr("PK") = esl::law::jurisdictions::PK;
            scope().attr("PW") = esl::law::jurisdictions::PW;
            scope().attr("PA") = esl::law::jurisdictions::PA;
            scope().attr("PG") = esl::law::jurisdictions::PG;
            scope().attr("PY") = esl::law::jurisdictions::PY;
            scope().attr("PE") = esl::law::jurisdictions::PE;
            scope().attr("PH") = esl::law::jurisdictions::PH;
            scope().attr("PN") = esl::law::jurisdictions::PN;
            scope().attr("PL") = esl::law::jurisdictions::PL;
            scope().attr("PT") = esl::law::jurisdictions::PT;
            scope().attr("PR") = esl::law::jurisdictions::PR;
            scope().attr("QA") = esl::law::jurisdictions::QA;
            scope().attr("KR") = esl::law::jurisdictions::KR;
            scope().attr("MD") = esl::law::jurisdictions::MD;
            scope().attr("RO") = esl::law::jurisdictions::RO;
            scope().attr("RU") = esl::law::jurisdictions::RU;
            scope().attr("RW") = esl::law::jurisdictions::RW;
            scope().attr("RE") = esl::law::jurisdictions::RE;
            scope().attr("BL") = esl::law::jurisdictions::BL;
            scope().attr("SH") = esl::law::jurisdictions::SH;
            scope().attr("KN") = esl::law::jurisdictions::KN;
            scope().attr("LC") = esl::law::jurisdictions::LC;
            scope().attr("MF") = esl::law::jurisdictions::MF;
            scope().attr("PM") = esl::law::jurisdictions::PM;
            scope().attr("VC") = esl::law::jurisdictions::VC;
            scope().attr("WS") = esl::law::jurisdictions::WS;
            scope().attr("SM") = esl::law::jurisdictions::SM;
            //    scope().attr("ST") = esl::law::jurisdictions::ST;
            scope().attr("SA") = esl::law::jurisdictions::SA;
            scope().attr("SN") = esl::law::jurisdictions::SN;
            scope().attr("RS") = esl::law::jurisdictions::RS;
            scope().attr("SC") = esl::law::jurisdictions::SC;
            scope().attr("SL") = esl::law::jurisdictions::SL;
            scope().attr("SG") = esl::law::jurisdictions::SG;
            scope().attr("SX") = esl::law::jurisdictions::SX;
            scope().attr("SK") = esl::law::jurisdictions::SK;
            scope().attr("SI") = esl::law::jurisdictions::SI;
            scope().attr("SB") = esl::law::jurisdictions::SB;
            scope().attr("SO") = esl::law::jurisdictions::SO;
            scope().attr("ZA") = esl::law::jurisdictions::ZA;
            scope().attr("SS") = esl::law::jurisdictions::SS;
            scope().attr("ES") = esl::law::jurisdictions::ES;
            scope().attr("LK") = esl::law::jurisdictions::LK;
            scope().attr("SD") = esl::law::jurisdictions::SD;
            scope().attr("SR") = esl::law::jurisdictions::SR;
            scope().attr("SJ") = esl::law::jurisdictions::SJ;
            scope().attr("SZ") = esl::law::jurisdictions::SZ;
            scope().attr("SE") = esl::law::jurisdictions::SE;
            scope().attr("CH") = esl::law::jurisdictions::CH;
            scope().attr("SY") = esl::law::jurisdictions::SY;
            scope().attr("TJ") = esl::law::jurisdictions::TJ;
            scope().attr("TH") = esl::law::jurisdictions::TH;
            scope().attr("MK") = esl::law::jurisdictions::MK;
            scope().attr("TL") = esl::law::jurisdictions::TL;
            scope().attr("TG") = esl::law::jurisdictions::TG;
            scope().attr("TK") = esl::law::jurisdictions::TK;
            scope().attr("TO") = esl::law::jurisdictions::TO;
            scope().attr("TT") = esl::law::jurisdictions::TT;
            scope().attr("TN") = esl::law::jurisdictions::TN;
            scope().attr("TR") = esl::law::jurisdictions::TR;
            scope().attr("TM") = esl::law::jurisdictions::TM;
            scope().attr("TC") = esl::law::jurisdictions::TC;
            scope().attr("TV") = esl::law::jurisdictions::TV;
            scope().attr("UG") = esl::law::jurisdictions::UG;
            scope().attr("UA") = esl::law::jurisdictions::UA;
            scope().attr("AE") = esl::law::jurisdictions::AE;
            scope().attr("GB") = esl::law::jurisdictions::GB;
            scope().attr("TZ") = esl::law::jurisdictions::TZ;
            scope().attr("UM") = esl::law::jurisdictions::UM;
            scope().attr("VI") = esl::law::jurisdictions::VI;
            scope().attr("US") = esl::law::jurisdictions::US;
            scope().attr("UY") = esl::law::jurisdictions::UY;
            scope().attr("UZ") = esl::law::jurisdictions::UZ;
            scope().attr("VU") = esl::law::jurisdictions::VU;
            scope().attr("VE") = esl::law::jurisdictions::VE;
            scope().attr("VN") = esl::law::jurisdictions::VN;
            scope().attr("WF") = esl::law::jurisdictions::WF;
            scope().attr("EH") = esl::law::jurisdictions::EH;
            scope().attr("YE") = esl::law::jurisdictions::YE;
            scope().attr("ZM") = esl::law::jurisdictions::ZM;
            scope().attr("ZW") = esl::law::jurisdictions::ZW;
            scope().attr("AX") = esl::law::jurisdictions::AX;
        }
    }



    ////////////////////////////////////////////////////////////////////////////
    // esl.mathematics
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_mathematics_ = create_scope("_mathematics");
//        class_<adept::Stack>("stack", init<>())
//            .def("pause_recording", &adept::Stack::pause_recording)
//            .def("continue_recording", &adept::Stack::continue_recording)
//            .def("new_recording", &adept::Stack::new_recording)
//            .def("compute_adjoint", &adept::Stack::compute_adjoint)
//            ;
    }

    ////////////////////////////////////////////////////////////////////////////
    // esl.simulation
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_simulation_ = create_scope("_simulation");

        class_<esl::entity<object>, boost::noncopyable>(
            "entity", init<identity<object>>())
            .def_readonly("identifier", &esl::entity<object>::identifier)
            .def(self_ns::str(self_ns::self))
            .def("create", &esl::entity<object>::create<object>)
            .def(self == self)
            .def(self != self)
            ;

        class_<esl::simulation::python_module::python_identity>("identity")
            .def("__init__", make_constructor(convert_digit_list))
            .def_readonly("digits", &esl::simulation::python_module::python_identity::digits)
            .def("__str__", &esl::simulation::python_module::python_identity::representation,
                 python_identity_representation_overload(args("width"), ""))
            .def("__repr__", &esl::simulation::python_module::python_identity::representation,
                 python_identity_representation_overload(args("width"), ""))
            .def(self < self)
            .def(self > self)
            .def(self == self)
            .def(self != self)
            .def(self <= self)
            .def(self >= self)
            .def("__hash__", &python_identity_hash)
            ;



        {
            boost::python::scope scope_parameter_ = create_scope("_parameter");

            class_<parameter_base>("parameter_base", init<>());

            class_<constant<double>>("constant_double", init<double>());
            class_<constant<std::int64_t>>("constant_int64", init<int64_t>());
            class_<constant<std::uint64_t>>("constant_uint64", init<uint64_t>());

            class_<parametrization>("parametrization", init<>())
                .def("get", get_helper)

                ;

        }

    }
}
}//namespace esl

#endif