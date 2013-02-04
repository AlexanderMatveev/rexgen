/*
    Copyright (c) 2012, Jan Starke <jan.starke@outofbed.org>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Jan Starke <jan.starke@outofbed.org> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Jan Starke <jan.starke@outofbed.org> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "librexgen_lua.h"
#include "../librexgen/librexgen.h"
#include "unicode.h"
#include "iterator/iterator.h"
#include "regex/regex.h"
#include <uniconv.h>

#if REXGEN_DEBUG == 1
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#endif

extern "C" {
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
}

#ifdef YYDEBUG
extern int rexgen_debug;
#endif

static const int BUFFER_SIZE = 4096;

static const luaL_Reg rexgen_lib [] = {
  { "parse_regex",	rexgen_parse_regex },
  { "get_syntax_tree",	rexgen_get_syntax_tree },
  {NULL, NULL}
};

extern "C"
int luaopen_rexgen(lua_State* L)
{
  
#if YYDEBUG==1
  rexgen_debug=1;
#endif
  
  luaL_newlib(L, rexgen_lib);
#ifdef REXGEN_DEBUG
#if REXGEN_DEBUG==1
  std::string initFileName = "log4cpp.properties";
  log4cpp::PropertyConfigurator::configure(initFileName);
#endif
#endif
  return 1;
}

extern "C"
int rexgen_iter(lua_State* L)
{
  Iterator *iter = *(Iterator **) lua_touserdata(L, lua_upvalueindex(1));
  if (iter->hasNext()) {
    iter->next();
    rexgen_value(L, iter);
    return 1;
  } else {
    return 0;
  }
}

extern "C" 
int rexgen_parse_regex(lua_State* L) {
  char_type xml[1024];
  Regex* re = parse_regex(luaL_checklstring(L, 1, NULL));
  
  Iterator** iter = (Iterator**) lua_newuserdata(L, sizeof (Iterator *));
  luaL_getmetatable(L, "rexgen.iter");
  lua_setmetatable(L, -2);
  *iter = re->iterator();
  lua_pushcclosure(L, rexgen_iter, 1);
  
  int length = re->appendRawValue(xml, sizeof(xml)/sizeof(xml[0])-1);
  push_utf8_string(L, xml, length);
  
  return 2;
}

extern "C"
int rexgen_value(lua_State* L, const Iterator* iter) {
  char_type buffer[BUFFER_SIZE];
  
  int length = iter->value(buffer, sizeof(buffer)/sizeof(buffer[0]));
  push_utf8_string(L, buffer, length);
  
  return 1;
}

extern "C"
int rexgen_get_syntax_tree(lua_State* L)
{
  char_type xml[1024];
  Regex* re = parse_regex(luaL_checklstring(L, 1, NULL));
  int length = re->appendRawValue(xml, sizeof(xml)/sizeof(xml[0])-1);
  push_utf8_string(L, xml, length);
  return 1;
}

void push_utf8_string(lua_State* L, char_type* str, int length)
{
#ifdef UTF8
  lua_pushlstring(L, str, length);
#else
  /* convert buffer to UTF-8 */
  uint8_t dst[BUFFER_SIZE];
#ifdef UTF16
#define convert_to_utf8(s,d,resultbuf,lengthp) u16_to_u8(s, d, resultbuf, lengthp)
#else /* UTF32 */
#define convert_to_utf8(s,d,resultbuf,lengthp) u32_to_u8(s, d, resultbuf, lengthp)
#endif
  size_t lengthp = sizeof(dst)/sizeof(dst[0]);
  /* uint8_t *res =  */
  convert_to_utf8(str, length, dst, &lengthp);
  lua_pushlstring(L, (const char*) dst, lengthp);
#endif
}