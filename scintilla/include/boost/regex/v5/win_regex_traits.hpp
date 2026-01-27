#ifndef BOOST_WIN_REGEX_TRAITS_HPP_INCLUDED
#define BOOST_WIN_REGEX_TRAITS_HPP_INCLUDED

#include <boost/regex/pattern_except.hpp>
#include <boost/regex/v5/regex_traits_defaults.hpp>
#include <boost/regex/v5/primary_transform.hpp>
#include <memory>
#include <map>
#include "VectorISA.h"

namespace boost{

//
// forward declaration is needed by some compilers:
//
template <class charT>
class w32_regex_traits;

template <>
class w32_regex_traits<wchar_t>
{
public:
   typedef wchar_t                       charT;
   typedef charT                         char_type;
   typedef std::size_t                   size_type;
   typedef std::basic_string<char_type>  string_type;
   typedef LCID                          locale_type;
   typedef std::uint32_t                 char_class_type;

   struct boost_extensions_tag{};

   w32_regex_traits():
      m_lower_map{new wchar_t[characterCount*2]}
   {
#if 0 // prevent compiler vectorize the loop generates big and slow code
      for (int i = 0; i < characterCount; i++) {
         m_lower_map[i] = static_cast<wchar_t>(i);
      }
#elif NP2_USE_SSE2
      {
         __m128i *ptr = reinterpret_cast<__m128i *>(m_lower_map.get());
         __m128i value = _mm_setr_epi16(0, 1, 2, 3, 4, 5, 6, 7);
         const __m128i acc = _mm_set1_epi16(0x0008);
         constexpr int count = characterCount / (sizeof(__m128i) / sizeof(wchar_t));
#if defined(__clang__)
         #pragma clang loop unroll(disable)
#elif defined(__GNUC__)
         #pragma GCC unroll 0
#endif
         for (int i = 0; i < count; i++) {
            _mm_storeu_si128(ptr, value);
            ptr++;
            value = _mm_add_epi16(value, acc);
         }
      }
#elif defined(_WIN64)
      {
         uint64_t *ptr = reinterpret_cast<uint64_t *>(m_lower_map.get());
         uint64_t value = 0x0003'0002'0001'0000ULL;
         constexpr uint64_t acc = 0x0004'0004'0004'0004ULL;
         constexpr int count = characterCount / (sizeof(uint64_t) / sizeof(wchar_t));
         for (int i = 0; i < count; i++) {
            *ptr++ = value;
            value += acc;
         }
      }
#else
      {
         uint32_t *ptr = reinterpret_cast<uint32_t *>(m_lower_map.get());
         uint32_t value = 0x0001'0000;
         constexpr uint32_t acc = 0x0002'0002;
         constexpr int count = characterCount / (sizeof(uint32_t) / sizeof(wchar_t));
         for (int i = 0; i < count; i++) {
            *ptr++ = value;
            value += acc;
         }
      }
#endif

      static_assert(sizeof(wchar_t) == sizeof(WORD));
      m_ctype_map = reinterpret_cast<WORD *>(m_lower_map.get() + characterCount);
      memset(m_ctype_map, 0, characterCount*sizeof(WORD));
      ::GetStringTypeExW(LOCALE_USER_DEFAULT, CT_CTYPE1, m_lower_map.get(), characterCount, m_ctype_map);
      ::LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, m_lower_map.get(), characterCount, m_lower_map.get(), characterCount, nullptr, nullptr, 0);

