#ifndef HANDLE_HPP
#define HANDLE_HPP

#include <cstdio>

namespace sanya {

// Forwarding for Handle.
class RawObject;
class RawFixnum;
class RawNil;
class RawPair;
class RawSymbol;
class RawVector;
class RawGrowableVector;
class RawDict;

/**
 * Copied from Google Dart's code -- this will slightly affect
 * the pointer access efficiency but since everything is inlined, it's ok.
 * But GC will be very happy...
 */
class Handle {
    friend class Heap;
    friend class RootSet;
    friend class DummyObjectHead;
public:
    /** @brief Remove itself and unlink from gc rootset */
    inline ~Handle();

    /** @brief Construct from newly-allocated raw object */
    inline Handle(RawObject *ro);

    /** @brief Copy constructor from another object */
    inline Handle(const Handle &o);

    /** @brief Raw pointer accessor */
    inline RawObject *raw() const;

    /** @brief Assignment constructor */
    inline void set_raw(RawObject *);

    /** @brief Assignment mutator */
    inline Handle &operator=(const Handle &o);

    /** @see set_raw */
    inline Handle &operator=(RawObject *ro);

    inline RawObject &AsObject() const;
    inline RawPair &AsPair() const;
    inline RawFixnum &AsFixnum() const;
    inline RawSymbol &AsSymbol() const;
    inline RawGrowableVector &AsGrowableVector() const;
    inline RawVector &AsVector() const;
    inline RawDict &AsDict() const;

    void print_info() {
        printf("raw = %p, prev_root = %p, next_root = %p\n",
                raw_, prev_root_, next_root_);
    }

private:
    inline Handle();

    inline void Empty();

    inline void LinkToRootSet(RawObject *ro);

    inline void UnlinkFromRootSet();

    /** @brief The raw object pointer, may be changed during GC */
    RawObject *raw_;

    /** @brief Previous object in the GC root */
    Handle *prev_root_;

    /** @brief Next object in the GC root */
    Handle *next_root_;
};

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:


#endif /* HANDLE_HPP */
