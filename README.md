# BinarySemaphore.h

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/rigtorp/TokenBucket/master/LICENSE)

[Binary semaphore](https://en.wikipedia.org/wiki/Semaphore_(programming))
using [futexes](https://en.wikipedia.org/wiki/Futex). Useful for
saving energy when waiting for new data in lock-free data structures.
The fast path can still be lock-free, only when the consumer has
blocked will you incur the cost of as syscall.

## About

This project was created by [Erik Rigtorp](http://rigtorp.se)
<[erik@rigtorp.se](mailto:erik@rigtorp.se)>.
