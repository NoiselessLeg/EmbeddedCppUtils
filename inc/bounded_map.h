#pragma once
#include <array>
#include <functional>
#include <memory>

namespace ntl
{
   namespace detail
   {
      template <typename KeyT, typename ValueT>
      struct map_node
      {
         using key_type = KeyT;
         using mapped_type = ValueT;
         using stored_type = std::pair<const KeyT, ValueT>;
         using value_type = std::aligned_storage_t<sizeof(stored_type), alignof(stored_type)>;

         map_node() :
            IsValid(false)
         {
         }

         map_node(const map_node& rhs) = default;
         map_node& operator = (const map_node& rhs) = default;

         map_node(map_node&& rhs) = default;
         map_node& operator = (map_node&& rhs) = default;

         bool IsValid;
         value_type Data;

      };
   }

   template <typename KeyT, 
             typename ValueT, 
             std::size_t Capacity,
             typename CompareT = std::less<KeyT>,
             typename Alloc = std::allocator<std::pair<const KeyT, ValueT>>
            >
   class bounded_map
   {
   public:
      using key_type = KeyT;
      using mapped_type = ValueT;
      using value_type = detail::map_node<const KeyT, ValueT>;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
      using key_compare = CompareT;
      using allocator_type = Alloc;
      using reference = value_type&;
      using const_reference = const value_type&;
      using pointer = typename std::allocator_traits<Alloc>::pointer;
      using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;

      class iterator
      {
      public:
         using value_type = typename detail::map_node<const KeyT, ValueT>;
         using reference = value_type&;
         using const_reference = const value_type&;

         constexpr iterator() noexcept:
            m_ValueValid(false),
            m_pValue(nullptr)
         {
         }

         constexpr iterator(const iterator& rhs) noexcept = default;
         constexpr iterator& operator = (const iterator& rhs) noexcept = default;

         constexpr iterator(iterator&& rhs) noexcept = default;
         constexpr iterator& operator = (iterator&& rhs) noexcept = default;

         constexpr iterator(value_type& elem) noexcept:
            m_ValueValid(elem.IsValid),
            m_pValue(&elem)
         {
         }

         reference operator *() noexcept
         {
            return *reinterpret_cast<pointer>(&(m_pValue->Data));
         }

         pointer operator ->() noexcept
         {
            return &(m_pValue->Data);
         }

         const_reference operator *() const
         {
            return *reinterpret_cast<pointer>(&(m_pValue->Data));
         }

         const_pointer operator ->() const
         {
            return &(m_pValue->Data);
         }
         

      private:
         bool        m_ValueValid;
         value_type* m_pValue;
      };

      template <typename ... Args>
      std::pair<iterator, bool> emplace(Args&& ... args)
      {
         
      }


   private:
      using storage_type = std::array<value_type, Capacity>;

      storage_type m_Storage;
      allocator_type m_Alloc;
   };
}