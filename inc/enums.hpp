
#ifndef _RECHAIN_ENUMS_HPP_
#define _RECHAIN_ENUMS_HPP_

#if defined(_WIN32) || defined(_WIN64)
    #define WINDOWS
#elif defined(__linux__)
    #define LINUX
#elif defined(_APPLE_)
    #define APPLE
#endif

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
