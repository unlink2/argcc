#ifndef __LIBLC__TYPEDEFS_H__
#define __LIBLC_TYPEDEFS_H__

namespace liblc {
    enum ObjectType {
        NIL,
        BOOLEAN,
        NUMBER,
        REAL,
        STRING,
        LIST,
        SECTION,
        IGNORE,
    };

    typedef ObjectType ArgparseType;
}

#endif
