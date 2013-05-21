/*
    Copyright (C) 2012-2013  Jan Starke <rexgen@outofbed.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/
*/

#include <librexgen/parser/rexgenparsercontext.h>
#include <librexgen/regex/regex.h>
#include <librexgen/iterator/iterator.h>
#include <librexgen/iterator/topiterator.h>
#include <librexgen/parser/osdepend.h>
#include <librexgen/parser/syntaxerror.h>
#include <sstream>
#include <string>
#include <cstdio>

using std::istringstream;

int rexgen_parse(RexgenParserContext* context);

Regex* parse_regex(RexgenParserContext* context) {
  
  try {
    if (rexgen_parse(context) != 0) {
      return nullptr;
    }
  } catch (SyntaxError& exc) {
    cerr << exc.getMessage() << endl;
    return nullptr;
  }  
  if (context->hasInvalidGroupReferences()) {
    throw SyntaxError("This regular expression has an invalid back reference");
  }
  return context->result;
}

EXPORT
Regex* parse_regex(const char* regex, const RexgenOptions& options) {
  const string strRegex(regex);
  istringstream is(strRegex);
  RexgenParserContext context(&is, options);
  return parse_regex(&context);
}

EXPORT
Iterator* regex_iterator(const char* regex, const RexgenOptions& options) {
  const string strRegex(regex);
  istringstream is(strRegex);
  RexgenParserContext context(&is, options);
  Regex* re = parse_regex(&context);
  if (re == nullptr) {
    return nullptr;
  }
  IteratorState* state = new IteratorState();
  state->setRandomize(options.randomize);
  Iterator* iter = new TopIterator(re->getId(), re->iterator(state), state);
  
  // register regex alternatives
  iter->updateReferences(state);
  
  // update references
  iter->updateReferences(state);
  return iter;
}