## ReChain

[![Build Status](https://travis-ci.org/mjhouse/rechain.svg?branch=master)](https://travis-ci.org/mjhouse/rechain)
[![Coverage Status](https://coveralls.io/repos/github/mjhouse/rechain/badge.svg?branch=master)](https://coveralls.io/github/mjhouse/rechain?branch=master)

ReChain is a modified blockchain design that uses signature transactions to enforce blockchain
integrity rather than an expensive proof-of-work calculation. There is still a fixed
proof-of-work difficulty in order to stop many transactions from being executed at once.

### Documentation

Generated documentation is available [here](https://mjhouse.github.io/rechain/). Currently,
there are no written tutorials or guides because the project isn't that far along yet.

#### ToDo

* set up automated linting with [CppCheck](http://cppcheck.sourceforge.net/) (or similar)
* set up automatic leak checking with [valgrind](http://valgrind.org/) (or similar)

#### Notes

* read [Cpp Best Practices](https://www.gitbook.com/book/lefticus/cpp-best-practices/details)

### Can I Contribute?

There isn't any support or guides for contributions, but if you add something cool, send me a pull request and I'll take a look at it. If you want to help and you aren't sure where to start, email me at mjhouse@protonmail.com.

### Tools

* make

* g++5.4.0

* doxygen

### Dependencies

| Dependency		        | Included for				                  | Status       |
|:----------------------|:--------------------------------------|:-------------|
| cxxopts               | Command line argument parsing         | Included     |
| cereal                | Serializing objects to/from files     | Included     |
| Crypto++              | Public keys and hashing               | Not included |
| Catch2                | Unit tests                            | Included     |
| Libtorrent            | For sharing published documents       | Not Included |
