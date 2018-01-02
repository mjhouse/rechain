## ReChain

[![build status](https://travis-ci.org/mjhouse/rechain.svg?branch=master)](https://travis-ci.org/mjhouse/rechain.svg?branch=master)
[![Coverage Status](https://coveralls.io/repos/github/mjhouse/rechain/badge.svg?branch=master)](https://coveralls.io/github/mjhouse/rechain?branch=master)

ReChain is a modified blockchain design that uses signature blocks to enforce blockchain
integrity rather than an expensive proof-of-work calculation. There is still a fixed
proof-of-work difficulty in order to stop many transactions from being executed at once.

### Documentation

Generated documentation is available [here](https://mjhouse.github.io/rechain/). Currently,
there are no written tutorials or guides because the project isn't that far along yet.

### Can I Contribute?

No.

### Tools

* make 4.1

* gcc 5.4.0

### Dependencies

| Dependency		| Included for				|
|:----------------------|:--------------------------------------|
| cxxopts.hpp		| Command line argument parsing 	|
| cereal		| Serializing objects to/from files	|
| libcrypto++-dev	| Public keys and hashing		|
| Catch2		| Unit tests				|
