/*
    Copyright (c) 2013, Jan Starke <jan.starke@outofbed.org>
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


#include <librexgen/regex/groupreference.h>
#include <librexgen/iterator/iteratorpermuter.h>
#include <librexgen/iterator/groupreferenceiterator.h>

Iterator* GroupReference::iterator(IteratorState* state) const {
  assert(groupRef != NULL);
  if (getMinOccurs() == 1 && getMaxOccurs() == 1) {
    return new GroupReferenceIterator(
      getId(), state->getIterator(groupRef->getGroupId()));
  } else {
    return new IteratorPermuter(
      getId(), this, state, getMinOccurs(), getMaxOccurs());
  }
}

Iterator* GroupReference::singleIterator(IteratorState* state) const {
  assert(groupRef != NULL);
  return new GroupReferenceIterator(
    getId(), state->getIterator(groupRef->getGroupId()));
}

int GroupReference::appendContent(
    char_type* dst, size_t size, int level) const {
  size_t l, length = 0;
  l = appendSpace(dst, size, level);
  if (size <= l) goto finish;
  length += l;
  size -= l;
  dst += l;

#if defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  length += utf_snprintf(dst, size, _T("%d\n"), groupRef->getId());
#else
   length += utf_snprintf(dst, size, "%d\n", groupRef->getId());
#endif
  finish:
  return length;
}