/*
 *
 * Copyright (c) 2003
 * John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
 
 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         syntax_type.hpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: Declares regular expression synatx type enumerator.
  */

#ifndef BOOST_REGEX_SYNTAX_TYPE_HPP
#define BOOST_REGEX_SYNTAX_TYPE_HPP

namespace boost{
namespace regex_constants{

typedef unsigned char syntax_type;

//
// values chosen are binary compatible with previous version:
//
constexpr syntax_type syntax_char = 0;
constexpr syntax_type syntax_open_mark = 1;
constexpr syntax_type syntax_close_mark = 2;
constexpr syntax_type syntax_dollar = 3;
constexpr syntax_type syntax_caret = 4;
constexpr syntax_type syntax_dot = 5;
constexpr syntax_type syntax_star = 6;
constexpr syntax_type syntax_plus = 7;
constexpr syntax_type syntax_question = 8;
constexpr syntax_type syntax_open_set = 9;
constexpr syntax_type syntax_close_set = 10;
constexpr syntax_type syntax_or = 11;
constexpr syntax_type syntax_escape = 12;
constexpr syntax_type syntax_dash = 14;
constexpr syntax_type syntax_open_brace = 15;
constexpr syntax_type syntax_close_brace = 16;
constexpr syntax_type syntax_digit = 17;
constexpr syntax_type syntax_comma = 27;
constexpr syntax_type syntax_equal = 37;
constexpr syntax_type syntax_colon = 36;
constexpr syntax_type syntax_not = 53;

// extensions:

constexpr syntax_type syntax_hash = 13;
constexpr syntax_type syntax_newline = 26;

// escapes:

typedef syntax_type escape_syntax_type;

constexpr escape_syntax_type escape_type_word_assert = 18;
constexpr escape_syntax_type escape_type_not_word_assert = 19;
constexpr escape_syntax_type escape_type_control_f = 29;
constexpr escape_syntax_type escape_type_control_n = 30;
constexpr escape_syntax_type escape_type_control_r = 31;
constexpr escape_syntax_type escape_type_control_t = 32;
constexpr escape_syntax_type escape_type_control_v = 33;
constexpr escape_syntax_type escape_type_ascii_control = 35;
constexpr escape_syntax_type escape_type_hex = 34;
constexpr escape_syntax_type escape_type_unicode = 0; // not used
constexpr escape_syntax_type escape_type_identity = 0; // not used
constexpr escape_syntax_type escape_type_backref = syntax_digit;
constexpr escape_syntax_type escape_type_decimal = syntax_digit; // not used
constexpr escape_syntax_type escape_type_class = 22; 
constexpr escape_syntax_type escape_type_not_class = 23; 

// extensions:

constexpr escape_syntax_type escape_type_left_word = 20;
constexpr escape_syntax_type escape_type_right_word = 21;
constexpr escape_syntax_type escape_type_start_buffer = 24;                 // for \`
constexpr escape_syntax_type escape_type_end_buffer = 25;                   // for \'
constexpr escape_syntax_type escape_type_control_a = 28;                    // for \a
constexpr escape_syntax_type escape_type_e = 38;                            // for \e
constexpr escape_syntax_type escape_type_E = 47;                            // for \Q\E
constexpr escape_syntax_type escape_type_Q = 48;                            // for \Q\E
constexpr escape_syntax_type escape_type_X = 49;                            // for \X
constexpr escape_syntax_type escape_type_C = 50;                            // for \C
constexpr escape_syntax_type escape_type_Z = 51;                            // for \Z
constexpr escape_syntax_type escape_type_G = 52;                            // for \G

constexpr escape_syntax_type escape_type_property = 54;                     // for \p
constexpr escape_syntax_type escape_type_not_property = 55;                 // for \P
constexpr escape_syntax_type escape_type_named_char = 56;                   // for \N
constexpr escape_syntax_type escape_type_extended_backref = 57;             // for \g
constexpr escape_syntax_type escape_type_reset_start_mark = 58;             // for \K
constexpr escape_syntax_type escape_type_line_ending = 59;                  // for \R

constexpr escape_syntax_type syntax_max = 60;

}
}


#endif
