/// \file   python_module.cpp
///
/// \brief  This contains the bindings
///
/// \details    To facilitate quick building, we create a single Python binary
///             module. We programmatically create scopes to create submodules
///             from a single file. This integrates with the Python code which
///             follows the regular folder structure/submodule layout.
///             We also flatten the esl namespaces through usage of the 'using'
///             namespace declaration.
///
///             We simplify the interface to the identity<t> system, by
///             creating a python_identity type that can be converted without
///             checking to all other identities.
///
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


#include <boost/python/to_python_converter.hpp> // to_python


////////////////////////////////////////////////////////////////////////////////
// utility
////////////////////////////////////////////////////////////////////////////////
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <random>


boost::shared_ptr<std::seed_seq> construct_seed_seq_from_list(boost::python::list list)
{

    std::vector<std::seed_seq::result_type> result_;
    for(boost::python::ssize_t i = 0; i < boost::python::len(list); ++i) {
        result_.push_back(boost::python::extract<std::seed_seq::result_type>(list[i]));
    }
    return boost::make_shared<std::seed_seq>(result_.begin(), result_.end());
}

///
/// \brief
///
/// \tparam object_t_
template<typename object_t_>
void do_release(typename boost::shared_ptr<object_t_> const &, object_t_ *)
{

}

template<typename object_t_>
typename std::shared_ptr<object_t_> to_std(typename boost::shared_ptr<object_t_> const &p)
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

#include <optional>

template<typename element_t_>
boost::python::object optional_to_python(const std::optional<element_t_> &o)
{
    if(o.has_value()){
        return boost::python::object(o.value());
    }
    return boost::python::object(); // Python's None
}



////////////////////////////////////////////////////////////////////////////////
// map indexing suite

# include <boost/python/suite/indexing/indexing_suite.hpp>


// Forward declaration
template <class Container, bool NoProxy, class DerivedPolicies>
class map_indexing_suite;

namespace detail2
{
    template <class Container, bool NoProxy>
    class final_map_derived_policies
        : public map_indexing_suite<Container,
            NoProxy, final_map_derived_policies<Container, NoProxy> >
    {

    };
}

 // By default indexed elements are returned by proxy. This can be
// disabled by supplying *true* in the NoProxy template parameter.
//
template <
    class Container,
    bool NoProxy = false,
    class DerivedPolicies
    = detail2::final_map_derived_policies<Container, NoProxy> >
class map_indexing_suite
: public indexing_suite<
        Container
        , DerivedPolicies
        , NoProxy
        , true
        , typename Container::value_type::second_type
        , typename Container::key_type
        , typename Container::key_type
    >
{
public:

    typedef typename Container::value_type value_type;
    typedef typename Container::value_type::second_type data_type;
    typedef typename Container::key_type key_type;
    typedef typename Container::key_type index_type;
    typedef typename Container::size_type size_type;
    typedef typename Container::difference_type difference_type;

    template <class Class>
    static void
    extension_def(Class& cl)
    {
        cl.def("keys", &keys);
        cl.def("values", &values);

        //  Wrap the map's element (value_type)
        std::string elem_name = "map_indexing_suite_";
        object class_name(cl.attr("__name__"));
        extract<std::string> class_name_extractor(class_name);
        elem_name += class_name_extractor();
        elem_name += "_entry";

        typedef typename boost::mpl::if_<
            boost::mpl::and_<boost::is_class<data_type>, boost::mpl::bool_<!NoProxy> >
        , return_internal_reference<>
            , default_call_policies
              >::type get_data_return_policy;

        class_<value_type>(elem_name.c_str())
            .def("__repr__", &DerivedPolicies::print_elem)
            .def("data", &DerivedPolicies::get_data, get_data_return_policy())
            .def("key", &DerivedPolicies::get_key)
            ;
    }

    static object
    print_elem(typename Container::value_type const& e)
    {
        return "(%s, %s)" % boost::python::make_tuple(e.first, e.second);
    }

    static
    typename boost::mpl::if_<
        boost::mpl::and_<boost::is_class<data_type>, boost::mpl::bool_<!NoProxy> >
    , data_type&
    , data_type
    >::type
    get_data(typename Container::value_type& e)
    {
        return e.second;
    }

    static typename Container::key_type
    get_key(typename Container::value_type& e)
    {
        return e.first;
    }

    static data_type&
    get_item(Container& container, index_type i_)
    {
        auto i = container.find(i_);
        if (i == container.end()){
            PyErr_SetString(PyExc_KeyError, "Invalid key");
            throw_error_already_set();
        }
        return i->second;
    }

    static void
    set_item(Container& container, index_type i, data_type const& v)
    {
        container[i] = v;
    }

    static void
    delete_item(Container& container, index_type i)
    {
        container.erase(i);
    }

    static size_t
    size(Container& container)
    {
        return container.size();
    }

    static bool
    contains(Container& container, key_type const& key)
    {
        return container.find(key) != container.end();
    }

    static bool
    compare_index(Container& container, index_type a, index_type b)
    {
        return container.key_comp()(a, b);
    }

    [[nodiscard]] static boost::python::list keys(const Container &container)
    {
        boost::python::list result_;
        for(const auto &[k, v]: container){
            result_.append(k);
        }
        return result_;
    }

    [[nodiscard]] static boost::python::list values(const Container &container)
    {
        boost::python::list result_;
        for(const auto &[k, v]: container){
            result_.append(v);
        }
        return result_;
    }



    static index_type
    convert_index(Container& /*container*/, PyObject* i_)
    {
        extract<key_type const&> i(i_);
        if (i.check())
        {
            return i();
        }else{
            extract<key_type> i(i_);
            if (i.check())
                return i();
        }

        PyErr_SetString(PyExc_TypeError, "Invalid index type");
        throw_error_already_set();
        return index_type();
    }
};













////////////////////////////////////////////////////////////////////////////////
// esl top level
////////////////////////////////////////////////////////////////////////////////
#include <esl/agent.hpp>
#include <esl/exception.hpp>
#include <esl/version.hpp>
#include <esl/quantity.hpp>
using namespace esl;


