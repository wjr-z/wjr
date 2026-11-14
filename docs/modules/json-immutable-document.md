# Owning immutable JSON documents

Include `<wjr/json/immutable_document.hpp>` and link `wjr::json`. Parse a complete
JSON value into an input-independent owner, then borrow views for repeated reads.

```cpp
wjr::json::ondemand_reader reader;
reader.read(input);
auto parsed = wjr::json::immutable_document::parse(reader);
if (!parsed) {
    return;
}
const auto object = parsed->root().get_object();
if (object) {
    const auto name = object->find("name");
    if (name) {
        const auto text = name->get_string();
        if (text) {
            consume(*text);
        }
    }
}
```

The move-only owner retains one allocation containing 16-byte value records,
12-byte object members and decoded string bytes on the supported 64-bit targets.
`storage_size()` reports the requested size of this allocation. Parsing uses
temporary growable storage and briefly holds both the builder and packed output.

## Ownership and access

The input and reader may be destroyed or reused after a successful parse. Keys
and string values own their decoded bytes, including embedded NUL. Returned
`string_view` objects have a length and carry no terminating-NUL guarantee.

`root()` requires an lvalue owner holding a successfully parsed value. Every
value, container, iterator and string view borrows that owner object. Destruction,
replacement or moving the owner invalidates these borrowed objects. Obtain new
views from the destination after a move. A moved-from owner can be destroyed or
assigned, and its `storage_size()` is zero.

Array and object iterators satisfy `std::forward_iterator` and return small views
by value. Arrays preserve input order and skip complete nested subtrees during
iteration. `array_view::at(index)` walks preceding siblings, returning
`INDEX_OUT_OF_BOUNDS` for an invalid index. Objects iterate decoded keys in byte
lexicographic order. `object_view::find(key)` uses binary search and returns
`NO_SUCH_FIELD` for an absent key.

Scalar getters check the exact stored kind and return `INCORRECT_TYPE` on a
mismatch. Nonnegative integral tokens fitting `uint64_t` use `get_unsigned()`.
Negative integral tokens fitting `int64_t` use `get_signed()`. Decimal or exponent
syntax uses `get_float()`, preserving floating negative zero. The shared numeric
parser represents larger finite integers as double, with its ordinary rounding.
Floating overflow or underflow reports `BIGINT_ERROR`. Getter calls perform no
numeric conversions.

## Validation and failure

Parsing consumes one complete JSON value using the existing lexer, visitor,
number parser and string decoder. The checked entry additionally validates raw
UTF-8, unescaped control bytes, four JSON whitespace characters and token
boundaries. Duplicate decoded keys use the final value. Every superseded value
is fully validated. Packed storage excludes superseded subtrees and bytes.

Syntax failures return `parse_error`. Its offset is zero-based in the original
input. Invalid UTF-8 points at the malformed sequence start. String-control and
escape failures point at the opening quote. Callback failures identify the
current token or detected boundary, and grammar failures identify the observed
structural token or end of input. Input size and internal indexes are limited to
`uint32_t`, with nesting limited by the shared visitor. Allocation failures
propagate as exceptions and release partially built state.

The existing mutable DOM and `check` entry have older validation behavior.
Performance comparisons use common valid inputs and explicitly retain this
validation difference in their interpretation.

## Consumer and tradeoffs

The [snapshot example](../../examples/json/immutable_snapshots.cpp) loads Twitter
data, destroys its input and reader, traverses statuses and projects decoded
text and user names into independently owning application records.

This representation targets compact retained snapshots and repeated field
lookup. Numeric arrays retain essentially the same per-value size as the
mutable DOM. Packing adds traversal and peak memory. Many overwritten large
values can produce a high transient peak despite a very small final snapshot.
The [performance report](../performance/json-immutable-document.md) compares
complete consumers and retained capacities against the mutable DOM and
simdjson 3.9.1 DOM.
