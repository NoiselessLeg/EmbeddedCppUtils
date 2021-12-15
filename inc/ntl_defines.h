// Copyright(c) 2021 John Lewis, and Moodycamel
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#ifdef __cpp_if_constexpr
#define NTL_FEATURE_HAS_CONSTEXPR_IF
#endif

#if defined(_MSC_VER)
#define NTL_WIND
#if defined(_WIN32) || defined(WIN32)
#define NTL_WIND_32
#endif
#elif defined (__GNUC__)
#define NTL_LINUX
#endif


// Excerpted from Cameron Desrochers's ConcurrentQueue; as a nice
// way to check if exceptions are disabled.
#if (defined(_MSC_VER) && defined(_CPPUNWIND)) || (defined(__GNUC__) && defined(__EXCEPTIONS)) || (!defined(_MSC_VER) && !defined(__GNUC__))
#define NTL_HAS_EXCEPTIONS
#define NTL_THROW_NOARG(ex_type) throw ex_type
#define NTL_THROW(ex_type, ...) throw ex_type(__VA_ARGS__)
#define NTL_TRY try
#define NTL_CATCH(ex_spec) catch (ex_spec)
#else
#include <cstdio>
#define NTL_THROW_NOARG(ex_type) std::abort()
#define NTL_THROW(ex, ...) printf(__VA_ARGS__#'\n'); std::abort()
#define NTL_CATCH(ex_spec) else

#ifdef NTL_FEATURE_HAS_CONSTEXPR_IF
#define NTL_TRY if constexpr(true)
#else
#define NTL_TRY if (true)
#endif

#endif

#if __has_include(<optional>)
#define NTL_FEATURE_HAS_OPTIONAL
#endif

#ifdef NTL_FEATURE_HAS_CONSTEXPR_IF
#define NTL_CONSTEXPR_IF if constexpr
#define NTL_CONSTEXPR_ELSEIF(condition) else if constexpr (condition)
#else
#define NTL_CONSTEXPR_IF if
#define NTL_CONSTEXPR_ELSEIF(condition) else if (condition)
#endif


//TODO: need feature macro here
//#define NTL_FEATURE_HAS_INDEX_SEQUENCE

#if __has_cpp_attribute(nodiscard)
#define NTL_FEATURE_HAS_NODISCARD
#define NTL_NODISCARD [[nodiscard]]
#define NTL_NODISCARD_TEXT(reason) [[nodiscard(reason)]]
#else
#define NTL_NODISCARD
#define NTL_NODISCARD_TEXT(reason)
#endif

#ifdef __cpp_lib_semaphore
#define NTL_FEATURE_HAS_SEMAPHORE
#endif

#define NTL_DEBUG_ENABLED

#ifdef NTL_DEBUG_ENABLED
#include <iostream>
#define NTL_DEBUG_PRINTF(...) std::cout << std::format(__VA_ARGS__)
#else
#define NTL_DEBUG_PRINTF(...)
#endif

#ifndef MAX_NUM_NTL_TASKS
#define MAX_NUM_NTL_TASKS 128
#endif

#ifndef MAX_NUM_NTL_TASK_HANDLERS
#include <thread>
#define MAX_NUM_NTL_TASK_HANDLERS 8
#endif