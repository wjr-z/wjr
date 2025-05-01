#include <wjr/biginteger/detail/precompute-chars-convert.hpp>

namespace wjr {

namespace {

inline constexpr std::array<uint64_t, 37> precompute_chars_convert_big_base = {
    UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
    UINT64_C(0xa8b8b452291fe821), UINT64_C(0x0000000000000000), UINT64_C(0x6765c793fa10079d),
    UINT64_C(0x41c21cb8e1000000), UINT64_C(0x3642798750226111), UINT64_C(0x0000000000000000),
    UINT64_C(0xa8b8b452291fe821), UINT64_C(0x8ac7230489e80000), UINT64_C(0x4d28cb56c33fa539),
    UINT64_C(0x1eca170c00000000), UINT64_C(0x780c7372621bd74d), UINT64_C(0x1e39a5057d810000),
    UINT64_C(0x5b27ac993df97701), UINT64_C(0x0000000000000000), UINT64_C(0x27b95e997e21d9f1),
    UINT64_C(0x5da0e1e53c5c8000), UINT64_C(0xd2ae3299c1c4aedb), UINT64_C(0x16bcc41e90000000),
    UINT64_C(0x2d04b7fdd9c0ef49), UINT64_C(0x5658597bcaa24000), UINT64_C(0xa0e2073737609371),
    UINT64_C(0x0c29e98000000000), UINT64_C(0x14adf4b7320334b9), UINT64_C(0x226ed36478bfa000),
    UINT64_C(0x383d9170b85ff80b), UINT64_C(0x5a3c23e39c000000), UINT64_C(0x8e65137388122bcd),
    UINT64_C(0xdd41bb36d259e000), UINT64_C(0x0aee5720ee830681), UINT64_C(0x0000000000000000),
    UINT64_C(0x172588ad4f5f0981), UINT64_C(0x211e44f7d02c1000), UINT64_C(0x2ee56725f06e5c71),
    UINT64_C(0x41c21cb8e1000000)};

inline constexpr uint64_t precompute_chars_convert_storage[] = {
    UINT64_C(0xd3ad0eeba1000000), UINT64_C(0x3e21f7954fe4a741), UINT64_C(0x5f1e32bffbdc5d1c),
    UINT64_C(0xd2d8af57d5d929cb), UINT64_C(0xa2fd64b0ccbf84ba), UINT64_C(0x01e8ca3185deb719),
    UINT64_C(0xfb41000000000000), UINT64_C(0x6174834b58bc387c), UINT64_C(0x50c038ea88265f88),
    UINT64_C(0x355e3bed6d2e729c), UINT64_C(0xa879bdd799c4488f), UINT64_C(0x51bacab484a08216),
    UINT64_C(0xc9f326d45cc68e49), UINT64_C(0x44f2a6a7b2f7fcbd), UINT64_C(0xdae730af9e224c74),
    UINT64_C(0x81a5b7f53b009592), UINT64_C(0x0ff51f1ae0bbcca8), UINT64_C(0x0003a5437c8091f2),
    UINT64_C(0xcd90868100000000), UINT64_C(0xd995e880aedaa268), UINT64_C(0xb784b43da5a9aef0),
    UINT64_C(0xf263079a4188c2ba), UINT64_C(0x6a5c49b340a88ee5), UINT64_C(0x6d2d81b712a80dbf),
    UINT64_C(0x5ce6b1b1ca05ce6b), UINT64_C(0x0d965d5bf0075d6f), UINT64_C(0x2c26118c407f1c58),
    UINT64_C(0x1675edab10c46efd), UINT64_C(0x8707615cd6a6e601), UINT64_C(0x4c81799c5b0b2eda),
    UINT64_C(0xc2d8e74c447413e7), UINT64_C(0x4ec34714fec04aa7), UINT64_C(0xcd8318510a8695cf),
    UINT64_C(0xe84cc7cba6a67297), UINT64_C(0x2c901fa7302978de), UINT64_C(0xdf9990cc0a0d3514),
    UINT64_C(0x6b3dd5319c42ea3d), UINT64_C(0xe35f08baa089c0df), UINT64_C(0x8251f105062f7a80),
    UINT64_C(0x8fa79af9d3c1b861), UINT64_C(0x0000000d4a44fb4b), UINT64_C(0x046d55d231cb4d01),
    UINT64_C(0x191229064171b10f), UINT64_C(0x9cdb68c18e83e312), UINT64_C(0xb7ffc0b789b8ffcd),
    UINT64_C(0x4b958b6005d0a970), UINT64_C(0xae152d3e1eec74f8), UINT64_C(0xd5c50c5105936cf5),
    UINT64_C(0x41c09569a9dc6a06), UINT64_C(0x34a827a71a50e044), UINT64_C(0x1feebc831fdb6805),
    UINT64_C(0xb493b83136896f86), UINT64_C(0x2b1aabb1b9d9f3ca), UINT64_C(0x2d6623c263cebead),
    UINT64_C(0x814ed646ca672ca8), UINT64_C(0x59abbe2fb1917181), UINT64_C(0xb3ce02962639a988),
    UINT64_C(0x4d9474a4219e8f86), UINT64_C(0x51e6550edf8123ac), UINT64_C(0xd879fab0b1ffccf3),
    UINT64_C(0xb1300de91861b435), UINT64_C(0x0dad44c90ce8e477), UINT64_C(0x8720815d8820820f),
    UINT64_C(0x50b4286a20faf8d6), UINT64_C(0x93aa2e3900ca40ee), UINT64_C(0x2e5e61dab8db012b),
    UINT64_C(0x2ee4e367e55cba4a), UINT64_C(0x6b445244c4cab346), UINT64_C(0x93f17d6346f9254c),
    UINT64_C(0x25f844c78c978751), UINT64_C(0xd147eedf87c0baaf), UINT64_C(0xd35cc4e8b75c1abd),
    UINT64_C(0xeb2fa6dcd54f781f), UINT64_C(0xfec491695418bd92), UINT64_C(0x797549042207179f),
    UINT64_C(0x64d0f79da8533ed6), UINT64_C(0xe74d98982218e578), UINT64_C(0xe3828675170471eb),
    UINT64_C(0xcb2aefff2357870d), UINT64_C(0xf3e00e160cb6f63e), UINT64_C(0xf2d8d5c4338f1de5),
    UINT64_C(0xe55892ccddb68e25), UINT64_C(0xcc5ab963d91b0b38), UINT64_C(0x6b8897e82cde8d5e),
    UINT64_C(0xa08d798abce43602), UINT64_C(0x00000000000000b0), UINT64_C(0x6484821219e80000),
    UINT64_C(0xa39d74e60029edd9), UINT64_C(0xce3aa9e922ff55e0), UINT64_C(0x672d90d4b3578b20),
    UINT64_C(0x2022c19fe0e7c615), UINT64_C(0x70a13556184a8640), UINT64_C(0xd36be92632788a28),
    UINT64_C(0x6e5a62f87258da1a), UINT64_C(0xb02489c849c4c77d), UINT64_C(0xf8364a17c977e473),
    UINT64_C(0xcf6ae0c6ee7e5112), UINT64_C(0x091254e907124144), UINT64_C(0x5052d48e3f3ae73c),
    UINT64_C(0xd8cb2701c561f2e6), UINT64_C(0xef650841ea230095), UINT64_C(0x8581f0f68e7c03c8),
    UINT64_C(0xa4709f23da0a33f7), UINT64_C(0x841bd1fb80cc83f7), UINT64_C(0x21a9b9077cd0d173),
    UINT64_C(0xf1611bb6d87ccf01), UINT64_C(0xc358bce53264990b), UINT64_C(0x0fb509d62743bd63),
    UINT64_C(0xa9c3395900bf394d), UINT64_C(0x5170ea7bdb523a7e), UINT64_C(0x1db88e22f11129db),
    UINT64_C(0xc8738364d76d3c4e), UINT64_C(0x5d4d99872bfc8282), UINT64_C(0x83a93b45fc7c3ab2),
    UINT64_C(0x47f2d64ae5d476c1), UINT64_C(0xe690d26a4288d09a), UINT64_C(0xd1daea030c2fbc46),
    UINT64_C(0xbb5090defdd89b31), UINT64_C(0xb7e095d673f2ed99), UINT64_C(0xbbf1692bb8286d9d),
    UINT64_C(0x6a075f0e1b7fa056), UINT64_C(0xe623fa562dcffc0a), UINT64_C(0x787a15864d720dcb),
    UINT64_C(0x9c5e8f9e05e2b98e), UINT64_C(0x50c26fc5fc7e8357), UINT64_C(0xd5faa27398c2d209),
    UINT64_C(0x386656ee35feb2a0), UINT64_C(0xd608af79ab8a8015), UINT64_C(0x20ec11ae52ce1a45),
    UINT64_C(0x9f3353e52e804083), UINT64_C(0x140c4b608b7ba74b), UINT64_C(0x5525e79f7564d1de),
    UINT64_C(0x4c6541cfe8c6a5ab), UINT64_C(0x406d7dabaa6cffcd), UINT64_C(0x09ba310509b26c12),
    UINT64_C(0xd863366249001015), UINT64_C(0xdd645df74c7cbc03), UINT64_C(0xc381f965c89f1bf9),
    UINT64_C(0x615ea8e43836484e), UINT64_C(0xffd8bc0e4b4812dc), UINT64_C(0x58747ad4cc506213),
    UINT64_C(0x7017a13af089f2c6), UINT64_C(0xdc232ff80e3abeff), UINT64_C(0x4c36cdc20a2bbc03),
    UINT64_C(0x9b81840296df7d5f), UINT64_C(0x187b7f63f6037574), UINT64_C(0x152a572ee7940b5f),
    UINT64_C(0x9a136eca32891701), UINT64_C(0x8d2a913f72ca11ca), UINT64_C(0xcde589746ef5950a),
    UINT64_C(0x4e776cb47e5fa2ac), UINT64_C(0x6f662a54258dbbd1), UINT64_C(0x311821f5874f247d),
    UINT64_C(0x76573a90e46dc0fc), UINT64_C(0x1e276c7ffe3adc39), UINT64_C(0x73e8a14346e15d96),
    UINT64_C(0x25b8e5a7c5a84465), UINT64_C(0x21c66d84c6912974), UINT64_C(0x74f883b60d783bed),
    UINT64_C(0xb2ca908c59b506f8), UINT64_C(0xde5a057932789b0b), UINT64_C(0xe3dfcd53fe36abc0),
    UINT64_C(0x1f7993c0c223e80d), UINT64_C(0xa3c7af13eb754320), UINT64_C(0x2a6c69276dbb8620),
    UINT64_C(0xbc1cbf624c2fee8f), UINT64_C(0x95be954167c832a3), UINT64_C(0xf939e0baaa798a3c),
    UINT64_C(0x077b4cee4c7bb1ea), UINT64_C(0x7412c0c6091477fe), UINT64_C(0x3d47d4dd36194788),
    UINT64_C(0x6825887efd1c882f), UINT64_C(0x775767d79c521ed0), UINT64_C(0x0774b2e897af7a14),
    UINT64_C(0x02b9375586d86fff), UINT64_C(0x0000000000004210)};

struct precompute_chars_convert_info_t {
    uint32_t offset;
    uint32_t size;
    uint32_t shift;
    uint32_t digits_in_base;
};

precompute_chars_convert_info_t precompute_chars_convert_info[] = {
    {0, 6, 2, 152}, {6, 12, 4, 304}, {18, 23, 9, 608}, {41, 45, 19, 1216}, {86, 90, 38, 2451}};

struct precompute_chars_convert_header_t {
    uint32_t start;
    uint32_t end;
};

precompute_chars_convert_header_t precompute_chars_convert_header[36] = {
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 5}, {5, 5}, {5, 5},
    {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5},
    {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}};
} // namespace

