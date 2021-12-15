#pragma once
#include <array>
#include <atomic>
#include <memory>
#include <stdexcept>

namespace ntl
{
   namespace detail
   {
      struct null_table_node_t
      {
         constexpr null_table_node_t() noexcept = default;
      } null_table_node;

      template <typename Key, typename T, typename Allocator = std::allocator<std::pair<const Key, T>>>
      class table_node
      {
      public:
         using value_type = std::pair<const Key, T>;

         constexpr table_node() noexcept :
            m_IsInUse(false)
         {
         }

         constexpr table_node(const table_node& rhs) noexcept = default;

         constexpr table_node(const std::pair<Key, T>& elem) noexcept :
            m_IsInUse(true)
         {
            std::allocator_traits<Allocator>::construct(m_Alloc,
               reinterpret_cast<value_type*>(&m_StorageArea), elem);
         }

         constexpr table_node(std::pair<Key, T>&& elem) noexcept :
            m_IsInUse(true)
         {
            std::allocator_traits<Allocator>::construct(m_Alloc,
               reinterpret_cast<value_type*>(&m_StorageArea), std::move(elem));
         }

         table_node& operator = (const table_node& rhs) noexcept
         {
            if (this != &rhs)
            {
               std::allocator_traits<Allocator>::destroy(m_Alloc, reinterpret_cast<value_type*>(&m_StorageArea));

               m_IsInUse = rhs.IsInUse;
               if (m_IsInUse)
               {
                  const value_type* rhsObj = reinterpret_cast<value_type*>(&rhs.m_StorageArea);
                  std::allocator_traits<Allocator>::construct(m_Alloc,
                     reinterpret_cast<value_type*>(&m_StorageArea), *rhsObj);
               }
            }

            return *this;
         }

         table_node& operator = (table_node&& rhs) noexcept
         {
            if (this != &rhs)
            {
               std::allocator_traits<Allocator>::destroy(m_Alloc, reinterpret_cast<value_type*>(&m_StorageArea));

               m_IsInUse = rhs.IsInUse;
               rhs.m_IsInUse = false;

               if (m_IsInUse)
               {
                  value_type* rhsObj = reinterpret_cast<value_type*>(&rhs.StorageArea);
                  std::allocator_traits<Allocator>::construct(m_Alloc,
                     reinterpret_cast<value_type*>(&StorageArea), std::move(*rhsObj));
               }
            }
         }

         table_node& operator = (const value_type& rhs) noexcept
         {
            std::allocator_traits<Allocator>::destroy(m_Alloc, reinterpret_cast<value_type*>(&m_StorageArea));

            m_IsInUse = true;
            std::allocator_traits<Allocator>::construct(m_Alloc,
               reinterpret_cast<value_type*>(&m_StorageArea), rhs);

            return *this;
         }

         table_node& operator = (value_type&& rhs) noexcept
         {
            if (m_IsInUse)
            {
               std::allocator_traits<Allocator>::destroy(m_Alloc, reinterpret_cast<value_type*>(&m_StorageArea));
            }

            m_IsInUse = true;
            std::allocator_traits<Allocator>::construct(m_Alloc,
               reinterpret_cast<value_type*>(&m_StorageArea), std::move(rhs));
         }

         table_node& operator = (const null_table_node_t& rhs) noexcept
         {
            if (m_IsInUse)
            {
               std::allocator_traits<Allocator>::destroy(m_Alloc, reinterpret_cast<value_type*>(&m_StorageArea));
            }

            m_IsInUse = false;
         }

         ~table_node()
         {
            if (m_IsInUse)
            {
               std::allocator_traits<Allocator>::destroy(m_Alloc, reinterpret_cast<value_type*>(&m_StorageArea));
            }
            m_IsInUse = false;
         }

         bool in_use() const noexcept
         {
            return m_IsInUse;
         }

         value_type& data() noexcept
         {
            return *reinterpret_cast<value_type*>(&m_Storage);
         }

         const value_type& data() const noexcept
         {
            return *reinterpret_cast<value_type*>(&m_Storage);
         }

      private:
         Allocator m_Alloc;

         std::atomic<bool> m_IsInUse;
         std::aligned_storage_t<std::pair<Key, T>> m_Storage;
      };

   }

   template <typename Key,
             typename T,
             std::size_t Capacity,
             typename Hash = std::hash<const Key>,
             typename KeyEqual = std::equal_to<Key>,
             typename Allocator = std::allocator<std::pair<const Key, T>>
            >
   class bounded_hash_map
   {
   public:
      using key_type = Key;
      using mapped_type = T;
      using value_type = std::pair<const Key, T>;
      using stored_type = detail::table_node<Key, T, Allocator>;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
      using hasher = Hash;
      using key_equal = KeyEqual;
      using allocator_type = Allocator;
      using reference = value_type&;
      using const_reference = const value_type&;
      using pointer = typename std::allocator_traits<Allocator>::pointer;
      using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

