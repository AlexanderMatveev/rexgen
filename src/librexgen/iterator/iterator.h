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


#ifndef ITERATOR_H
#define ITERATOR_H
#include <log4cpp/Category.hh>
#include <assert.h>
#include "../unicode.h"

class Iterator
{
public:
  Iterator(int _id):
    LOGGER(log4cpp::Category::getRoot()), state(resetted), id(_id),
    can_use_value(false) {}
  
  virtual bool hasNext() const { return false; }
  virtual void next() {}
  virtual int value(char_type* /* dst */, ssize_t /* size */ ) const { return 0; }
  virtual void reset() { state = resetted; }
  
  virtual bool canUseValue() const { return (state == usable); }
  
  int getId() const { return id; }
  virtual int toString(char_type* /* dst */, ssize_t /* size */) { return 0; }
  
protected:
  log4cpp::Category& LOGGER;
  
  enum {
    resetted,
    usable,
    not_usable
  } state;
  
private:
  const int id;
  bool can_use_value;
};

#endif // ITERATOR_H
