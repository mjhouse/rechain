## ReChain

ReChain is a modified blockchain design that uses signature blocks to enforce blockchain
integrity rather than an expensive proof-of-work calculation. There is still a fixed
proof-of-work difficulty in order to stop many transactions from being executed at once.

### Documentation

Generated documentation is available [here](https://mjhouse.github.io/rechain/). Currently,
there are no written tutorials or guides because the project isn't that far along yet.

### Can I Contribute?

No.

### Dependencies

| Dependency		| Included for				|
|:----------------------|:--------------------------------------|
| cxxopts.hpp		| For command line argument parsing 	|
| cereal		| To serialize objects to/from files	|
| libcrypto++-dev	| All sorts of crypto goodies		|
| Catch2		| For unit tests			|
