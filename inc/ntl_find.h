// Copyright(c) 2021 John Lewis
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
#include <algorithm>
#include <type_traits>
#include <utility>

// this is primarily a wrapper for find()'ing values in containers in some "interesting" coding standards, which
// prohibit developers from using function calls in control statements (i.e. prohibiting
// checking equality against end() in an if-statement. yes, they are extremely dumb, but these
// standards do exist. these wrappers allow for more succinct checking and less room
// for error.

namespace ntl
{
   namespace detail
   {
      // for older versions of GCC
      template <typename...>
      struct void_t_workaround
      {
         using type = void;
      };

      template <typename ... Ts>
      using void_t = typename void_t_workaround<Ts...>::type;

      template <typename T, typename U = void>
      struct is_maplike_impl : std::false_type {};

      template <typename T>
      struct is_maplike_impl<T, detail::void_t<typename T::key_type,
         typename T::mapped_type, decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])>> : std::true_type {};

   }

   template <typename T>
   struct is_maplike_t : detail::is_maplike_impl<T>::type {};

   template <typename IterTy, typename ConditionTy>
   std::pair<bool, IterTy> find_if(IterTy begin, IterTy end, ConditionTy cond)
   {
      auto itr = std::find_if(begin, end, cond);
      return std::make_pair(itr != end, itr);
   }

   template <typename IterTy>
   std::pair<bool, IterTy> find(IterTy begin, IterTy end, const typename IterTy::reference elem)
   {
      auto itr = std::find(begin, end, elem);
      return std::make_pair(itr != end, itr);
   }

   template <typename ContainerTy, typename = std::enable_if_t<is_maplike_t<ContainerTy>{}>>
   std::pair<bool, typename ContainerTy::iterator> find(ContainerTy& container, const typename ContainerTy::key_type& key)
   {
      auto itr = container.find(key);
      return std::make_pair(itr != container.end(), itr);
   }

   template <typename ContainerTy, typename = std::enable_if_t<is_maplike_t<ContainerTy>{}>>
      std::pair<bool, typename ContainerTy::const_iterator> find(const ContainerTy& container, const typename ContainerTy::key_type& key)
   {
      auto itr = container.find(key);
      return std::make_pair(itr != container.end(), itr);
   }

   template <typename ContainerTy, typename = std::enable_if_t<!is_maplike_t<ContainerTy>{}>>
      std::pair<bool, typename ContainerTy::const_iterator> find(const ContainerTy& container, const typename ContainerTy::value_type& needle)
   {
      return ntl::find(std::begin(container), std::end(container), needle);
   }
}