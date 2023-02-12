/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2022 Andrey Semashev
 */
/*!
 * \file scope/scope_final.hpp
 *
 * This header contains definition of \c scope_final template.
 */

#ifndef BOOST_SCOPE_SCOPE_FINAL_HPP_INCLUDED_
#define BOOST_SCOPE_SCOPE_FINAL_HPP_INCLUDED_

#include <type_traits>
#include <boost/config.hpp>
#include <boost/scope/detail/is_not_like.hpp>
#include <boost/scope/detail/move_or_copy_construct_ref.hpp>
#include <boost/scope/detail/type_traits/conjunction.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {
namespace scope {

/*!
 * \brief Scope final guard that invokes a function upon leaving the scope.
 *
 * A scope exit guard wraps a function object callable with no arguments
 * that can be one of:
 *
 * \li A user-defined class with a public <tt>operator()</tt>.
 * \li An lvalue reference to such class.
 * \li An lvalue reference to function taking no arguments.
 *
 * The scope final guard unconditionally invokes the wrapped function object
 * on destruction.
 */
template< typename Func >
class scope_final
{
private:
    Func m_func;

public:
    //! Constructs a scope final guard with a given callable function object.
    template<
        typename F,
        typename = typename std::enable_if< detail::conjunction<
            std::is_constructible< Func, typename detail::move_or_copy_construct_ref< F, Func >::type >,
            detail::is_not_like< Func, scope_final >
        >::value >::type
    >
    scope_final(F&& func) noexcept(std::is_nothrow_constructible< Func, typename detail::move_or_copy_construct_ref< F, Func >::type >::value) try :
        m_func(static_cast< typename detail::move_or_copy_construct_ref< F, Func >::type >(func))
    {
    }
    catch (...)
    {
        func();
    }

    scope_final(scope_final const&) = delete;
    scope_final& operator= (scope_final const&) = delete;

    //! Invokes the wrapped callable function object and destroys the callable.
    ~scope_final() noexcept(noexcept(std::declval< Func& >()()))
    {
        m_func();
    }
};

#if !defined(BOOST_NO_CXX17_DEDUCTION_GUIDES)
template< typename Func >
scope_final(Func) -> scope_final< Func >;
#endif // !defined(BOOST_NO_CXX17_DEDUCTION_GUIDES)

} // namespace scope

using scope::scope_final;

#if defined(BOOST_MSVC)
#define BOOST_SCOPE_DETAIL_UNIQUE_VAR_TAG __COUNTER__
#else
#define BOOST_SCOPE_DETAIL_UNIQUE_VAR_TAG __LINE__
#endif

/*!
 * \brief The macro creates a uniquely named final scope exit guard.
 *
 * The macro should be followed by a function object that should be called
 * on leaving the current scope. Usage example:
 *
 * <code>
 * BOOST_SCOPE_FINAL []
 * {
 *     std::cout << "Hello world!" << std::endl;
 * };
 * </code>
 *
 * \note Using this macro requires C++17.
 */
#define BOOST_SCOPE_FINAL \
    boost::scope::scope_final BOOST_JOIN(_boost_scope_final_, BOOST_SCOPE_DETAIL_UNIQUE_VAR_TAG) =

} // namespace boost

#endif // BOOST_SCOPE_SCOPE_FINAL_HPP_INCLUDED_
