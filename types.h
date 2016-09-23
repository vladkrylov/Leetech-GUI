#ifndef TYPES_H
#define TYPES_H

#define UNKNOWN -1

typedef enum {
    COLL_BOTTOM,
    COLL_TOP,
    COLL_RIGHT,
    COLL_LEFT,
    N_COLLIMATORS
} collimator_t;

typedef enum {
    BOX_ENTRANCE,
    BOX_EXIT1,
    BOX_EXIT2,
    N_SETS
} collbox_t;

#endif // TYPES_H