      class iterator
      {
      public:
         constexpr iterator() noexcept:
            m_Begin(nullptr),
            m_Last(nullptr),
            m_Curr(nullptr)
         {
         }

         constexpr iterator(const stored_type* begin,
                            const stored_type* last,
                            stored_type* elem) noexcept:
            m_Begin(begin),
            m_Last(last),
            m_Curr(elem)
         {
         }

         constexpr iterator(const iterator& begin,
                            const iterator& last,
                            stored_type* elem) noexcept:
            m_Begin(begin.m_Curr),
            m_Last(last.m_Curr),
            m_Curr(elem)
         {
         }

         constexpr iterator(const iterator& rhs) noexcept = default;
         iterator& operator = (const iterator& rhs) noexcept = default;

         constexpr iterator(iterator& rhs) noexcept = default;
         iterator& operator = (iterator&& rhs) noexcept = default;

         iterator& operator ++() noexcept
         {
            ++m_Curr;
            while (m_Curr != m_Last && !m_Curr->in_use())
            {
               ++m_Curr;
            }

            return *this;
         }

         iterator operator ++(int) noexcept
         {
            iterator tmp(*this);
            ++m_Curr;
            while (m_Curr != m_Last && !m_Curr->in_use())
            {
               ++m_Curr;
            }

            return tmp;
         }

         iterator& operator --() noexcept
         {
            --m_Curr;
            while (m_Curr != m_Begin && !m_Curr->in_use())
            {
              --m_Curr;
            }

            return *this;
         }

         iterator operator --(int) noexcept
         {
            iterator tmp(*this);
            --m_Curr;
            while (m_Curr != m_Begin && !m_Curr->in_use())
            {
               --m_Curr;
            }

            return tmp;
         }

         bool operator ==(const iterator& rhs) const noexcept
         {
            return m_Curr == rhs.m_Curr;
         }

         bool operator != (const iterator& rhs) const noexcept
         {
            return !(*this == rhs);
         }

         const_reference operator ->() const noexcept
         {
            return m_Curr->data();
         }

         reference operator ->() noexcept
         {
            return m_Curr->data();
         }

         const_pointer operator *() const noexcept
         {
            return &(m_Curr->data());
         }

         pointer operator *() noexcept
         {
            return &(m_Curr->data());
         }

      private:
         friend class const_iterator;

         const stored_type* m_Begin;
         const stored_type* m_Last;
         stored_type* m_Curr;
      };
      
      class const_iterator
      {
      public:
         constexpr const_iterator() noexcept:
            m_Begin(nullptr),
            m_Last(nullptr),
            m_Curr(nullptr)
         {
         }

         constexpr const_iterator(const stored_type* begin,
                                  const stored_type* last,
                                  const stored_type* elem) noexcept:
            m_Begin(begin),
            m_Last(last),
            m_Curr(elem)
         {
         }

         constexpr const_iterator(const iterator& begin,
                                  const iterator& last,
                                  stored_type* elem) noexcept:
            m_Begin(begin.m_Curr),
            m_Last(last.m_Curr),
            m_Curr(elem)
         {
         }

         constexpr const_iterator(const const_iterator& begin,
                                  const const_iterator& last,
                                  stored_type* elem) noexcept:
            m_Begin(begin.m_Curr),
            m_Last(last.m_Curr),
            m_Curr(elem)
         {
         }

         constexpr const_iterator(const const_iterator& rhs) noexcept = default;
         const_iterator& operator = (const const_iterator& rhs) noexcept = default;

         constexpr const_iterator(const_iterator& rhs) noexcept = default;
         const_iterator& operator = (const_iterator&& rhs) noexcept = default;

         constexpr const_iterator(const iterator& rhs) noexcept :
            m_Begin(rhs.m_Begin),
            m_Last(rhs.m_Last),
            m_Curr(rhs.m_Curr)
         {
         }

         const_iterator& operator = (const iterator& rhs) noexcept
         {
            m_Begin = rhs.m_Begin;
            m_Last = rhs.m_Last;
            m_Curr = rhs.m_Curr;
            return *this;
         }

         constexpr const_iterator(iterator&& rhs) noexcept :
            m_Begin(std::move(rhs.m_Begin)),
            m_Last(std::move(rhs.m_Last)),
            m_Curr(std::move(rhs.m_Curr))
         {
         }

         const_iterator& operator = (iterator&& rhs) noexcept
         {
            m_Begin = std::move(rhs.m_Begin);
            m_Last = std::move(rhs.m_Last);
            m_Curr = std::move(rhs.m_Curr);
            return *this;
         }



         const_iterator& operator ++() noexcept
         {
            ++m_Curr;
            while (m_Curr != m_Last && !m_Curr->in_use())
            {
               ++m_Curr;
            }

            return *this;
         }

