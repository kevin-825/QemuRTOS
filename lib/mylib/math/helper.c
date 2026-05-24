/* lib/div64.c - minimal unsigned 64-bit divide/mod helpers for RV32 */
typedef unsigned long long u64;

u64 __udivdi3(u64 a, u64 b)
{
    if (b == 0) return (u64)-1;
    u64 q = 0, r = 0;
    for (int i = 63; i >= 0; --i) {
        r = (r << 1) | ((a >> i) & 1ULL);
        if (r >= b) {
            r -= b;
            q |= (1ULL << i);
        }
    }
    return q;
}

u64 __umoddi3(u64 a, u64 b)
{
    if (b == 0) return (u64)-1;
    u64 q = __udivdi3(a, b);
    return a - q * b;
}
