## ReChain

[![Build Status](https://travis-ci.org/mjhouse/rechain.svg?branch=master)](https://travis-ci.org/mjhouse/rechain)
[![Coverage Status](https://coveralls.io/repos/github/mjhouse/rechain/badge.svg?branch=master)](https://coveralls.io/github/mjhouse/rechain?branch=master)

ReChain is a modified blockchain design that uses signature records to enforce blockchain
integrity rather than an expensive proof-of-work calculation (although there is still a fixed 
proof-of-work to stop clients from creating large numbers of blocks at once). The goal is to 
create a network of clients that share the burden of hosting, publishing and peer-reviewing 
a repository of research documents in an immutable blockchain.

### Documentation/Status

* [Taskboard](https://tree.taiga.io/project/mjhouse-rechain/taskboard/2018_4_3)
* [Documentation](https://mjhouse.github.io/rechain/)

Currently, there are no written tutorials or guides because the project isn't that far along yet (the documentation is generated using doxygen). The taskboard is what I'm using to keep things organized. You can see the features and fixes I haven't started yet in the backlog (left-side menu).

### Can I Contribute?

There isn't any support or guides for contributions, but if you add something cool, send me a pull request and I'll take a look at it. If you want to help and you aren't sure where to start, email me at mjhouse@protonmail.com.


### ToDo

- [ ] write bash script to configure environment and verify that necessary tools are installed for development
- [X] publish should broadcast Records to miners rather than mining locally
- [ ] mining should be a daemonized process that listens for new Records
- [X] review trust calculation and add linear scaling (so trust score is percentage of total)
- [ ] refactor PrivateKey/PublicKey into a single class (don't need user-settable public key)
- [ ] set up automated linting with [clang-tidy](http://clang.llvm.org/extra/clang-tidy/) (or similar)
- [ ] set up automatic leak checking with [valgrind](http://valgrind.org/) (or similar)

#### Notes

* read [Cpp Best Practices](https://www.gitbook.com/book/lefticus/cpp-best-practices/details)

### Tools

* make

* g++5.4.0

* doxygen

### Dependencies

| Dependency		    | Included for		                    | Status       |
|:----------------------|:--------------------------------------|:-------------|
| cxxopts               | Command line argument parsing         | Included     |
| cereal                | Serializing objects to/from files     | Included     |
| Crypto++              | Public keys and hashing               | Not included |
| Catch2                | Unit tests                            | Included     |
| libtorrent            | Sharing published documents           | Not Included |
