#pragma once
#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <vector>

namespace ntl
{
   template <typename KeyType, typename T, std::size_t MaxNumBuckets, typename HashFunc = std::hash<KeyType>, typename KeyEqual = std::equal_to<KeyType>, typename Allocator = std::allocator<std::pair<const KeyType, T>>>
   class hash_map
   {
   public:
      using key_type = KeyType;
      using mapped_type = T;
      using value_type = std::pair<const key_type, mapped_type>;

   private:
      using bucket_type = std::vector<value_type>;
      using table_type = std::array<bucket_type, MaxNumBuckets>;

   public:
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
      using hasher = HashFunc;
      using key_equal = KeyEqual;
      using allocator_type = Allocator;
      using reference = value_type&;
      using const_reference = const value_type&;
      using pointer = typename std::allocator_traits<Allocator>::pointer;
      using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

      class iterator
      {
      public:
         iterator(table_type& parent) :
            m_ParentTable(parent)
            m_UnderlyingItr(parent[0].begin()),
            m_BaseIdx(0)
         {
         }

         iterator(table_type& parent, size_type idx, typename table_type::iterator itr) :
            m_ParentTable(parent)
            m_UnderlyingItr(itr),
            m_BaseIdx(idx)
         {
         }

         iterator(const iterator& rhs) = default;
         iterator(iterator&& rhs) = default;

         reference operator *()
         {
            return *m_UnderlyingItr;
         }

         pointer operator ->()
         {
            return m_UnderlyingItr;
         }

         const_reference operator *() const
         {
            return *m_UnderlyingItr;
         }

         const_pointer operator ->() const
         {
            return m_UnderlyingItr;
         }

         template <typename Other>
         bool operator == (const Other& rhs) const
         {
            return m_UnderlyingItr == rhs.m_UnderlyingItr;
         }

         template <typename Other>
         bool operator != (const Other& rhs) const
         {
            return !(*this == rhs);
         }

         iterator& operator ++()
         {
            ++m_UnderlyingItr;
            if (m_UnderlyingItr == m_ParentTable[m_BaseIdx].end())
            {
               ++m_BaseIdx;
               m_UnderlyingItr = m_ParentTable[m_BaseIdx].begin();
            }

            return *this;
         }

         iterator operator++(int unused)
         {
            iterator i = *this;

            ++m_UnderlyingItr;
            if (m_UnderlyingItr == m_ParentTable[m_BaseIdx].end())
            {
               ++m_BaseIdx;
               m_UnderlyingItr = m_ParentTable[m_BaseIdx].begin();
            }

            return i;
         }

      private:
         table_type&                   m_ParentTable;
         typename table_type::iterator m_UnderlyingItr;
         size_type                     m_BaseIdx;
      };

      class const_iterator
      {
      public:
         const_iterator(table_type& parent) :
            m_ParentTable(parent)
            m_UnderlyingItr(parent[0].begin()),
            m_BaseIdx(0)
         {
         }

         const_iterator(table_type& parent, size_type idx, typename table_type::const_iterator itr) :
            m_ParentTable(parent)
            m_UnderlyingItr(itr),
            m_BaseIdx(idx)
         {
         }

         const_iterator(const const_iterator& rhs) = default;
         const_iterator(const_iterator&& rhs) = default;

         const_iterator(const iterator& rhs) :
            m_ParentTable(rhs.m_ParentTable),
            m_UnderlyingItr(rhs.m_UnderlyingItr),
            m_BaseIdx(rhs.m_BaseIdx)
         {
         }

         const_iterator(iterator&& rhs):
            m_ParentTable(std::move(rhs.m_ParentTable)),
            m_UnderlyingItr(std::move(rhs.m_UnderlyingItr)),
            m_BaseIdx(std::move(rhs.m_BaseIdx))
         {
         }

         const_reference operator *() const
         {
            return *m_UnderlyingItr;
         }

         const_pointer operator ->() const
         {
            return m_UnderlyingItr;
         }

         template <typename Other>
         bool operator == (const Other& rhs) const
         {
            return m_UnderlyingItr == rhs.m_UnderlyingItr;
         }

         template <typename Other>
         bool operator != (const Other& rhs) const
         {
            return !(*this == rhs);
         }