#include <esl/simulation/identity.hpp>

using esl::python_identity;



class python_agent
: public agent
, public boost::python::wrapper<agent>
{
public:
    python_agent(python_identity i)
    : agent(reinterpret_identity_cast<agent>(i))
    {

    }


    simulation::time_point act(simulation::time_interval step,
                               std::seed_seq &seed) override
    {
        auto o = get_override("act");
        if(o){
            boost::python::object result_ = o(step, boost::ref(seed));

            if(result_.is_none()){
                return simulation::time_point(step.upper);
            }

            return boost::python::extract<simulation::time_point>(result_);
        }
        return agent::act(step, boost::ref(seed));
    }

    std::string describe() const override
    {
        auto o = get_override("describe");
        if(o){
            boost::python::object result_ = o();
            return boost::python::extract<std::string>(result_);
        }
        return agent::describe();
    }
};









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
    PyErr_SetString (   PyExc_RuntimeError, e.what());
}

////////////////////////////////////////////////////////////////////////////////
// esl.computation
////////////////////////////////////////////////////////////////////////////////
#include <esl/computation/python_module_computation.hpp>
#include <esl/computation/block_pool.hpp>
#include <esl/computation/environment.hpp>
#include <esl/computation/timing.hpp>
using namespace esl::computation;



///
/// \brief  This class enables access to the computation::environment
///         internal functions for Python derived classes.
///
/// \details    Python does not have access via inheritance of protected
///             members, so we need to make everything public
///
class python_environment
: public esl::computation::environment
, public wrapper<esl::computation::environment>
{
public:
    python_environment()
    : esl::computation::environment()
    {

    }

    virtual ~python_environment() = default;






    void step(simulation::model &simulation) override
    {
        boost::python::override method_ = get_override("step");

        if(!method_.is_none()){
            method_(simulation);
            return;
        }
        environment::step(simulation);
    }

    void run(simulation::model &simulation) override
    {
        boost::python::override o = get_override("run");

        if(!o.is_none()){
            o(simulation);
            return;
        }
        environment::run(simulation);
    }

    size_t activate() override
    {
        boost::python::override method_ = get_override("activate");
        if(!method_.is_none()){
            boost::python::object result_ = method_();
            return boost::python::extract<size_t>(result_);
        }
        return environment::activate();
    }


    size_t deactivate() override
    {
        boost::python::override method_ = get_override("deactivate");
        if(!method_.is_none()){
            boost::python::object result_ = method_();
            return boost::python::extract<size_t>(result_);
        }
        return environment::deactivate();
    }

    void before_step() override
    {
        boost::python::override method_ = get_override("before_step");
        if(!method_.is_none()){
            method_();
        }else {
            environment::before_step();
        }
    }

    void after_step(simulation::model &simulation) override
    {
        boost::python::override method_ = get_override("after_step");
        if(!method_.is_none()){
            method_(simulation);
        }else{
            environment::after_step(simulation);
        }
    }

    void after_run(simulation::model &simulation) override
    {
        boost::python::override method_ = get_override("after_run");
        if(!method_.is_none()){
            method_(simulation);
        }else {
            environment::after_run(simulation);
        }
    }

    void activate_agent_python(const python_identity a)
    {
        environment::activate_agent(reinterpret_identity_cast<agent>(a));
    }

    void deactivate_agent_python(const python_identity a)
    {
        environment::deactivate_agent(reinterpret_identity_cast<agent>(a));
    }

    size_t send_messages(simulation::model &simulation)
    {
        boost::python::override method_ = get_override("send_messages");
        if(!method_.is_none()){
            boost::python::object result_ = method_(simulation);
            return boost::python::extract<size_t>(result_);
        }else {
            return environment::send_messages(simulation);
        }
    }
};






////////////////////////////////////////////////////////////////////////////////
// esl.computation.distributed
////////////////////////////////////////////////////////////////////////////////
#include <esl/computation/distributed/protocol.hpp>
using namespace esl::computation::distributed;


////////////////////////////////////////////////////////////////////////////////
// esl.data
////////////////////////////////////////////////////////////////////////////////
#include <esl/data/severity.hpp>
#include <esl/data/log.hpp>

