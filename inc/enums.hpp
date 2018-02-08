
#ifndef _RECHAIN_ENUMS_HPP_
#define _RECHAIN_ENUMS_HPP_

/** For different log levels */
enum Level {
    error   = 0,
    warning = 1,
    debug   = 2,
    info    = 3,
    none    = 4
};

/** For different flavors of Record. */
enum DataType { 
    Publication, 
    Signature 
};

#endif
