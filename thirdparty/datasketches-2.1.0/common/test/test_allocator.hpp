/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef TEST_ALLOCATOR_HPP
#define TEST_ALLOCATOR_HPP

#include <new>
#include <utility>

// this allocator keeps the total allocated size in a global variable for testing

namespace datasketches {

extern long long test_allocator_total_bytes;

template <class T> class test_allocator {
public:
  typedef T                 value_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef std::size_t       size_type;
  typedef std::ptrdiff_t    difference_type;

  template <class U>
  struct rebind { typedef test_allocator<U> other; };

  test_allocator() {}
  test_allocator(const test_allocator&) {}
  template <class U>
  test_allocator(const test_allocator<U>&) {}
  ~test_allocator() {}

  pointer address(reference x) const { return &x; }
  const_pointer address(const_reference x) const {
    return x;
  }

  pointer allocate(size_type n, const_pointer = 0) {
    void* p = new char[n * sizeof(value_type)];
    if (!p) throw std::bad_alloc();
    test_allocator_total_bytes += n * sizeof(value_type);
    return static_cast<pointer>(p);
  }

  void deallocate(pointer p, size_type n) {
    if (p) delete[] (char*) p;
    test_allocator_total_bytes -= n * sizeof(value_type);
  }

  size_type max_size() const {
    return static_cast<size_type>(-1) / sizeof(value_type);
  }

  template<typename... Args>
  void construct(pointer p, Args&&... args) {
    new(p) value_type(std::forward<Args>(args)...);
  }
  void destroy(pointer p) { p->~value_type(); }

private:
  void operator=(const test_allocator&);
};

template<> class test_allocator<void> {
public:
  typedef void        value_type;
  typedef void*       pointer;
  typedef const void* const_pointer;

  template <class U>
  struct rebind { typedef test_allocator<U> other; };
};


template <class T>
inline bool operator==(const test_allocator<T>&, const test_allocator<T>&) {
  return true;
}

template <class T>
inline bool operator!=(const test_allocator<T>&, const test_allocator<T>&) {
  return false;
}

} /* namespace datasketches */

#endif