         const_iterator& operator ++()
         {
            ++m_UnderlyingItr;
            if (m_UnderlyingItr == m_ParentTable[m_BaseIdx].end())
            {
               ++m_BaseIdx;
               m_UnderlyingItr = m_ParentTable[m_BaseIdx].begin();
            }

            return *this;
         }

         const_iterator operator++(int unused)
         {
            const_iterator i = *this;

            ++m_UnderlyingItr;
            if (m_UnderlyingItr == m_ParentTable[m_BaseIdx].end())
            {
               ++m_BaseIdx;
               m_UnderlyingItr = m_ParentTable[m_BaseIdx].begin();
            }

            return i;
         }

      private:
         const table_type&                   m_ParentTable;
         typename table_type::const_iterator m_UnderlyingItr;
         size_type                           m_BaseIdx;
      };

      using local_iterator = typename table_type::iterator;
      using const_local_iterator = typename table_type::const_iterator;

      hash_map() = default;
      hash_map(const hash_map& rhs) = default;
      hash_map(hash_map&& rhs) = default;
      hash_map& operator =(const hash_map& rhs) = default;
      hash_map& operator = (hash_map&& rhs) = default;

      iterator begin()
      {
         return iterator(m_Table);
      }

      const_iterator begin() const
      {
         return const_iterator(m_Table);
      }

      iterator end()
      {
         constexpr size_type lastIdx = m_Table.max_size() - 1;
         return iterator(m_Table, lastIdx, m_Table[lastIdx].end());
      }

      const_iterator end()
      {
         constexpr size_type lastIdx = m_Table.max_size() - 1;
         return const_iterator(m_Table, lastIdx, m_Table[lastIdx].end());
      }

      iterator find(const key_type& key)
      {
         auto tableIdx = get_index(key);
         iterator ret_itr = end();
         assert(tableIdx < m_Table.max_size());

         bucket_type& refVal = m_Table[tableIdx];
         if (!refVal.empty())
         {
            key_equal ke{};
            if (ke(refVal.front().first, key))
            {
               ret_itr = iterator(m_Table, tableIdx, refVal.begin());
            }
            else
            {
               // go ahead and increment past begin,
               // since we tested for equality to front element
               // above
               const_local_iterator bucket_itr = refVal.begin();
               ++bucket_itr;

               while (!ke(bucket_itr->first, key) && bucket_itr != refVal.end())
               {
                  ++bucket_itr;
               }

               if (bucket_itr == refVal.end())
               {
                  ret_itr = end();
               }
               else
               {
                  ret_itr = iterator(m_Table, tableIdx, bucket_itr);
               }
            }

         }

         return itr;
      }

      const_iterator find(const key_type& key) const
      {
         auto tableIdx = get_index(key);
         const_iterator ret_itr = end();
         assert(tableIdx < m_Table.max_size());

         bucket_type& refVal = m_Table[tableIdx];
         if (!refVal.empty())
         {
            key_equal ke{};
            if (ke(refVal.front().first, key))
            {
               ret_itr = const_iterator(m_Table, tableIdx, refVal.begin());
            }
            else
            {
               // go ahead and increment past begin,
               // since we tested for equality to front element
               // above
               const_local_iterator bucket_itr = refVal.begin();
               ++bucket_itr;

               while (!ke(bucket_itr->first, key) && bucket_itr != refVal.end())
               {
                  ++bucket_itr;
               }

               if (bucket_itr == refVal.end())
               {
                  ret_itr = end();
               }
               else
               {
                  ret_itr = iterator(m_Table, tableIdx, bucket_itr);
               }
            }

         }

         return itr;
      }

      void insert(const value_type& value) noexcept
      {
         const std::size_t idx = get_index(value.first);
         m_Table[idx].emplace_back(value);
      }

      void insert(value_type&& value) noexcept
      {
         const std::size_t idx = get_index(value.first);
         m_Table[idx].emplace_back(std::forward(value));
      }

   private:

      static std::size_t get_index(const key_type& key) noexcept
      {
         hasher fcn{};
         return fcn(key) & (MaxNumBuckets - 1);
      }

      table_type m_Table;

   };
}