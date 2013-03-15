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

#include <librexgen/regex/regex.h>

int Regex::next_id = 0;

static int parseHexChar(char c) {
  if (c >= '0' && c <= '9') {
    return ((c) - '0');
  } else if (c >= 'a' && c <= 'f') {
    return ((c) - 'a' + 10);
  } else if (c >= 'A' && c <= 'F') {
    return ((c) - 'A' + 10);
  }
  return -1;
}

static unsigned __int32 parseHexString(const char_type* ptr, size_t length) {
  unsigned int hexvalue = 0;
  while (*ptr != 0 && length > 0) {
    const int v = parseHexChar(*ptr);
    if (v == -1) return -1;
    hexvalue = (hexvalue << 4) + v;
    ptr++;
    length--;
  }
  return hexvalue;
}

char_type Regex::parseFirstCharacter(const char_type* c) {
  if (c[0] == '\0') {
    return c[0];
  }
  /*
   * handle special sequences like \x, \u, \n, \r and so on
   */
  if (c[0] == '\\' && c[1] != '\0') {
    switch (c[1]) {
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case '0':
      return '\0';
    case 'x':
      return (char_type) parseHexString((c+2), 2);
    case 'u':
#if defined(UTF32)
      return parseHexString((c+2), 4);
#elif defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
		/* zero out the leading bytes */
		return (char_type) (0x0000ffff & parseHexString((c+2), 4));
#else
      unsigned __int32 codepoint[2];
      char_type buffer[4];
      size_t length = 0;
      codepoint[0] = parseHexString((c+2), 4);
      codepoint[1] = 0;
#if defined(UTF8)
      u32_to_u8(codepoint, 1, buffer, &length);
#elif defined(UTF16)
      u32_to_u16(codepoint, 1, buffer, &length);
#else
#error "no encoding defined"
#endif
      return buffer[0];
#endif
    default:
      return c[1];
    }
  }
  return c[0];
}

int Regex::xmlEncapsulate(
    char_type* dst, size_t size, const char_type* clazz, int level) const {
  int l;
  int length = 0;
#if defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  const wchar_t* format = _T("<%s id=\"%d\" min=\"%d\" max=\"%d\">\n");
#else
  const char* format = "<" PRINTF_FORMAT " id=\"%d\" min=\"%d\" max=\"%d\">\n";
#endif

  l = utf_snprintf(
    dst, size, format, clazz, getId(), getMinOccurs(), getMaxOccurs());
  length += l;
  if ((size -= l) < 0) goto finish;
  dst += l;


  l = appendContent(dst, size, level+1);
  length += l;
  if ((size -= l) < 0) goto finish;
  dst += l;

  l = appendSpace(dst, size, level);
  length += l;
  if ((size -= l) < 0) goto finish;
  dst += level;

#if defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  format = _T("</%s>\n");
#else
  format = "</" PRINTF_FORMAT  ">\n";
#endif

  length += utf_snprintf(dst, size, format, getXmlTag());

  finish:
  return length;
}

int Regex::appendSpace(char_type* dst, size_t size, int count) const {
  int length = 0;
  while (count > 0 && size > 1) {
    *dst = ' ';
    ++dst;
    --size;
    --count;
    ++length;
  }
  if (size >= 1) {
    *dst = '\0';
  }
  return length;
}
