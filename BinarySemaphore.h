/*
Copyright (c) 2016 Erik Rigtorp <erik@rigtorp.se>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#pragma once

#include <atomic>
#include <cerrno>
#include <exception>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

namespace {
static int futex(int *uaddr, int futex_op, int val,
                 const struct timespec *timeout, int *uaddr2, int val3) {
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr, val3);
}
}

class BinarySemaphore {
public:
  bool wait(struct timespec *timeout = nullptr) {
    for (;;) {
      int one = 1;
      if (i.compare_exchange_strong(one, 0, std::memory_order_relaxed,
                                    std::memory_order_relaxed)) {
        return true;
      }
      auto rc = futex((int *)&i, FUTEX_WAIT, 0, timeout, nullptr, 0);
      if (rc == -1) {
        if (errno == ETIMEDOUT) {
          return false;
        }
        if (errno != EAGAIN) {
          std::terminate();
        }
      }
    }
  }
  void post() {
    int zero = 0;
    if (i.compare_exchange_strong(zero, 1, std::memory_order_relaxed,
                                  std::memory_order_relaxed)) {
      if (futex((int *)&i, FUTEX_WAKE, 1, nullptr, nullptr, 0) == -1) {
        std::terminate();
      }
    }
  }

private:
  std::atomic<int> i = {0};
};
