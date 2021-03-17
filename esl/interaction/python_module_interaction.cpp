/// \file   python_module_interaction.cpp
///
/// \brief
///
/// \authors    maarten
/// \date       2020-11-27
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
#include <esl/interaction/python_module_interaction.hpp>
#include <esl/interaction/transfer.hpp>
using namespace esl;
using namespace esl::simulation;
using namespace esl::interaction;


#include <boost/serialization/export.hpp>

#ifdef WITH_PYTHON
#include <string>

# include <boost/python/suite/indexing/indexing_suite.hpp>
# include <boost/python/suite/indexing/vector_indexing_suite.hpp>
# include <boost/python/iterator.hpp>
# include <boost/python/call_method.hpp>
# include <boost/python/tuple.hpp>


BOOST_CLASS_EXPORT(esl::interaction::python_message);


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
        //container[i] = v;
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





void send_message_python(communicator &c, boost::python::object o)
{
    extract<const communicator::message_t &> asdf(o);

}

//communicator::callback_handle make_handle()
//{
//    return [&](communicator::message_t,
//              simulation::time_interval,
//              std::seed_seq &) {
//        };
//}

//
//communicator::callback_t make_callback(boost::python::object &o)
//{
//    return communicator::callback_t
//    { .function = [] (communicator::message_t,
//                        simulation::time_interval,
//                        std::seed_seq &)
//         {
//             return simulation::time_point(123);
//         }
//    , .description = "asdf"
//    , .file = "asdf.f"
//    , .line = 123
//    , .message = "asdfasdf"
//    }    ;
//}


BOOST_PYTHON_MODULE(_interaction)
{
    def("make_callback_handle", &make_callback_handle);

    class_<communicator::callback_handle>( "callback_handle");

    class_<communicator::callback_t>( "callback_t")
        .def_readwrite("function", &communicator::callback_t::function)
        .def_readwrite("description", &communicator::callback_t::description)
        .def_readwrite("message", &communicator::callback_t::message)
        .def_readwrite("file", &communicator::callback_t::file)
        .def_readwrite("line", &communicator::callback_t::line)
        //.def("__call__", &communicator::callback_t::operator ())
        ;

    enum_<communicator::scheduling>("scheduling")
        .value("in_order", communicator::scheduling::in_order)
        .value("random", communicator::scheduling::random)
        ;

    class_<communicator::inbox_t>("inbox_t")
        .def(multimap_indexing_suite<communicator::inbox_t>());


    class_<communicator::outbox_t>("outbox_t")
        .def(boost::python::vector_indexing_suite<communicator::outbox_t>());

    class_<communicator>("communicator")
        .def("send_message", send_message_python)
        .def_readwrite("inbox", &communicator::inbox)
        .def_readwrite("outbox", &communicator::outbox)
        ;

    class_<header>("header", init<message_code, identity<agent>, identity<agent>, simulation::time_point, simulation::time_point>())
        .def(init<message_code, identity<agent>, identity<agent>, simulation::time_point>())
        .def(init<message_code, identity<agent>, identity<agent>>())
        .def(init<message_code, identity<agent>>())
        .def(init<message_code>())
        .def(init<message_code>())
        .def_readwrite("type", &header::type)
        .def_readwrite("sender", &header::sender)
        .def_readwrite("recipient", &header::recipient)
        .def_readwrite("sent", &header::sent)
        .def_readwrite("received", &header::received)
    ;

    class_<python_message, bases<header>>("message")
        .def_readonly("code", &python_message::python_code)
    ;
}

#endif