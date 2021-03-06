/*
    rexgen - a tool to create words based on regular expressions
    Copyright (C) 2012-2017  Jan Starke <jan.starke@outofbed.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2 of the License, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
    more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin St, Fifth Floor, Boston, MA 02110, USA
*/

#include <librexgen/iterator/streamregexiterator.h>
#include <cstdio>
namespace rexgen {
  void StreamRegexIterator::readNextWord() {
    __hasNext = false;
    assert(callback != NULL);
    length = callback(&buffer[0], STREAMREGEXITERATOR_MAXLEN);
    if (length > 0) {
      __hasNext = true;
    }
  }

  std::shared_ptr<SerializableState> StreamRegexIterator::getCurrentState() const {
    auto s = Iterator::getCurrentState();
    s->addValue(0);
    return s;
  }

  void StreamRegexIterator::setCurrentState(const std::shared_ptr<SerializableState>& s) {
    Iterator::setCurrentState(s);
    // fseek(infile, s->getValue(0), SEEK_SET);
  }

}