/*
    mclib (Multi-Canvas Library)
    Copyright (C) 2021-2022  Yukino Amamiya
  
    This file is part of the mclib Library. This library is
    a graphics library for desktop applications only and it's
    only for windows.

    This library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General
    Public License as published by the Free Software Foundation;
    either version 2.1 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the GNU Lesser General Public License for
    more details.

    You should have received a copy of the GNU Lesser General
    Public License along with this library; if not, write to
    the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
    
    Yukino Amamiya
    iamyukino[at outlook.com]
    
    @file src/pyobj.h
    This is a C++11 header.
*/


#ifndef MCL_PYOBJ
# define MCL_PYOBJ

# include <cstdlib>
# include <tuple>
# include <type_traits>
# include <typeinfo>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4365)
#endif

# include <ostream>
# include <sstream>

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {
    
   /**
    * @class pytuple
    * @brief simplified python tuple
    * 
    * @code
    *   auto t1 = mcl::maktuple (1, 2, "Hello");
    *   auto t2 = mcl::maktuple (0.2, 99, "World");
    *   auto t3 = mcl::maktuple (t1);
    *   std::cout << t1 << '\n';               // (1, 2, Hello)
    *   std::cout << t3 << '\n';               // ((1, 2, Hello),)
    *   std::cout << mcl::cmp(t2, t1) << '\n'; // -1
    *   std::string str = t1[-1];
    *   std::cout << str << '\n';              // Hello
    *   std::cout << t2[2] << '\n';            // World
    *   std::cout << mcl::len(t2) << '\n';     // 3
    *   for (auto i: t2)
    *       std::cout << i << '\n';            // 0.2\n99\nWorld
    * @endcode
    *
    * @ingroup py-style
    * @ingroup mclib
    * @{
    */
    
    // Runtime subscript access tuples
    constexpr int // Recursive termination condition
    mcl_tuple_visitor (size_t, std::tuple<> const&, void*) { return 0; }
    
    template <typename lhs_t, typename rhs_t>
    [[noreturn]] constexpr typename std::enable_if<!std::is_convertible<rhs_t, lhs_t>::value, void>::type
    mcl_copy (lhs_t* , rhs_t* ) // The specified subscript element type cannot be
    { throw std::bad_cast (); } // implicitly converted to the target type
    
    template <typename lhs_t, typename rhs_t>
    constexpr typename std::enable_if<std::is_convertible<rhs_t, lhs_t>::value, void>::type
    mcl_copy (lhs_t* lhs, rhs_t* rhs) noexcept
    { const_cast<typename std::remove_const<decltype(*lhs)>::type>(*lhs) = *rhs; }
    
    template <typename visit_t, typename Tv, typename... Ts> void 
    mcl_tuple_visitor (size_t index, std::tuple<Tv, Ts...> const& t, visit_t* ptr) {
        if (index >= (1 + sizeof...(Ts))) 
            throw std::invalid_argument ("Bad Index");
        else if (index > 0)
            mcl_tuple_visitor (index - 1, reinterpret_cast<std::tuple<Ts...> const&>(t), ptr);
        else mcl_copy (ptr, &std::get<0>(t));
    }
    
    
    // Outputs specified tuple elements at run time
    struct mcl_do_is_printable {
        template<typename lhs_t, typename rhs_t, 
          typename = decltype(*static_cast<lhs_t*>(0) << *static_cast<rhs_t*>(0))>
            static std::integral_constant<bool, true> test(int); // only declaration
        template<typename, typename>
            static std::integral_constant<bool, false> test(...);
    };
    
    template<typename lhs_t, typename rhs_t>
    struct mcl_is_printable
    : public mcl_do_is_printable {
        typedef decltype(test<lhs_t, rhs_t>(0)) type;
    }; // Judge whether the target type can be output
    
    template <typename lhs_t, typename rhs_t>
    constexpr typename std::enable_if<mcl_is_printable<lhs_t, rhs_t>::type::value, void>::type
    mcl_oss_print (lhs_t* lhs, rhs_t* rhs) noexcept
    { *lhs << *rhs; }
    
    template <typename lhs_t, typename rhs_t>
    constexpr typename std::enable_if<!mcl_is_printable<lhs_t, rhs_t>::type::value, void>::type
    mcl_oss_print (lhs_t* , rhs_t* ) 
    { throw std::ios_base::failure ("The specified type does not overload operator<< . "
        "(with type = " + std::string (typeid(rhs_t).name()) + ")"); }
    
    constexpr int
    mcl_tuple_printer (size_t, std::tuple<> const&, void*) noexcept{ return 0; }
    
    template <typename os_t, typename Tv, typename... Ts> void 
    mcl_tuple_printer (size_t index, std::tuple<Tv, Ts...> const& t, os_t* oss) {
        if (index >= (1 + sizeof...(Ts)))
            throw std::invalid_argument ("Bad index");
        else if (index > 0)
            mcl_tuple_printer (index - 1, reinterpret_cast<std::tuple<Ts...> const&>(t), oss);
        else mcl_oss_print (oss, &std::get<0>(t));
    }
        // Use the characteristics of compile time extension to obtain the elements with
        // specified subscripts through recursion at run time
    
    
    // Compare between tuples of defferent types
    template <typename... lhs_t, typename... rhs_t>
    constexpr typename std::enable_if<!sizeof...(lhs_t) && sizeof...(rhs_t), int>::type
    cmp (std::tuple<lhs_t...> const& , std::tuple<rhs_t...> const& ) noexcept
    { return -1; } // lhs reaches the end first, then rhs is larger
    
    // Compare between tuples of defferent types
    template <typename... lhs_t, typename... rhs_t>
    constexpr typename std::enable_if<sizeof...(lhs_t) && !sizeof...(rhs_t), int>::type
    cmp (std::tuple<lhs_t...> const& , std::tuple<rhs_t...> const& ) noexcept
    { return 1; } // rhs reaches the end first, then lhs is larger
    
    // Compare between tuples of defferent types
    template <typename... lhs_t, typename... rhs_t>
    constexpr typename std::enable_if<!sizeof...(lhs_t) && !sizeof...(rhs_t), int>::type
    cmp (std::tuple<lhs_t...> const& , std::tuple<rhs_t...> const& ) noexcept
    { return 0; } // lhs and rhs reaches the end at the same time, this means two tuples
                  // are exactly equal.

    // Compare between tuples of defferent types
    template <typename lhsf, typename rhsf, typename... lhs_t, typename... rhs_t>
    constexpr typename std::enable_if<
        !(std::is_same<lhsf, rhsf>::value
            || (std::is_arithmetic<lhsf>::value && std::is_arithmetic<rhsf>::value)
        ), int>::type
    cmp (std::tuple<lhsf, lhs_t...> const&, std::tuple<rhsf, rhs_t...> const&) noexcept {
        return std::is_arithmetic<lhsf>::value ? -1
            : ( std::is_arithmetic<rhsf>::value ? 1 : (sizeof (lhsf) > sizeof (rhsf)) );
    } // The end is not reached and the first element type is different. Compare the type,
      // and the number type is smaller. Otherwise, press sizeof for comparison.
    
    // Compare between tuples of defferent types
    template <typename lhsf, typename rhsf, typename... lhs_t, typename... rhs_t>
    constexpr typename std::enable_if<
        (std::is_same<lhsf, rhsf>::value
            || (std::is_arithmetic<lhsf>::value && std::is_arithmetic<rhsf>::value)
        ), int>::type
    cmp (std::tuple<lhsf, lhs_t...> const& lhs, std::tuple<rhsf, rhs_t...> const& rhs) noexcept {
        return std::get<0>(lhs) == std::get<0>(rhs) ? 
            cmp (reinterpret_cast<std::tuple<lhs_t...> const&>(lhs),
                 reinterpret_cast<std::tuple<rhs_t...> const&>(rhs))
            : (std::get<0>(lhs) > std::get<0>(rhs)) - (std::get<0>(lhs) < std::get<0>(rhs));
    } // If the end is not reached and the first element type is the same, compare whether the
      // first element of two tuples is equal. If it is not equal, the comparison result is
      // returned, and if it is equal, the next element is compared recursively.
    
    // Get the length of tuple
    template <typename... T> 
    constexpr size_t len (std::tuple<T...> const&) noexcept 
    { return sizeof... (T); }
    
    // Class like tuple in python
    template <typename... T>
    class pytuple
    : public std::tuple<T...> {
    
    public:
        template <typename... pt>
        class proxy_t {
            friend pytuple;
            friend class pytuple::iterator;
            
            std::tuple<pt...> const& m_ptr;
            size_t index;
            constexpr proxy_t (std::tuple<pt...> const& m, size_t i) noexcept
                : m_ptr (m), index (i) { } 
            
        public:
            template<typename cv>
            inline operator cv const () const{
                cv v; // ignore Int-unini, may not have default constructor
                mcl_tuple_visitor<cv> (index, m_ptr, &v);
                return v;
            }
            friend inline std::ostream&
            operator<< (std::ostream& os, proxy_t<pt...> const& rhs) {
                mcl_tuple_printer<std::ostream> (rhs.index, rhs.m_ptr, &os);
                return os;
            }
            friend inline std::wostream&
            operator<< (std::wostream& os, proxy_t<pt...> const& rhs) {
                mcl_tuple_printer<std::wostream> (rhs.index, rhs.m_ptr, &os);
                return os;
            }
        };
        
    public:
        constexpr pytuple () = default;
        constexpr pytuple (const pytuple<T...>&) = default;
        constexpr pytuple (pytuple&&) = default;
        pytuple<T...>& operator= (pytuple<T...> const&) = default;
        pytuple<T...>& operator= (pytuple<T...>&&) = default;
        
        constexpr pytuple (std::tuple<T...> const& tp)
        : std::tuple<T...> (tp) { }
        
        constexpr pytuple (T&&... parms)
        : std::tuple<T...> (std::make_tuple (std::forward<T>(parms)...)) { }
        
        constexpr proxy_t<T...> operator[] (long long index) const noexcept
        { return proxy_t <T...>(*this, index >= 0 ? index : sizeof...(T) + index); }
        
    public:
        template <typename... rhs_t>
        constexpr bool operator< (proxy_t<rhs_t...> const& rhs) const noexcept
        { return cmp (*this, rhs) < 0; }
        
        template <typename... rhs_t>
        constexpr bool operator> (proxy_t<rhs_t...> const& rhs) const noexcept
        { return cmp (*this, rhs) > 0; }
        
        template <typename... rhs_t>
        constexpr bool operator== (proxy_t<rhs_t...> const& rhs) const noexcept
        { return cmp (*this, rhs) == 0; }
        
        template <typename... rhs_t>
        constexpr bool operator!= (proxy_t<rhs_t...> const& rhs) const noexcept
        { return cmp (*this, rhs) != 0; }
        
        template <typename... rhs_t>
        constexpr bool operator<= (proxy_t<rhs_t...> const& rhs) const noexcept
        { return cmp (*this, rhs) <= 0; }
        
        template <typename... rhs_t>
        constexpr bool operator>= (proxy_t<rhs_t...> const& rhs) const noexcept
        { return cmp (*this, rhs) >= 0; }
        
    private:
        template <typename char_type, size_t i = 1> 
        constexpr typename std::enable_if<
            (i >= sizeof...(T)), std::basic_ostream<char_type>& >::type
        str_ (std::basic_ostream<char_type>& ss) const noexcept
        { return ss << ")"; }
        
        template <typename char_type, size_t i = 1>
        constexpr typename std::enable_if<
            (i < sizeof...(T)), std::basic_ostream<char_type>& >::type
        str_ (std::basic_ostream<char_type>& ss) const noexcept
        { return ss << ", " << std::get<i>(*this), str_<char_type, i + 1>(ss); }
        
    public:
        friend constexpr std::ostream&
        operator<< (std::ostream&  os, pytuple<T...> const& tu) {
            return sizeof...(T) ? (
                os << '(' << std::get<0>(tu),
                (sizeof...(T) == 1) ? (os << ",)") : (tu.str_ (os))
            ) : (os << "()");
        }
        friend constexpr std::wostream&
        operator<< (std::wostream& os, pytuple<T...> const& tu) {
            return sizeof...(T) ? (
                os << '(' << std::get<0>(tu),
                (sizeof...(T) == 1) ? (os << L",)") : (tu.str_ (os))
            ) : (os << L"()");
        }
        
    public:
        class iterator {
            std::tuple<T...> const& m_ptr;
            size_t index;
            constexpr iterator (std::tuple<T...> const& m, size_t i) noexcept
                : m_ptr (m), index (i) { } 
            friend pytuple;
        public:
            constexpr pytuple::proxy_t<T...> operator* () noexcept
            { return pytuple::proxy_t<T...>(m_ptr, index); }
            constexpr bool operator!= (iterator const& rhs) const noexcept
            { return this->index != rhs.index; }
            inline iterator& operator++ () noexcept{ ++ index; return *this; }
        };
        constexpr iterator begin () const noexcept{ return iterator {*this, 0}; }
        constexpr iterator end   () const noexcept{ return iterator {*this, sizeof...(T)}; }
        
    public:
        template <typename... rhs_t>
        constexpr bool operator< (pytuple<rhs_t...> const& rhs) noexcept
        { return cmp (*this, rhs) < 0; }
        
        template <typename... rhs_t>
        constexpr bool operator> (pytuple<rhs_t...> const& rhs) noexcept
        { return cmp (*this, rhs) > 0; }
        
        template <typename... rhs_t>
        constexpr bool operator== (pytuple<rhs_t...> const& rhs) noexcept
        { return cmp (*this, rhs) == 0; }
        
        template <typename... rhs_t>
        constexpr bool operator!= (pytuple<rhs_t...> const& rhs) noexcept
        { return cmp (*this, rhs) != 0; }
        
        template <typename... rhs_t>
        constexpr bool operator<= (pytuple<rhs_t...> const& rhs) noexcept
        { return cmp (*this, rhs) <= 0; }
        
        template <typename... rhs_t>
        constexpr bool operator>= (pytuple<rhs_t...> const& rhs) noexcept
        { return cmp (*this, rhs) >= 0; }
    };
    
    // Construct a pytuple object.
    template <typename... Ts>
    constexpr pytuple<typename std::decay<typename std::remove_reference<Ts>::type>::type...> 
    maktuple (Ts&&... agv) noexcept {
        return std::make_tuple (std::forward<Ts>(agv)...);
    }
   /** @}  */
    
} // namespace

#endif // MCL_PYOBJ
