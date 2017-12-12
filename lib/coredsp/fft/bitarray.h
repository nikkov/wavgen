#if !defined HAVE_BITARRAY_H__
#define      HAVE_BITARRAY_H__

#define BITS_PER_LONG	( sizeof(long) * 8 )

//#define NO_BITS_USE_ASM
//

#define  DIVMOD(n, d, bm) \
ulong d = n / BITS_PER_LONG; \
ulong bm = 1UL << (n % BITS_PER_LONG);

#define  DIVMOD_TEST(n, d, bm) \
ulong d = n / BITS_PER_LONG; \
ulong bm = 1UL << (n % BITS_PER_LONG); \
ulong t = bm & f_[d];


class bitarray
//
// bitarray class mostly for use as
// memory saving array of boolean values
// valid index is 0...nb_-1 (as usual in C arrays)
//
{
public:
    ulong *f_;   // bit bucket
    ulong n_;    // number of bits
    ulong nfw_;  // number of words where all bits are used, may be zero
    ulong mp_;   // mask for partially used word if there is one, else zero
    // (ones are at the positions of the _unused_ bits)

public:
    bitarray(ulong nbits)
    {
        n_ = ( 0==nbits ? 1 : nbits );  // avoid trouble for zero length

        // nw_: number of words (incl. partially used), nw>=1
        ulong nw = n_ / BITS_PER_LONG;       // number of words

        // nbl_: number of bits used in last (partially used) word, 0 if mw==mfw
        ulong nbl = n_ - nw*BITS_PER_LONG;  // number of bits used in last word
        nfw_ = nw;  // number of fully used words

        if ( 0!=nbl )  // there is a partially used word
        {
            ++nw;  // increase total number of words
            mp_ = (~0UL) >> (BITS_PER_LONG-nbl);  // correct mask for last word
        }
        else  mp_ = 0UL;

        f_ = new ulong[nw];  // finally allocate
    }


    ~bitarray()  { delete [] f_; }


    ulong test(ulong n)  const
    // Test whether n-th bit set.
    {
        DIVMOD_TEST(n, d, bm);
        return  t;
    }


    void set(ulong n)
    // Set n-th bit.
    {
        DIVMOD(n, d, bm);
        f_[d] |= bm;
    }


    void clear(ulong n)
    // Clear n-th bit.
    {
        DIVMOD(n, d, bm);
        f_[d] &= ~bm;
    }

    void change(ulong n)
    // Toggle n-th bit.
    {
        DIVMOD(n, d, bm);
        f_[d] ^= bm;
    }


    ulong test_set(ulong n)
    // Test whether n-th bit is set and set it.
    {
        DIVMOD_TEST(n, d, bm);
        f_[d] |= bm;
        return  t;
    }


    ulong test_clear(ulong n)
    // Test whether n-th bit is set and clear it.
    {
        DIVMOD_TEST(n, d, bm);
        f_[d] &= ~bm;
        return  t;
    }


    ulong test_change(ulong n)
    // Test whether n-th bit is set and toggle it.
    {
        DIVMOD_TEST(n, d, bm);
        f_[d] ^= bm;
        return  t;
    }


    void clear_all()
    // Clear all bits.
    {
        for (ulong k=0; k<nfw_; ++k)  f_[k] = 0;
        if ( mp_ )  f_[nfw_] = 0;
    }

    void set_all()
    // Set all bits.
    {
        for (ulong k=0; k<nfw_; ++k)  f_[k] = ~0UL;
        if ( mp_ )  f_[nfw_] = ~0UL;
    }


    ulong all_set_q()  const
    // Return whether all bits are set.
    {
        for (ulong k=0; k<nfw_; ++k)  if ( ~f_[k] )  return 0;
        if ( mp_ )
        {
            ulong z = f_[nfw_] & mp_;
            if ( z!=mp_ )  return 0;
        }
        return  1;
    }

    ulong all_clear_q()  const
    // Return whether all bits are clear
    {
        for (ulong k=0; k<nfw_; ++k)  if ( f_[k] )  return 0;
        if ( mp_ )
        {
            ulong z = f_[nfw_] & mp_;
            if ( z!=0 )  return 0;
        }
        return  1;
    }

    ulong next_set(ulong n)  const
    // Return index of next set or value beyond end.
    // Note: the given index n is included in the search
    {
        while ( (n<n_) && (!test(n)) )  ++n;
        return  n;
    }

    ulong next_clear(ulong n)  const
    // Return index of next clear or value beyond end.
    // Note: the given index n is included in the search
    {
        while ( (n<n_) && (test(n)) )  ++n;
        return  n;
    }

    // todo: next_set_sparse() / next_clear_sparse()


    void dump()  const;
    void dump_bits(ulong prettyq=0)  const;
};
// -------------------------

#undef  DIVMOD
#undef  DIVMOD_TEST

//inline bitarray & operator ^= (bitarray &t, const bitarray &h)
//// no check for compatible lengths (!)
//{
//    ulong nfw = t.nfw_;
//    for (ulong k=0; k<nfw; ++k)  t.f_[k] ^= h.f_[k];
//    if ( t.mp_ )  t.f_[nfw] ^= h.f_[nfw];
//    return  t;
//}
//// -------------------------

#endif // !defined HAVE_BITARRAY_H__
