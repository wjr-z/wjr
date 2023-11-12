#ifndef WJR_CONTAINER_LLIST_HPP__
#define WJR_CONTAINER_LLIST_HPP__
#include <stdatomic.h>

namespace wjr {

    struct llist {
        llist* next;
    };

}

#endif // ! WJR_CONTAINER_LLIST_HPP__