void python_log_detailed( esl::data::severity level
                        , boost::python::object o
                        , const std::string &function
                        , const std::string &file
                        , size_t line
                        )
{
    auto render_ = boost::python::extract<const char *>(boost::python::str(o));
    switch(level){
    case severity::trace:
        main_log.get<severity::trace>(function.c_str(), file.c_str(), static_cast<unsigned int>(line)) << render_ << std::endl;
        break;
    case severity::notice:
        main_log.get<severity::notice>(function.c_str(), file.c_str(), static_cast<unsigned int>(line)) << render_ << std::endl;
        break;
    case severity::warning:
        main_log.get<severity::notice>(function.c_str(), file.c_str(), static_cast<unsigned int>(line)) << render_ << std::endl;
        break;
    case severity::errorlog:
        main_log.get<severity::notice>(function.c_str(), file.c_str(), static_cast<unsigned int>(line)) << render_ << std::endl;
        break;

    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//esl.economics
////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/iso_4217.hpp>
#include <esl/economics/currencies.hpp>
#include <esl/economics/price.hpp>
#include <esl/economics/company.hpp>
using namespace esl::economics;

#include <esl/law/jurisdiction.hpp>
#include <esl/law/organization.hpp>
using namespace esl::law;


std::string python_currency_code(const iso_4217 &c)
{
    return (std::string() + c.code[0]) + c.code[1];
}

double python_price_to_floating_point(const price &p)
{
    return double(p);
}




struct python_organization
: public virtual organization
, public wrapper<organization>
{
    python_organization(const python_identity &i, const jurisdiction &j)
    : organization(reinterpret_identity_cast<organization>(i),j)
    {

    }
};



struct python_company
: public company
, public wrapper<company>
{
public:

    python_company(const python_identity &i, const law::jurisdiction &j)
    : company(reinterpret_identity_cast<company>(i),j)
    , wrapper<company>()
    {

    }


    std::optional<finance::dividend_policy>
    upcoming_dividend(simulation::time_interval interval,
                      std::seed_seq &seed) override
    {
        boost::python::override policy_ = wrapper<company>::get_override("upcoming_dividend")(interval, seed);


        if(policy_.is_none()){
            return {};
        }

        return extract<finance::dividend_policy>(policy_);
    }

};



////////////////////////////////////////////////////////////////////////////////
//esl.economics.accounting
////////////////////////////////////////////////////////////////////////////////
#include <esl/economics/accounting/standard.hpp>
using namespace esl::economics::accounting;

//we need to forward declare items from the finance submodule
#include <esl/economics/finance/stock.hpp>
#include <esl/economics/finance/bond.hpp>
#include <esl/economics/cash.hpp>
using namespace esl::economics::finance;


// because python has no ad-hoc polymorphism for function overloads,
// we need to wrap these.

price value_money(standard &s, const money &p, const quantity &q)
{
    return s.value(p, q);
}

price value_cash(standard &s, const cash &p, const quantity &q)
{
    return s.value(p, q);
}

price value_stock(standard &s, const stock &p, const quantity &q)
{
    return s.value(p, q);
}

price value_loan(standard &s, const loan &p, const quantity &q)
{
    return s.value(p, q);
}

price value_securities_lending_contract(standard &s, const securities_lending_contract &p, const quantity &q)
{
    return s.value(p, q);
}

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
#include <esl/economics/markets/ticker.hpp>

#include <esl/economics/markets/centralized_exchange.hpp>
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

boost::shared_ptr<quote> construct_quote_from_price(price p)
{
    return boost::make_shared<quote>(p);
}


boost::shared_ptr<quote> construct_quote_from_exchange_rate(const exchange_rate &e)
{
    return boost::make_shared<quote>(e);
}


boost::shared_ptr<ticker> python_ticker_constructor(const python_identity &base_property = python_identity()
             , const python_identity &quote_property = python_identity())
{
    return boost::make_shared<ticker>( reinterpret_identity_cast<esl::law::property>(base_property)
                 , reinterpret_identity_cast<esl::law::property>(quote_property));
}


boost::shared_ptr<centralized_exchange> python_centralized_exchange_constructor(const python_identity &identity = python_identity())
{

    std::vector<markets::ticker> traded_;

    return boost::make_shared<centralized_exchange>(
        reinterpret_identity_cast<centralized_exchange>(identity), traded_);
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

boost::shared_ptr<limit_order> python_limit_order_message_constructor
    (ticker symbol
    , const python_identity &owner
    , limit_order::side_t side
    , const quote &limit
    , std::uint64_t quantity
    , limit_order::lifetime_t lifetime // = good_until_cancelled
    )
{
    return boost::make_shared<limit_order>(
        symbol, reinterpret_identity_cast<agent>(owner), side, limit, quantity, lifetime
        );
}

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
        const python_identity &sender       = python_identity(),
        const python_identity& recipient     = python_identity(),
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
        , std::tuple<quote, esl::variable>> &quotes) const override
    {
        dict quotes_;

        for(const auto &[i, v]: quotes){
            auto t = make_tuple(std::get<0>(v), esl::value(std::get<1>(v)));
            quotes_.setdefault(i, t);
        }

        // specify the type, so that the return value is converted to python::object
        //boost::python::override return_value_ = get_override("excess_demand")(quotes_);

        boost::python::override method_ = get_override("excess_demand");
        if(!method_.is_none()){
            boost::python::object return_value_ = method_(quotes_);

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
        }//else{
        return {};
    }
};

boost::python::dict get_differentiable_order_message_supply(const esl::economics::markets::walras::differentiable_order_message &m)
{
    boost::python::dict result_;
    for(const auto &[k,v] : m.supply){
        result_[k] = boost::python::make_tuple(boost::python::object(std::get<0>(v)), boost::python::object(std::get<1>(v)));
    }
    return result_;
}

void set_differentiable_order_message_supply(esl::economics::markets::walras::differentiable_order_message &m, boost::python::dict t)
{
    m.supply.clear();
    auto items_ = t.items();

    for(boost::python::ssize_t i = 0; i < boost::python::len(items_); ++i){
        boost::python::tuple tuple_ = boost::python::tuple(items_[i]);

        python_identity k = boost::python::extract<python_identity>(tuple_[0]);

        auto v = boost::python::tuple(tuple_[1]);
        esl::quantity long_ = boost::python::extract<quantity>(v[0]);
        esl::quantity short_ = boost::python::extract<quantity>(v[1]);

        std::tuple<esl::quantity, esl::quantity> T = { long_, short_};

        m.supply.insert({esl::identity<esl::law::property>(k.digits), T});
    }
}


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
boost::python::dict compute_clearing_quotes(python_excess_demand_model *e)
{
    auto quotes_ = e->compute_clearing_quotes();
    boost::python::dict result_;

    if(quotes_.has_value()){
        for(const auto &[k,v]: quotes_.value()){
            auto i = python_identity(k);
            result_[boost::python::object(i)] = boost::python::object(v);
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
//# include <boost/python/suite/indexing/map_indexing_suite.hpp>

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


///
/// \brief  Provides interface for a multimap<k,v> to Python
///
///
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
        cl.def("keys", &keys);
        cl.def("values", &values);

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

    [[nodiscard]] static boost::python::list keys(const multimap_t_ &container)
    {
        boost::python::list result_;
        for(const auto &[k, v]: container){
            result_.append(k);
        }
        return result_;
    }

    [[nodiscard]] static boost::python::list values(const multimap_t_ &container)
    {
        boost::python::list result_;
        for(const auto &[k, v]: container){
            result_.append(v);
        }
        return result_;
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
#include <esl/economics/company.hpp>
using namespace esl::law;

std::string python_iso_17442_local(esl::law::iso_17442 &e)
{
    std::stringstream stream_;
    stream_.write(e.local.data(), e.local.size());
    return stream_.str();
}

std::string python_iso_17442_code(esl::law::iso_17442 &e)
{
    std::stringstream stream_;
    stream_.write(e.code.data(), e.code.size());
    return stream_.str();
}

std::string python_iso_17442_checksum(esl::law::iso_17442 &e)
{
    std::stringstream stream_;
    auto t = e.checksum();
    stream_<< std::get<0>(t) << std::get<1>(t);
    return stream_.str();
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
#include <esl/simulation/model.hpp>
#include <esl/simulation/time.hpp>
#include <esl/simulation/world.hpp>
using namespace esl::simulation;


template<typename entity_t_>
python_identity create_identity(entity_t_ &e)
{
    auto i = e.template create<object>();
    return i;
}


template<typename identity_t_>
boost::shared_ptr<identity_t_>
convert_digit_list_generic(const boost::python::list &list)
{
    std::vector<uint64_t> result_;
    for(boost::python::ssize_t i = 0; i < boost::python::len(list); ++i) {
        result_.push_back(boost::python::extract<std::uint64_t>(list[i]));
    }
    return boost::make_shared<identity_t_>(result_);
}



boost::python::dict agent_collection_local_agents(const agent_collection &c)
{
    boost::python::dict result_;

    for(const auto &[k,v]: c.local_agents_){
        result_[boost::python::object(k)] = boost::python::object(v);
    }

    return result_;
}





BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( python_identity_representation_overload
                                      , python_identity::representation
                                      , 0
                                      , 1);

size_t python_identity_hash(const python_identity &p)
{
    return std::hash<python_identity>()(p);
}




///
///
///
/// \param agent_constructor
/// \return
boost::shared_ptr<esl::agent> python_agent_collection_create_identifier(
    esl::simulation::agent_collection &a
    , boost::python::object parent)
{
    (void)a;
    (void)parent;
    boost::shared_ptr<esl::agent> result_;
    return result_;
}

///
///
///
/// \param agent_constructor
/// \return
boost::shared_ptr<esl::agent> python_agent_collection_create( agent_collection &
                                                             , boost::python::object )
{
    boost::shared_ptr<esl::agent> result_;
    //boost::python::call<int>(agent_constructor);
    return result_;
}

///
/// \param agent
void python_agent_collection_activate(agent_collection &c, boost::shared_ptr<esl::agent> a)
{
    c.activate(to_std(a));
}

///
/// \param agent
void python_agent_collection_deactivate(agent_collection &c, boost::shared_ptr<esl::agent> a)
{
    c.deactivate(to_std(a));
}

/// \brief  Export time_point constructor to Python.
///
/// \param o
/// \return
time_point python_time_point(const object& o)
{
    return extract<time_point>(o);
}

///
/// \brief  Export time_duration constructor to Python.
///
/// \param o
/// \return
time_duration python_time_duration(const object& o)
{
    return extract<time_duration>(o);
}


////////////////////////////////////////////////////////////////////////////////
// esl.simulation.parameter
////////////////////////////////////////////////////////////////////////////////
#include <esl/simulation/parameter/parametrization.hpp>

using namespace esl::simulation::parameter;

class python_model
: public model
, public wrapper<model>
{
public:
    python_model(environment &e, const parameter::parametrization &p)
        : model(e, p)
        , wrapper<model>()
    {

    }


    ~python_model() override = default;

    void initialize() override
    {
        boost::python::override method_ = get_override("initialize");
        if(method_.is_none()){
            model::initialize();
        }else{
            method_();
        }
    }

    ///
    /// \brief  Call Python code for user defined model logic.
    ///         Because we can't enforce return type as we would in C++,
    ///         we are more permissive and default to jumping over the entire
    ///         interval if no next event has been specified.
    ///
    /// \param step
    /// \return
    time_point step(time_interval step) override
    {
        boost::python::override method_ = get_override("step");

        if(method_.is_none()){
            return model::step(step);
        }

        boost::python::object result_ = method_(step);
        if(result_.is_none()){
            return step.upper;
        }
        return boost::python::extract<time_point>(result_);
    }

    void terminate() override
    {
        boost::python::override method_ = get_override("terminate");
        if(method_.is_none()){
            model::terminate();
        }else{
            method_();
        }
    }
};


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

///
/// \brief  Because Python allows returning many types from a single function,
///         we can wrap the function by packing the result in a python object.
///
/// \param p    The parameter set
/// \param name The parameter name
/// \return     The parameter value as a python object
boost::python::object parametrization_get_helper(const parametrization &p, const std::string &name)
{
    auto parameter_ = p.values.find(name);

    if(p.values.end() == parameter_) {
        throw esl::exception("not a valid parameter: " + name + "");
    }

    auto result_ = pack<double>(parameter_->second);
    if(result_.has_value()){
        return result_.value();
    }

    result_ = pack<std::int64_t>(parameter_->second);
    if(result_.has_value()){
        return result_.value();
    }

    result_ = pack<std::uint64_t>(parameter_->second);
    if(result_.has_value()){
        return result_.value();
    }

    throw esl::exception("parametrization[" + name + "] can't be converted to Python");
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

    ////////////////////////////////////////////////////////////////////////////
    // utility
    ////////////////////////////////////////////////////////////////////////////


    class_<std::seed_seq, boost::shared_ptr<std::seed_seq>,  boost::noncopyable>("seed", no_init)\
        .def("__init__", boost::python::make_constructor( construct_seed_seq_from_list))

        .def_readonly("size", &std::seed_seq::size)

        .def("generate", +[](std::seed_seq &seed){
                std::vector<std::uint32_t> seeds_(1);
                seed.generate(seeds_.begin(), seeds_.end());
                return seeds_[0];
        })


        .def("__str__", +[](std::seed_seq &seed){
                std::stringstream stream_;
            stream_ << "seed {";
            seed.param(std::ostream_iterator<typename std::seed_seq::result_type>(stream_, ", "));
            stream_ << "}";
            return stream_.str();
        })


        ;




    ////////////////////////////////////////////////////////////////////////////
    // top level module
    ////////////////////////////////////////////////////////////////////////////
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

        //class_<entity<void>>("entity");

        class_< python_agent
              , boost::noncopyable
              //, bases<communicator>
              >("agent", init<python_identity>())

            .def("describe", &python_agent::describe)
            .def("act", &python_agent::act)
            .add_property( "identifier"
                         , +[](const python_agent &a){return (python_identity)(a.identifier); }
                         )

            .add_property( "inbox"
                         , +[](const python_agent &a){return a.inbox; }
                         )

            .add_property("outbox"
                         , +[](const python_agent &a){return a.outbox; }
                         )

            .def("create", &create_identity<agent>)
            .def("__repr__", &python_agent::describe)
            .def("__str__", &python_agent::describe)
            ;



    def("version", version, "Print the library's version.");

    ////////////////////////////////////////////////////////////////////////////
    // esl.computation
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_computation_ = create_scope("_computation");

        class_<block_pool::block<object>>(
            "block", "The block is a unit of memory used by the block_pool allocator, and has fixed size.")
            .def_readwrite("data",
                           &computation::block_pool::block<object>::data)
            .def_readwrite("index",
                           &computation::block_pool::block<object>::index);

        // computational environment base class with default single thread
        class_<python_environment, boost::noncopyable>(
            "environment", "The environment class runs models: it schedules agents and delivers messages sent between agents.")
            .def("step", &python_environment::step)
            .def("run", &python_environment::run)
            .def("activate", &python_environment::activate)
            .def("deactivate", &python_environment::deactivate)
            .def("before_step", &python_environment::before_step)
            .def("after_step", &python_environment::after_step)
            .def("after_run", &python_environment::after_run)
            .def("activate_agent", &python_environment::activate_agent)
            .def("deactivate_agent", &python_environment::deactivate_agent)
            .def("send_messages", &python_environment::send_messages)
            ;

        // timing information
        class_<computation::agent_timing>(
            "agent_timing",
            "Contains performance metrics for individual agents.")
            .def_readwrite("messaging", &agent_timing::messaging)
            .def_readwrite("acting", &agent_timing::acting);


        {
            boost::python::scope scope_distributed_ =
                create_scope("_distributed");

            scope().attr("__doc__") = "submodule for distributed computing using MPI";

            class_<activation>("activation")
                .def_readwrite("location", &activation::location)
                .def_readwrite("activated", &activation::activated);

            class_<migration>("migration")
                .def_readwrite("source", &migration::source)
                .def_readwrite("target", &migration::target)
                .def_readwrite("migrant", &migration::migrant);

            class_<deactivation>("deactivation")
                .def_readwrite("deactivated", &deactivation::deactivated);
        }

//        class_<environment>("environment")
//            .def("run", &environment::run)
//            .def("step", &environment::step)
//            .def("activate", &environment::activate)
//            .def("deactivate", &environment::deactivate)
//            .def("before_step", &environment::before_step)
//            .def("after_step", &environment::after_step)
//            .def("after_run", &environment::after_run)
//            .def("activate_agent", &environment::activate_agent)
//            .def("deactivate_agent", &environment::deactivate_agent)
//            .def("send_messages", &environment::send_messages)
//        ;
//


    }


    ////////////////////////////////////////////////////////////////////////////
    // esl.data
    ////////////////////////////////////////////////////////////////////////////
    {
        boost::python::scope scope_data_ = create_scope("_data");


        enum_<severity>("severity")
            .value("trace",     severity::trace)
            .value("notice",    severity::notice)
            .value("warning",   severity::warning)
            .value("error",  severity::errorlog)
            ;

        def("log_detailed", &python_log_detailed);

        //     esl::data::main_log.get<level>(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__)

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

        class_<std::map<finance::share_class, std::uint64_t>>("map_share_class_uint64_t")
            .def(map_indexing_suite<std::map<finance::share_class, std::uint64_t>>())
        ;

        //class_<std::unordered_map<identity<shareholder>, std::map<finance::share_class, std::uint64_t>>>("unordered_map_shareholder_identity_share_class_uint64_t")
        //    .def(map_indexing_suite<std::unordered_map<identity<shareholder>, std::map<finance::share_class, std::uint64_t>>>())
        //**;



        class_<python_organization /*, bases<legal_person>*/, boost::noncopyable>("organization"
              , init<const python_identity &,const jurisdiction &>())
            ;


        class_< python_company, bases<python_organization>, boost::noncopyable
               >( "company", init<const python_identity &, const law::jurisdiction &>())
            .add_property("balance_sheet", +[](python_company &c) { return c.balance_sheet; })
            .add_property("shares_outstanding", +[](python_company &c) { return c.shares_outstanding; })
            .add_property("shareholders", +[](python_company &c) { return c.shareholders; })
            .def("unique_shareholders", +[](const python_company &c)
                                       {
                                           boost::python::list result_;
                                           for(const auto &sh : c.unique_shareholders()){
                                               result_.append((python_identity)sh);
                                           }
                                           return result_;
                                       })
            .def("total_shares", &python_company::company::total_shares)

        ;





        ////////////////////////////////////////////////////////////////////////////
        // esl.economics.accounting
        ////////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_accounting_ = create_scope("_accounting");

            class_<std::map<identity<law::property>, markets::quote> >("map_property_identifier_quote")
                .def(map_indexing_suite<std::map<identity<law::property>, markets::quote>>())
                ;

            class_<standard>("standard"
                            , "A basic accounting standard"
                            ,init<iso_4217>())
                .add_property("reporting_currency", &standard::reporting_currency)
                .add_property("foreign_currencies", &standard::foreign_currencies)
                .def("value", &value_money)
                .def("value", &value_cash)
                .def("value", &value_stock)
                .def("value", &value_loan)
                .def("value", &value_securities_lending_contract)
                ;
        }

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
//            scope().attr("USN")                    = currencies::USN;
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
            //.def(self * std::uint64_t)
            .def("__repr__", &price::representation)
            .def("__str__", &price::representation)
            .def("__float__", &python_price_to_floating_point);

        ////////////////////////////////////////////////////////////////////////
        // esl.economics.finance
        ////////////////////////////////////////////////////////////////////////
        {
            boost::python::scope scope_finance = create_scope("_finance");

            class_<finance::isin>("iso_6166", no_init)
                .def(init<geography::iso_3166_1_alpha_2, std::string>())
                .def(init<std::string>())
                .def(init<std::string, std::string>())
                .def(init<geography::iso_3166_1_alpha_2, cusip>())

                .add_property("issuer", &finance::isin::issuer)
                .add_property("code", &get_isin_code, &set_isin_code)
                .def("__repr__", &finance::isin::representation)
                .def("__str__", &finance::isin::representation)
                .def("checksum", +[](const finance::isin &i){ std::string s = ""; s += i.checksum(); return s;})
                ;


            class_<finance::cusip>("cusip", no_init)
            .def(init<std::string>())
            .add_property("issuer"
                         , +[](const finance::cusip &c){
                                return std::string(c.issuer.begin(), c.issuer.end());
                            })

            .def("__repr__", &finance::cusip::representation)
            .def("__str__", &finance::cusip::representation)
            .def("checksum", +[](const finance::cusip &c){  std::string s = ""; s += c.checksum(); return s; })
            ;


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

            class_<std::map<share_class, std::tuple<std::uint64_t, price>>>("map_share_class_uint64_t_price")
                .def(map_indexing_suite<std::map<share_class, std::tuple<std::uint64_t, price>>>());

            class_<finance::dividend_policy>("dividend_policy",
                                             init<time_point, time_point,time_interval,time_point,iso_4217,std::map<share_class, std::tuple<std::uint64_t, price>>>()
                                             )
                .add_property("announcement_date", &dividend_policy::announcement_date)
                .add_property("ex_dividend_date", &dividend_policy::ex_dividend_date)
                .add_property("dividend_period", &dividend_policy::dividend_period)
                .add_property("payable_date", &dividend_policy::payable_date)
                .add_property("dividend_currency", &dividend_policy::dividend_currency)
                .add_property("dividend_per_share", &dividend_policy::dividend_per_share)
                .def("total_dividends", &dividend_policy::total_dividends)
            ;

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

            class_<quote, boost::shared_ptr<quote>>("quote", no_init)
                .def("__init__", boost::python::make_constructor( construct_quote_from_price))
                .def("__init__", boost::python::make_constructor( construct_quote_from_exchange_rate))
                .add_property("price", &quote_helper_get_price, &quote_helper_set_price)
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


            class_<ticker>("ticker", no_init)
                .def("__init__", make_constructor(python_ticker_constructor))
                .add_property("base"
                    , +[](const ticker &r){return reinterpret_identity_cast<python_identity>(r.base); }
                    , +[](ticker &r, const python_identity &i){ r.base = reinterpret_identity_cast<law::property>(i); }
                               )
                .add_property("quote"
                    , +[](const ticker &r){return (python_identity)(r.quote); }
                    , +[](ticker &r, const python_identity &i){ r.quote = reinterpret_identity_cast<law::property>(i); }
                )
                .def(self == self)
                .def(self != self)
                .def(self < self)
                .def(self > self)
                .def(self <= self)
                .def(self >= self);

            enum_<indication>("indication")
                .value("firm", indication::firm)
                .value("indicative", indication::indicative);


            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // centralized exchange
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////


            class_<centralized_exchange, bases<market>>("centralized_exchange", no_init)
                .def("__init__", make_constructor(python_centralized_exchange_constructor))
                .def_readwrite("positions", &centralized_exchange::positions)
            ;

            class_<centralized_exchange::position_report>("position_report", init<std::uint64_t,std::uint64_t,std::uint64_t>())
            .def_readwrite("supply", &centralized_exchange::position_report::supply)//, "The number of tradeable economic property brought to the exchange by the agent")
            .def_readwrite("bought", &centralized_exchange::position_report::bought)
            .def_readwrite("sold", &centralized_exchange::position_report::sold)
            .def("liquidity_provided", &centralized_exchange::position_report::liquidity_provided)
            .def("liquidity_taken", &centralized_exchange::position_report::liquidity_taken)

            ;


            class_<logon_message, bases<header>>("logon_message", init<>())
            .def_readwrite("supply", &logon_message::supply)
            ;


            class_<logout_message, bases<header>>("logout_message", init<>())
            ;


            enum_<market_data_request_message::request_t>("request_t")
            .value("snapshot", market_data_request_message::request_t::snapshot)
            .value("stream", market_data_request_message::request_t::stream)
            .value("cancel", market_data_request_message::request_t::cancel)
            ;



            class_<market_data_request_message, bases<header>>("market_data_request_message", init<>())
            .def_readwrite("instruments", &market_data_request_message::instruments)
            .def_readwrite("type", &market_data_request_message::type)
            .def_readwrite("depth", &market_data_request_message::depth)
            ;


            class_<order_status_request, bases<header>>("order_status_request")
            ;


            class_<order_status_response_message, bases<header>>("order_status_response_message")
            ;


            class_<new_order_single, bases<header>>("new_order_single")
            .def_readwrite("order_details", &new_order_single::order_details)
            ;



            class_<execution_report_message, bases<header>>("execution_report_message")
            .def_readwrite("report", &execution_report_message::report)
            ;


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

                class_<std::vector<execution_report>>("execution_reports")
                    .def(vector_indexing_suite<std::vector<execution_report>>())
                    .def("clear", +[](std::vector<execution_report> &e){return e.clear();})
                    //.def("size", +[](const std::vector<execution_report> &e){return e.size();}) // superseeded by len(e)
                    ;

                enum_<execution_report::state_t>("execution_state")
                    .value("invalid", execution_report::state_t::invalid)
                    .value("cancel", execution_report::state_t::cancel)
                    .value("match", execution_report::state_t::match)
                    .value("placement", execution_report::state_t::placement);

                class_<execution_report>("execution_report"
                                         , init<execution_report::state_t
                    , limit_order::side_t
                    , std::uint32_t
                    , std::uint64_t
                    , quote
                    , python_identity >())

                    .def_readwrite("state", &execution_report::state)
                    .def_readwrite("quantity", &execution_report::quantity)
                    .def_readwrite("identifier", &execution_report::identifier)
                    .def_readwrite("side", &execution_report::side)
                    .def_readwrite("limit", &execution_report::limit)
                    .add_property ("owner"
                                   , +[](const execution_report &r){return reinterpret_identity_cast<python_identity>(r.owner); }
                                  , +[](execution_report &r, const python_identity &i){ r.owner = reinterpret_identity_cast<agent>(i); }
                                  )
                    .def("__repr__", &execution_report::representation)
                    .def("__str__", &execution_report::representation);

                // for list of order identifiers
                class_<std::vector<basic_book::order_identifier>>("order_identifier_list")
                    .def(vector_indexing_suite<std::vector<basic_book::order_identifier>>())
                    .def("clear", +[](std::vector<basic_book::order_identifier> &e){return e.clear();})
                    ;


                enum_<limit_order::side_t>("side_t")
                    .value("buy", limit_order::side_t::buy)
                    .value("sell",  limit_order::side_t::sell)
                    ;

                enum_<limit_order::lifetime_t>("lifetime_t")
                    .value("good_until_cancelled", limit_order::lifetime_t::good_until_cancelled)
                    .value("fill_or_kill",  limit_order::lifetime_t::fill_or_kill)
                    .value("immediate_or_cancel",  limit_order::lifetime_t::immediate_or_cancel)
                    ;

                ///
                /// \brief Export the abstract base class, so that python users too can
                ///        implement new order books.
                ///
                class_<limit_order>("limit_order", no_init)
                    .def("__init__", make_constructor(python_limit_order_message_constructor))
                    .def_readwrite("lifetime", &limit_order::lifetime)
                    .def_readwrite("side", &limit_order::side)
                    .def_readwrite("symbol", &limit_order::symbol)
                    .add_property ( "owner"
                                    , +[](const limit_order &r){return reinterpret_identity_cast<python_identity>(r.owner); }
                                  , +[](limit_order &r, const python_identity &i){ r.owner = reinterpret_identity_cast<agent>(i); } )
                    .def_readwrite("limit", &limit_order::limit)
                    .def_readwrite("quantity", &limit_order::quantity)
                    ;

                class_<std::vector<limit_order>>("limit_orders")
                    .def(vector_indexing_suite<std::vector<limit_order>>());

                ///
                /// \brief Export the abstract base class, so that python users too can
                ///        implement new order books.
                ///
                class_<basic_book, boost::noncopyable>("basic_book", no_init)
                    .def_readwrite("reports", &basic_book::reports)
                    .def("ask", +[](const basic_book& b) { return optional_to_python<quote>(b.ask()); })
                    .def("bid", +[](const basic_book& b) { return optional_to_python<quote>(b.bid()); })
                    .def("insert", &basic_book::insert)
                    .def("cancel", &basic_book::cancel)
                    .def("orders", +[](const basic_book& b) {
                            boost::python::list result_;
                            for(auto i: b.orders()){
                                result_.append(i);
                            }
                            return result_;
                        })
                    .def("display", &basic_book::display);

                //
                class_<static_order_book, bases<basic_book>>
                    ( "static_order_book"
                    , "Limit order book optimized for fast throughput. Uses statically allocated memory pool."
                    , init<quote, quote, size_t>())
                    .def_readwrite("reports", &basic_book::reports)
                    ;

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
                    init<python_identity,
                         python_identity,
                         esl::simulation::time_point,
                         esl::simulation::time_point>())
                    .add_property(
                        "supply",
                        &get_differentiable_order_message_supply, &set_differentiable_order_message_supply);

                // expose vector of messages to Python
                class_<messages_t>("messages_t")
                    .def(vector_indexing_suite<messages_t>());

                class_<python_excess_demand_model, boost::noncopyable>(
                    "excess_demand_model", no_init)  // non-trivial constructor
                    .def("__init__",
                         make_constructor(
                             &excess_demand_model_python_constructor))
                    .def_readwrite("circuit_breaker", &tatonnement::excess_demand_model::circuit_breaker)
                    .def_readwrite("methods", &tatonnement::excess_demand_model::methods)
                    .def_readwrite("quotes", &tatonnement::excess_demand_model::quotes)
                    .def("compute_clearing_quotes", &compute_clearing_quotes)
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

        class_<esl::geography::iso_3166_1_alpha_2>("iso_3166_1_alpha_2", init<std::string>())
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
            multimap_indexing_suite<communicator::inbox_t>())
//            .def( "insert"
//                , +[](const ... &r){return ; }
//                )
            ;


        class_<communicator::outbox_t>("outbox_t")
            .def(
                boost::python::vector_indexing_suite<communicator::outbox_t>());

        class_<communicator>("communicator")
            .def("send_message", send_message_python)
            .def_readwrite("inbox", &communicator::inbox)
            .def_readwrite("outbox", &communicator::outbox);


        class_<header>("header",
                       init<message_code, python_identity, python_identity,
                            simulation::time_point, simulation::time_point>())
            .def(init<message_code, python_identity, python_identity,
                      simulation::time_point>())
            .def(init<message_code, python_identity, python_identity>())
            .def(init<message_code, python_identity>())
            .def(init<message_code>())
            .def(init<message_code>())
            .def_readwrite("type", &header::type)

            .add_property("sender"
                , +[](const header &r){return (python_identity)(r.sender); }
                , +[](header &r, const python_identity &i){ r.sender = reinterpret_identity_cast<agent>(i); }
            )

            .add_property("recipient"
                , +[](const header &r){return (python_identity)(r.recipient); }
                , +[](header &r, const python_identity &i){ r.recipient = reinterpret_identity_cast<agent>(i); }
            )

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

        class_<iso_17442>("iso_17442", init<std::string>())
            .add_property("local", &python_iso_17442_local)
            .add_property("code", &python_iso_17442_code)
            .def("checksum", &python_iso_17442_checksum)
            ;

        class_<legal_entity>("legal_entity")
            ;

        class_<jurisdiction>("jurisdiction", init<geography::iso_3166_1_alpha_2, economics::iso_4217>())
            .add_property("sovereign", &esl::law::jurisdiction::sovereign)
            .add_property("tender", &esl::law::jurisdiction::tender)
            ;


        //
        //  \brief this is not to be used, it is there to complete the type system
        //
        class_<identity<property>>( "property_identity")
            .def("__init__", make_constructor(convert_digit_list_generic<identity<property>>))

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
              //, bases<entity<void>>
              >( "property", init<python_identity>())
            .def("__init__", make_constructor(+[](const python_identity &i) { return boost::make_shared<property>(reinterpret_identity_cast<property>(i)); }))
            .def("name", &property::name)
            .add_property("identifier"
                , +[](const property &r){return (python_identity)(r.identifier); }
            )

            ;

        class_<natural_person>
            ( "natural_person", init<esl::geography::iso_3166_1_alpha_2>())
            .add_property("nationality", &natural_person::nationality)
            ;

        {
            boost::python::scope scope_jurisdictions_ = create_scope("_jurisdictions");

            scope().attr("AF") = esl::law::jurisdictions::AF;
            scope().attr("AL") = esl::law::jurisdictions::AL;
            scope().attr("DZ") = esl::law::jurisdictions::DZ;
            scope().attr("AS") = esl::law::jurisdictions::AS;
            scope().attr("AD") = esl::law::jurisdictions::AD;
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
            scope().attr("MR") = esl::law::jurisdictions::MR;
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
            scope().attr("ST") = esl::law::jurisdictions::ST;
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
            "entity", init<python_identity>())
            .def_readonly("identifier", &esl::entity<object>::identifier)
            .def(self_ns::str(self_ns::self))
            .def("create", &create_identity<esl::entity<object>>)
            .def(self == self)
            .def(self != self)
            ;

        class_<python_identity>("identity")
            .def("__init__", make_constructor(convert_digit_list_generic<python_identity>))
            .def_readonly("digits", &python_identity::digits)
            .def("__str__", &python_identity::representation,
                 python_identity_representation_overload(args("width"), ""))
            .def("__repr__", &python_identity::representation,
                 python_identity_representation_overload(args("width"), ""))
            .def(self < self)
            .def(self > self)
            .def(self == self)
            .def(self != self)
            .def(self <= self)
            .def(self >= self)
            .def("__hash__", &python_identity_hash)
            ;

        class_<agent_collection>("agent_collection", init<std::reference_wrapper<computation::environment>>())
            .def("create_identifier", python_agent_collection_create_identifier)
            .def("create", python_agent_collection_create)
            .def("activate", python_agent_collection_activate)
            .def("deactivate", python_agent_collection_deactivate)
            .def("local_agents", agent_collection_local_agents)
            ;

        class_<python_model, boost::noncopyable>("model",init<environment &, const parameter::parametrization &>())
           .def_readonly("start", &python_model::start)
           .def_readwrite("end", &python_model::end)
           .def_readwrite("time", &python_model::time)
           .def_readonly("sample", &python_model::sample)
           .def("initialize", &python_model::initialize)
           .def("step", &python_model::step)
           .def("terminate", &python_model::terminate)
           .def_readonly("world", &python_model::world)
           .def_readwrite("agents", &python_model::agents)
           ;

        def("time_point", python_time_point);

        def("time_duration", python_time_duration);

        class_<time_interval>("time_interval",
                             init<time_point, time_point>())
           .def_readwrite("lower", &time_interval::lower)
           .def_readwrite("upper", &time_interval::upper)
           .def("empty", &time_interval::empty)
           .def("singleton", &time_interval::singleton)
           .def("degenerate", &time_interval::degenerate)
           .def("contains", &time_interval::contains)
           .def("__repr__", &time_interval::representation)
           .def("__str__", &time_interval::representation);


        class_<world, boost::noncopyable>("world")
            .def_readonly("identifier", &world::identifier)
            .def("__repr__", &world::representation)
            .def("__str__", &world::representation)
            .def("create", &create_identity<world>)
            ;



        implicitly_convertible<python_identity,identity<agent>>();
        implicitly_convertible<identity<agent>,python_identity>();

        implicitly_convertible<python_identity,identity<property>>();
        implicitly_convertible<identity<property>,python_identity>();


        {
        ////////////////////////////////////////////////////////////////////
        // esl.simulation.parameter
        ////////////////////////////////////////////////////////////////////
        boost::python::scope scope_parameter_ = create_scope("_parameter");

        class_<parameter_base, std::shared_ptr<parameter_base>>("parameter_base", "Abstract base class of model parameters.",init<>());

        class_<constant<double>,bases<parameter_base>>("constant_double", "Floating point number model parameter.",init<double>());
        implicitly_convertible<std::shared_ptr<constant<double>>, std::shared_ptr<parameter_base>>();

        class_<constant<std::int64_t>,bases<parameter_base>>("constant_int64", "Signed 64-bit integer model parameter.", init<int64_t>());
        implicitly_convertible<std::shared_ptr<constant<std::int64_t>>, std::shared_ptr<parameter_base>>();

        class_<constant<std::uint64_t>,bases<parameter_base>>("constant_uint64","Unsigned 64-bit integer model parameter.", init<uint64_t>());
        implicitly_convertible<std::shared_ptr<constant<std::uint64_t>>, std::shared_ptr<parameter_base>>();

        class_<constant<price>,bases<parameter_base>>("constant_price", "Price model parameter.", init<price>());
        implicitly_convertible<std::shared_ptr<constant<price>>, std::shared_ptr<parameter_base>>();


        class_<std::map<std::string, std::shared_ptr<parameter_base> > >("parameter_values", "Stores model parameters by parameter name.")
        .def(map_indexing_suite<std::map<std::string, std::shared_ptr<parameter_base> >>())
        //.def("keys", map_indexing_suite<std::map<std::string, std::shared_ptr<parameter_base> >>::)

        ;

        class_<parametrization>("parametrization", init<>())
            .def("__getitem__", parametrization_get_helper)
            .def_readwrite("values", &parametrization::values)
        ;

        }

    }
}
}//namespace esl

#endif