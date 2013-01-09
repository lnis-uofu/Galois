/** GFIFO worklist -*- C++ -*-
 * @file
 * @section License
 *
 * Galois, a framework to exploit amorphous data-parallelism in irregular
 * programs.
 *
 * Copyright (C) 2012, The University of Texas at Austin. All rights reserved.
 * UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING THIS
 * SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR ANY PARTICULAR PURPOSE, NON-INFRINGEMENT AND WARRANTIES OF
 * PERFORMANCE, AND ANY WARRANTY THAT MIGHT OTHERWISE ARISE FROM COURSE OF
 * DEALING OR USAGE OF TRADE.  NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH
 * RESPECT TO THE USE OF THE SOFTWARE OR DOCUMENTATION. Under no circumstances
 * shall University be liable for incidental, special, indirect, direct or
 * consequential damages or loss of profits, interruption of business, or
 * related expenses which may arise from use of Software or Documentation,
 * including but not limited to those resulting from defects in Software and/or
 * Documentation, or loss or inaccuracy of data of any kind.
 *
 * @author Andrew Lenharth <andrewl@lenharth.org>
 */

#ifndef GALOIS_WORKLIST_GFIFO_H
#define GALOIS_WORKLIST_GFIFO_H

#include "Galois/Runtime/ll/PaddedLock.h"
#include "Galois/gdeque.h"
#include "WLCompileCheck.h"

namespace Galois {
namespace WorkList {

template<typename T = int, bool concurrent = true>
  class GFIFO : private boost::noncopyable, private Runtime::LL::PaddedLock<concurrent>  {
  Galois::gdeque<T> wl;

  using Runtime::LL::PaddedLock<concurrent>::lock;
  using Runtime::LL::PaddedLock<concurrent>::try_lock;
  using Runtime::LL::PaddedLock<concurrent>::unlock;

public:
  template<bool newconcurrent>
  using rethread = GFIFO<T, newconcurrent>;
  template<typename Tnew>
  using retype = GFIFO<Tnew, concurrent>;

  typedef T value_type;

  void push(const value_type& val) {
    lock();
    wl.push_back(val);
    unlock();
  }

  template<typename Iter>
  void push(Iter b, Iter e) {
    lock();
    while (b != e)
      wl.push_back(*b++);
    unlock();
  }

  template<typename RangeTy>
  void push_initial(RangeTy range) {
    if (Runtime::LL::getTID() == 0)
      push(range.begin(), range.end());
  }

  boost::optional<value_type> pop() {
    boost::optional<value_type> retval;
    lock();
    if (!wl.empty()) {
      retval = wl.front();
      wl.pop_front();
    }
    unlock();
    return retval;
  }
};
GALOIS_WLCOMPILECHECK(GFIFO)

} // end namespace WorkList
} // end namespace Galois

#endif