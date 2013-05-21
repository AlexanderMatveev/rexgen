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

#ifndef __Regex_h__
#define __Regex_h__

#include <librexgen/iterator/iteratorpermuter.h>
#include <librexgen/regex/quantifier.h>
#include <librexgen/iterator/iterator.h>
#include <iostream>
#include <librexgen/unicode.h>
#include <librexgen/debug.h>
#include <librexgen/simplestring.h>

typedef enum {
  Compound,
  Alternative,
  Terminal,
  Class,
  Reference,
  Stream
} RegexType;

class IteratorState;

class Regex  {
public:
  Regex() : quantifier(1,1) {id=createId();}
  virtual ~Regex() {}
  
  inline unsigned int getMinOccurs() const { return quantifier.getMin(); }
  inline unsigned int getMaxOccurs() const { return quantifier.getMax(); }
  
  inline void setMinOccurs(unsigned int _min) { quantifier.setMin(_min); }
  inline void setMaxOccurs(unsigned int _max) { quantifier.setMax(_max); }
  
  inline void setQuantifier(const Quantifier& q) { quantifier = q; }
    
  virtual const char* getXmlTag() const = 0;
  virtual RegexType getRegexType() const = 0;
  
  virtual inline int getMaxSize() const {
    return getMaxOccurs();
  }
  
  inline void appendRawValue(SimpleString& dst) const {
    appendRawValue(dst, 0);
  }
  
  inline void appendRawValue(SimpleString& dst, int level) const {
    return xmlEncapsulate(dst, getXmlTag(), level);
  }
  
  void appendSpace(SimpleString& dst, int count) const;
  
  virtual void appendContent(SimpleString& dst, int level) const = 0;
  
  virtual void xmlEncapsulate(SimpleString& dst, const char* clazz, int level) const;
  
  virtual Iterator* iterator(IteratorState* state) const {
    if (getMinOccurs() == 1 && getMaxOccurs() == 1) {
      return singleIterator(state);
    } else {
      return new IteratorPermuter(
        getId(), this, state, getMinOccurs(), getMaxOccurs());
    }    
  }
  virtual Iterator* singleIterator(IteratorState* state) const = 0;
  
  int getId() const { return id; }
  
  virtual int getGroupId() const { return -1; }
  
protected:
  
  virtual int createId() {
    return ++next_id;
  }
  
private:
  Quantifier quantifier;
  static int next_id;
  int id;
};

#endif