#pragma once
#include "ntl_defines.h"
#include "ntl_strings.h"
#include <fstream>
#include <sstream>
#include <vector>

namespace ntl
{
   template <typename ElemTy>
   class csv_cell_formatter
   {
   public:
      ElemTy operator () (std::istream& strm) const
      {
         ElemTy elem{};
         if (!(strm >> elem))
         {
            NTL_THROW(std::runtime_error, "Failed to read element in CSV");
         }
         return elem;
      }
   };

   template <>
   class csv_cell_formatter<bool>
   {
   public:
      bool operator () (std::istream& strm) const
      {
         std::string csvElem;
         strm >> csvElem;
         std::string asLower = ntl::tolower(csvElem);

         bool value;
         if (asLower == "true")
         {
            value = true;
         }
         else if (asLower == "false")
         {
            value = false;
         }
         else
         {
            std::stringstream csvElemStrm(csvElem);
            if (!(csvElemStrm >> value))
            {
               NTL_THROW(std::runtime_error, "Failed to read element in CSV");
            }
         }

         return value;
      }
   };

   template <typename ... ColumnTypes>
   class csv_row
   {
   private:
      using Underlying_T = std::tuple<ColumnTypes...>;

   public:
      csv_row(const std::string& trimmedLine)
      {
         std::istringstream strm(trimmedLine);
         read_elements(strm, ',', std::make_index_sequence<sizeof...(ColumnTypes)>{});
      }
      csv_row(const std::string& trimmedLine, char delim)
      {
         std::istringstream strm(trimmedLine);
         read_elements(strm, delim, std::make_index_sequence<sizeof...(ColumnTypes)>{});
      }

      csv_row(ColumnTypes&& ... args):
         m_RowData(std::make_tuple(std::forward<ColumnTypes>(args)...))
      {
      }

      // since i don't think auto return type is supported by GCC 4.9.2
      template <std::size_t ColumnIndex>
      const std::tuple_element_t<ColumnIndex, Underlying_T>& get() const noexcept
      {
         static_assert(ColumnIndex < std::tuple_size_v<decltype(m_RowData)>, "Column index exceeds maximum number of columns.");
         return std::get<ColumnIndex>(m_RowData);
      }

      template <std::size_t ColumnIndex>
      std::tuple_element_t<ColumnIndex, Underlying_T>& get() noexcept
      {
         static_assert(ColumnIndex < std::tuple_size_v<decltype(m_RowData)>, "Column index exceeds maximum number of columns.");
         return std::get<ColumnIndex>(m_RowData);
      }

   private:
      template <typename T>
      bool read_element(std::istream& strm, char delim, T& value)
      {
         std::string csvElem;
         if (std::getline(strm, csvElem, delim))
         {
            csv_cell_formatter<T> formatter{};
            std::stringstream csvElemStrm(csvElem);
            value = formatter(csvElemStrm);
         }

         return true;
      }

      template <std::size_t ... I>
      void read_elements(std::istream& strm, char delim, std::index_sequence<I...>)
      {
         // really funky syntax; this is needed to get a reference to each element of RowData.
         // You can't just use ... syntax willy nilly. I still am not sure what this really
         // does. I think that effectively an initializer list of booleans is created of size I
         // and each elem is initialized by read_element call to a deeper
         // recursion index of I (e.g. I-2, I-3). i'm still not sure how the compiler
         // knows how to deduce that from the context.
         std::initializer_list<bool> { read_element(strm, delim, std::get<I>(m_RowData))... };
      }

      Underlying_T m_RowData;

   };

   template <typename ... ColumnTypes>
   csv_row<ColumnTypes...> make_csv_row(ColumnTypes&& ... args)
   {
      return csv_row(std::forward<ColumnTypes>(args)...);
   }

   template <typename ... ColumnTypes>
   class csv_parser
   {
   public:
      using row_type = csv_row<ColumnTypes...>;
      using underlying_container = std::vector<row_type>;
      using iterator = typename underlying_container::iterator;
      using const_iterator = typename underlying_container::const_iterator;
      using reverse_iterator = typename underlying_container::reverse_iterator;
      using const_reverse_iterator = typename underlying_container::const_reverse_iterator;

      csv_parser(const char* fileName) :
         csv_parser(fileName, ',')
      {
      }

      csv_parser(const char* fileName, char delim)
      {
         std::ifstream inputStrm(fileName);
         if (inputStrm)
         {
            std::string line;
            while (std::getline(inputStrm, line))
            {
               auto commentCharPos = line.find_first_of('#');
               if (commentCharPos != std::string::npos)
               {
                  line = line.substr(0, commentCharPos);
               }

               if (!line.empty())
               {
                  m_File.emplace_back(line, delim);
               }
            }
         }
         else
         {
            NTL_THROW(std::runtime_error, "Failed to read CSV file.");
         }
      }

      const row_type& at(std::size_t idx) const
      {
         return m_File.at(idx);
      }

      row_type& at(std::size_t idx)
      {
         return m_File.at(idx);
      }

      iterator begin() noexcept
      {
         return m_File.begin();
      }

      const_iterator begin() const noexcept
      {
         return cbegin();
      }

      const_iterator cbegin() const noexcept
      {
         return m_File.cbegin();
      }

      const_iterator cend() const noexcept
      {
         return m_File.cend();
      }

      const_reverse_iterator crbegin() const noexcept
      {
         return m_File.crbegin();
      }

      const_reverse_iterator crend() const noexcept
      {
         return m_File.crend();
      }

      iterator end() noexcept
      {
         return m_File.end();
      }

      const_iterator end() const noexcept
      {
         return cend();
      }

      reverse_iterator rbegin() noexcept
      {
         return m_File.rbegin();
      }

      const_reverse_iterator rbegin() const noexcept
      {
         return crbegin();
      }

      reverse_iterator rend() noexcept
      {
         return m_File.rend();
      }

      const_reverse_iterator rend() const noexcept
      {
         return crend();
      }

      const row_type& operator [](std::size_t idx) const noexcept
      {
         return m_File[idx];
      }

      row_type& operator [](std::size_t idx) noexcept
      {
         return m_File[idx];
      }

      void reserve(std::size_t numRows)
      {
         m_File.reserve(numRows);
      }

   private:
      underlying_container m_File;
   };
}