      //
      // get the collation format used by m_pcollate:
      //
      m_collate_type = BOOST_REGEX_DETAIL_NS::find_sort_syntax(this, &m_collate_delim);
      // we need to start by initialising our syntax map so we know which
      // character is used for which purpose:
      for(regex_constants::syntax_type i = 1; i < regex_constants::syntax_max; ++i)
      {
         if (const char* ptr = BOOST_REGEX_DETAIL_NS::get_default_syntax(i))
         {
            while(*ptr)
            {
               const unsigned char c = *ptr++;
               this->m_char_map[c] = i;
            }
         }
      }
   }
   static size_type length(const char_type* p) noexcept
   {
      return std::char_traits<charT>::length(p);
   }
   regex_constants::syntax_type syntax_type(charT c) const
   {
      const auto i = m_char_map.find(c);
      return ((i == m_char_map.end()) ? 0 : i->second);
   }
   regex_constants::escape_syntax_type escape_syntax_type(charT c) const
   {
      const auto i = m_char_map.find(c);
      if(i == m_char_map.end())
      {
         if(m_ctype_map[c] & C1_LOWER) return regex_constants::escape_type_class;
         if(m_ctype_map[c] & C1_UPPER) return regex_constants::escape_type_not_class;
         return 0;
      }
      return i->second;
   }
   charT translate(charT c) const
   {
      return c;
   }
   charT translate_nocase(charT c) const
   {
      return this->tolower(c);
   }
   charT translate(charT c, bool icase) const
   {
      return icase ? this->tolower(c) : c;
   }
   charT tolower(charT c) const
   {
      return m_lower_map[c];
   }
   charT toupper(charT c) const
   {
      return c; // toupper() is unused
   }
   string_type transform(const charT* p1, const charT* p2) const
   {
      return w32_transform(p1, p2);
   }
   string_type transform_primary(const charT* p1, const charT* p2) const
   {
      string_type result;
      //
      // What we do here depends upon the format of the sort key returned by
      // sort key returned by this->transform:
      //
      switch(m_collate_type)
      {
      case BOOST_REGEX_DETAIL_NS::sort_C:
      case BOOST_REGEX_DETAIL_NS::sort_unknown:
         // the best we can do is translate to lower case, then get a regular sort key:
         {
            result.assign(p1, p2);
            for(charT &c : result)
               c = this->tolower(c);
            result = this->transform(&*result.begin(), &*result.begin() + result.size());
            break;
         }
      case BOOST_REGEX_DETAIL_NS::sort_fixed:
         {
            // get a regular sort key, and then truncate it:
            result.assign(this->transform(p1, p2));
            result.erase(this->m_collate_delim);
            break;
         }
      case BOOST_REGEX_DETAIL_NS::sort_delim:
            // get a regular sort key, and then truncate everything after the delim:
            result.assign(this->transform(p1, p2));
            std::size_t i;
            for(i = 0; i < result.size(); ++i)
            {
               if(result[i] == m_collate_delim)
                  break;
            }
            result.erase(i);
            break;
      }
      if(result.empty())
         result = string_type(1, charT(0));
      return result;
   }
   char_class_type lookup_classname(const charT* p1, const charT* p2) const
   {
      char_class_type result = lookup_classname_imp(p1, p2);
      if(result == 0)
      {
         string_type temp(p1, p2);
         for(charT &c : temp)
            c = this->tolower(c);
         result = lookup_classname_imp(&*temp.begin(), &*temp.begin() + temp.size());
      }
      return result;
   }
   string_type lookup_collatename(const charT* p1, const charT* p2) const
   {
      //std::string name(p1, p2);
      std::string name(p2 - p1, '\0');
      const charT* t1 = p1;
      for (char &c : name) {
         c = static_cast<char>(*t1++);
      }
      name = BOOST_REGEX_DETAIL_NS::lookup_default_collate_name(name);
      if(!name.empty())
         return string_type(name.begin(), name.end());
      if(p2 - p1 == 1)
         return string_type(1, *p1);
      return string_type();
   }
   bool isctype(charT c, char_class_type f) const
   {
      if((f & mask_base)
         && (m_ctype_map[c] & (f & mask_base)))
         return true;
      else if((f & mask_unicode) && BOOST_REGEX_DETAIL_NS::is_extended(c))
         return true;
      else if((f & mask_word) && (c == '_'))
         return true;
      else if((f & mask_vertical)
         && (::boost::BOOST_REGEX_DETAIL_NS::is_separator(c) || (c == '\v')))
         return true;
      else if((f & mask_horizontal)
         && (m_ctype_map[c] & C1_SPACE) && !(::boost::BOOST_REGEX_DETAIL_NS::is_separator(c) || (c == '\v')))
         return true;
      return false;
   }
   std::intmax_t toi(const charT*& p1, const charT* p2, int radix) const
   {
      return ::boost::BOOST_REGEX_DETAIL_NS::global_toi(p1, p2, radix, *this);
   }
   int value(charT c, int radix) const
   {
      int result = ::boost::BOOST_REGEX_DETAIL_NS::global_value(c);
      return result < radix ? result : -1;
   }
   locale_type imbue([[maybe_unused]] locale_type l) const
   {
      const locale_type result(getloc());
      return result;
   }
   locale_type getloc() const
   {
      return LOCALE_USER_DEFAULT;
   }
   std::string error_string(regex_constants::error_type n) const
   {
      return BOOST_REGEX_DETAIL_NS::get_default_error_string(n);
   }

private:
   unsigned                       m_collate_type;    // the form of the collation string
   charT                          m_collate_delim;   // the collation group delimiter
   WORD *m_ctype_map;
   std::unique_ptr<wchar_t[]> m_lower_map;
   // TODO: use a hash table when available!
   std::map<charT, regex_constants::syntax_type> m_char_map;

