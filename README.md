## ReChain

<!-- @cond suppress 
-->
<!-- 
[![Build Status](https://travis-ci.org/mjhouse/rechain.svg?branch=master)](https://travis-ci.org/mjhouse/rechain)
[![Coverage Status](https://coveralls.io/repos/github/mjhouse/rechain/badge.svg?branch=master)](https://coveralls.io/github/mjhouse/rechain?branch=master)
-->
<!-- @endcond 
-->

__Not Maintained__

ReChain is a modified blockchain design that uses signature records to enforce blockchain
integrity rather than an expensive proof-of-work calculation (although there is still a fixed 
proof-of-work to stop clients from creating large numbers of records at once). The goal is to 
create a network of clients that share the burden of hosting, publishing and peer-reviewing 
a repository of research documents in an immutable blockchain.

### Documentation/Status

* [Backlog](https://tree.taiga.io/project/mjhouse-rechain/backlog)
* [Documentation](https://mjhouse.github.io/rechain/)

Currently, there are no written tutorials or guides because the project isn't that far along yet (the documentation is generated using doxygen). You can see the features and fixes I haven't started yet in the backlog. The taskboard (right side, latest sprint) is what I'm using to keep things organized on a daily basis.

### Can I Contribute?

There isn't any support or guides for contributions, but if you add something cool, send me a pull request and I'll take a look at it. If you want to help and you aren't sure where to start, email me at mjhouse@protonmail.com.

### Tools

* make 4.1

* g++ 5.4.0

* doxygen 1.8.11

* valgrind 3.11.0

### Dependencies

| Dependency		    | Included for		                    | Status       |
|:----------------------|:--------------------------------------|:-------------|
| cxxopts               | Command line argument parsing         | Included     |
| cereal                | Serializing objects to/from files     | Included     |
| Crypto++              | Public keys and hashing               | Not included |
| Catch2                | Unit tests                            | Included     |
| libtorrent            | Sharing published documents           | Not Included |