precompute_chars_convert_t *precompute_chars_convert(precompute_chars_convert_t *pre_table,
                                                     size_t n, unsigned int base,
                                                     uint64_t *mem_table) noexcept {
    const auto set = [base](precompute_chars_convert_t *pre, const uint64_t *ptr, uint32_t size,
                            uint32_t shift, uint32_t digits_in_base) {
        *pre = {ptr, size, shift, digits_in_base, base};
    };

    // empty state
    set(pre_table, nullptr, 0, 0, 0);

    const precompute_chars_convert_info_t *pre_start;
    const precompute_chars_convert_info_t *pre_end;

    {
        auto header = precompute_chars_convert_header[base - 1];
        pre_start = precompute_chars_convert_info + header.start;
        pre_end = precompute_chars_convert_info + header.end;
    }

    if (n >= 8) {
        const uint64_t big_base = precompute_chars_convert_big_base[base];
        const uint32_t digits_in_one_base = precompute_chars_convert_digits_in_one_base[base];

        uint32_t digits = 0;
        uint32_t shift = 0;
        uint32_t digits_in_base = 0;

        WJR_CHECK(pre_start != pre_end);

        do {
            if (pre_start != pre_end) {
                digits = pre_start->size;
                shift = pre_start->shift;
                digits_in_base = pre_start->digits_in_base;

                WJR_CHECK(n >= digits + shift);

                ++pre_table;
                set(pre_table, precompute_chars_convert_storage + pre_start->offset, digits, shift,
                    digits_in_base);
                ++pre_start;
                continue;
            }

            sqr(mem_table, pre_table->ptr, digits);
            digits <<= 1;
            shift <<= 1;
            digits_in_base <<= 1;

            auto is_zero = static_cast<int>(mem_table[0] == 0);
            mem_table += is_zero;
            digits -= is_zero;
            shift += is_zero;

            if (WJR_UNLIKELY(mem_table[digits - 1] == 0)) {
                mem_table[digits - 1] = mul_1(mem_table, mem_table, digits - 1, big_base);
                digits_in_base += digits_in_one_base;

                is_zero = mem_table[0] == 0;
                mem_table += is_zero;
                digits -= is_zero;
                shift += is_zero;
            }

            ++pre_table;
            set(pre_table, mem_table, digits, shift, digits_in_base);
            mem_table += digits;
        } while (n * 2 > (digits + shift) * 5);
    }

    return pre_table;
}

} // namespace wjr