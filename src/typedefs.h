#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

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
