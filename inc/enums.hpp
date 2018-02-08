
#ifndef _RECHAIN_ENUMS_HPP_
#define _RECHAIN_ENUMS_HPP_

/** For different log levels */
enum Level {
    debug   = 0,
    info    = 1,
    warning = 2,
    error   = 3,
    none    = 4
};

/** For different flavors of Record. */
enum DataType { 
    Publication, 
    Signature 
};

#endif