         const_iterator operator ++(int) noexcept
         {
            const_iterator tmp(*this);
            ++m_Curr;
            while (m_Curr != m_Last && !m_Curr->in_use())
            {
               ++m_Curr;
            }

            return tmp;
         }

         const_iterator& operator --() noexcept
         {
            --m_Curr;
            while (m_Curr != m_Begin && !m_Curr->in_use())
            {
               --m_Curr;
            }

            return *this;
         }

         const_iterator operator --(int) noexcept
         {
            const_iterator tmp(*this);
            --m_Curr;
            while (m_Curr != m_Begin && !m_Curr->in_use())
            {
               --m_Curr;
            }

            return tmp;
         }

         bool operator ==(const const_iterator& rhs) const noexcept
         {
            return m_Curr == rhs.m_Curr;
         }

         bool operator != (const const_iterator& rhs) const noexcept
         {
            return !(*this == rhs);
         }

         bool operator ==(const iterator& rhs) const noexcept
         {
            return m_Curr == rhs.m_Curr;
         }

         bool operator != (const iterator& rhs) const noexcept
         {
            return !(*this == rhs);
         }

         const_reference operator ->() const noexcept
         {
            return m_Curr->data();
         }

         const_pointer operator *() const noexcept
         {
            return &(m_Curr->data());
         }

      private:
         const stored_type* m_Begin;
         const stored_type* m_Last;
         const stored_type* m_Curr;
      };

      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      constexpr bounded_hash_map() noexcept:
         m_Begin(&m_DataArray[Capacity], &m_DataArray[Capacity], &m_DataArray[Capacity])
         m_End(&m_DataArray[Capacity], &m_DataArray[Capacity], &m_DataArray[Capacity])
      {
      }

      iterator begin() noexcept
      {
         return m_Begin;
      }

      const_iterator begin() const noexcept
      {
         return cbegin();
      }

      const_iterator cbegin() const noexcept
      {
         return m_Begin;
      }

      iterator end() noexcept
      {
         return m_End;
      }

      const_iterator end() const noexcept
      {
         return cend();
      }

      const_iterator cend() const noexcept
      {
         return m_End;
      }

      bool empty() const noexcept
      {
         return begin() == end();
      }

      std::pair<iterator, bool> insert(const value_type& value)
      {
         size_type idx = compute_index(value.first);
         while (m_DataArray[idx].in_use() && idx < Capacity)
         {
            ++idx;
         }
         std::pair<iterator, bool> ret;

         if (idx >= Capacity)
         {
            ret = std::make_pair(m_End, false);
         }
         else
         {
            m_DataArray[idx] = value;
            ret = std::make_pair(&m_DataArray[idx], true);
         }

         return ret;
      }

      std::pair<iterator, bool> insert(value_type&& value)
      {
         size_type idx = compute_index(value.first);
         while (m_DataArray[idx].in_use() && idx < Capacity)
         {
            ++idx;
         }
         std::pair<iterator, bool> ret;

         if (idx >= Capacity)
         {
            ret = std::make_pair(m_End, false);
         }
         else
         {
            m_DataArray[idx] = std::move(value);
            ret = std::make_pair(&m_DataArray[idx], true);
         }

         return ret;
      }

      iterator find(const Key& key)
      {
         size_type idx = compute_index(key);

         key_equal ke{};
         while (!m_DataArray[idx].in_use() && !ke(m_DataArray[idx].data.first) && idx < Capacity)
         {
            ++idx;
         }

         iterator ret_itr;
         if (idx < Capacity)
         {
            ret_itr = iterator(m_Begin, m_End, &m_DataArray[idx])
         }
         else
         {
            ret_itr = iterator(m_Begin, m_End, &m_DataArray[Capacity])
         }

         return ret_itr;
      }

      const_iterator find(const Key& key) const
      {
         size_type idx = compute_index(key);

         key_equal ke{};
         while (!m_DataArray[idx].in_use() && !ke(m_DataArray[idx].data.first) && idx < Capacity)
         {
            ++idx;
         }

         const_iterator ret_itr;
         if (idx < Capacity)
         {
            ret_itr = const_iterator(m_Begin, m_End, &m_DataArray[idx])
         }
         else
         {
            ret_itr = const_iterator(m_Begin, m_End, &m_DataArray[Capacity])
         }

         return ret_itr;
      }

      iterator erase(const_iterator pos)
      {
      }

   private:
      constexpr static bool is_power_of_two(size_type capacity)
      {
         static_assert(capacity > 0, "Capacity cannot be zero.");
         return (capacity & (capacity - 1)) == 0;
      }

      static size_type compute_index(const Key& key)
      {
         hasher hf{};
         auto hash_result = hf(key);
         if constexpr (is_power_of_two(Capacity))
         {
            hash_result &= (Capacity - 1);
         }
         else
         {
            hash_result = hash_result % Capacity;
         }

         return hash_result;
      }

      std::array<stored_type, Capacity> m_DataArray;

      iterator m_Begin;
      iterator m_End;

   };
}