   static constexpr int characterCount = 0xffff + 1;
   static constexpr char_class_type mask_word = 0x0400; // must be C1_DEFINED << 1
   static constexpr char_class_type mask_unicode = 0x0800; // must be C1_DEFINED << 2
   static constexpr char_class_type mask_horizontal = 0x1000; // must be C1_DEFINED << 3
   static constexpr char_class_type mask_vertical = 0x2000; // must be C1_DEFINED << 4
   static constexpr char_class_type mask_base = 0x3ff;  // all the masks used by the CT_CTYPE1 group

   static char_class_type lookup_classname_imp(const charT* p1, const charT* p2)
   {
      static const char_class_type masks[22] =
      {
         0,
         0x0104u, // C1_ALPHA | C1_DIGIT
         0x0100u, // C1_ALPHA
         0x0040u, // C1_BLANK
         0x0020u, // C1_CNTRL
         0x0004u, // C1_DIGIT
         0x0004u, // C1_DIGIT
         (~(0x0020u|0x0008u|0x0040) & 0x01ffu) | 0x0400u, // not C1_CNTRL or C1_SPACE or C1_BLANK
         mask_horizontal,
         0x0002u, // C1_LOWER
         0x0002u, // C1_LOWER
         (~0x0020u & 0x01ffu) | 0x0400, // not C1_CNTRL
         0x0010u, // C1_PUNCT
         0x0008u, // C1_SPACE
         0x0008u, // C1_SPACE
         0x0001u, // C1_UPPER
         mask_unicode,
         0x0001u, // C1_UPPER
         mask_vertical,
         0x0104u | mask_word,
         0x0104u | mask_word,
         0x0080u, // C1_XDIGIT
      };
      std::size_t state_id = 1u + (std::size_t)BOOST_REGEX_DETAIL_NS::get_default_class_id(p1, p2);
      if(state_id < sizeof(masks) / sizeof(masks[0]))
         return masks[state_id];
      return masks[0];
   }

   static std::wstring w32_transform(const wchar_t* p1, const wchar_t* p2)
   {
      int bytes = ::LCMapStringEx(
         LOCALE_NAME_USER_DEFAULT,       // locale identifier
         LCMAP_SORTKEY,  // mapping transformation type
         p1,  // source string
         static_cast<int>(p2 - p1),        // number of characters in source string
         nullptr,  // destination buffer
         0,        // size of destination buffer
         nullptr, nullptr, 0
      );
      if (!bytes)
         return std::wstring(p1, p2);
      std::string result(++bytes, '\0');
      bytes = ::LCMapStringEx(
         LOCALE_NAME_USER_DEFAULT,       // locale identifier
         LCMAP_SORTKEY,  // mapping transformation type
         p1,  // source string
         static_cast<int>(p2 - p1),        // number of characters in source string
         reinterpret_cast<wchar_t*>(&*result.begin()),  // destination buffer *of bytes*
         bytes,        // size of destination buffer
         nullptr, nullptr, 0
      );
      if (bytes > static_cast<int>(result.size()))
         return std::wstring(p1, p2);
      while (!result.empty() && result.back() == L'\0')
      {
         result.pop_back();
      }
      std::wstring r2;
      for (const char c : result)
         r2.push_back(static_cast<unsigned char>(c));
      return r2;
   }
};

} // boost

#endif
