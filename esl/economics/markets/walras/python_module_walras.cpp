/// \file   python_module_walras.cpp
///
/// \brief
///
/// \authors    Maarten P. Scholl
/// \date       2020-10-11
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
#include <esl/economics/markets/walras/python_module_walras.hpp>
#include <esl/economics/markets/walras/price_setter.hpp>
#include <esl/economics/markets/walras/tatonnement.hpp>

#include <esl/law/property_collection.hpp>



#ifdef WITH_PYTHON

#include <esl/simulation/python_module_simulation.hpp>

#include <utility>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using boost::python::init;
using boost::python::no_init;
using boost::python::object;
using boost::python::enum_;
using boost::python::class_;
using boost::python::dict;
using boost::python::list;
using boost::python::len;
using boost::python::make_tuple;
using boost::python::extract;
using boost::python::wrapper;
using boost::python::vector_indexing_suite;

using namespace esl::economics::markets;
using namespace esl::economics::markets::tatonnement;



template<typename object_t_>
void do_release(typename boost::shared_ptr<object_t_> const&, object_t_*)
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
: public excess_demand_model
, public wrapper<excess_demand_model>
{
public:
    explicit python_excess_demand_model(esl::law::property_map<quote> initial_quotes)
        : excess_demand_model(std::move(initial_quotes))
        , wrapper<excess_demand_model>()
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

//
typedef std::vector<boost::shared_ptr<walras::differentiable_order_message>> messages_t;

///
/// \brief  converts the list of demand messages to Python
///
/// \param e
/// \return
messages_t get_excess_demand_functions(const excess_demand_model &e)
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
void set_excess_demand_functions(excess_demand_model &e, const list& l)
{
    std::vector<boost::shared_ptr<walras::differentiable_order_message>> result_;
    e.excess_demand_functions_.clear();
    for(int i = 0; i < len(l); ++i){
        extract<boost::shared_ptr<python_differentiable_order_message>> extractor(l[i]);
        e.excess_demand_functions_.push_back(to_std(extractor()));
    }
}

BOOST_PYTHON_MODULE(_walras)
{
    enum_<excess_demand_model::solver>("solver")
        // TODO: implement automatic differentiation for Python
        //.value("root", excess_demand_model::root)
        //.value("minimization", excess_demand_model::minimization)
        .value("derivative_free_root", excess_demand_model::derivative_free_root)
        .value("derivative_free_minimization", excess_demand_model::derivative_free_minimization)
        .export_values()
        ;

    class_< python_differentiable_order_message, boost::noncopyable>
        ("differentiable_order_message",
         init< esl::simulation::python_module::python_identity
              ,esl::simulation::python_module::python_identity
              , esl::simulation::time_point
              ,esl::simulation::time_point
              >())
        .add_property("supply", &walras::differentiable_order_message::supply)
        ;

    // expose vector of messages to Python
    class_<messages_t>("messages_t").def(vector_indexing_suite<messages_t>() );

    class_<python_excess_demand_model, boost::noncopyable>("excess_demand_model", no_init) // non-trivial constructor
        .def("__init__", make_constructor(&excess_demand_model_python_constructor))
        .def_readwrite("circuit_breaker", &excess_demand_model::circuit_breaker)
        .def_readwrite("methods", &excess_demand_model::methods)
        .def_readwrite("quotes", &excess_demand_model::quotes)
        .def("compute_clearing_quotes", &clear_market)
        .add_property("excess_demand_functions", &get_excess_demand_functions, &set_excess_demand_functions)
        ;




}

#endif
