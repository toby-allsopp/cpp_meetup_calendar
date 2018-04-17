# Ranges Part 2: range-v3 and standardisation

## After Boost Range there was...

## range-v3

* https://github.com/ericniebler/range-v3
* Very similar to Boost Range (Eric Niebler invovled in both)
* Includes a C++11-compatible emulation of Concepts
* Currently works ONLY on GCC and Clang
    * There is an out-of-date port to MSVC but it will confuse you because the API has changed
* Parts proposed for eventual inclusion in the C++ standard

## range-v3 example

## Standardisation

### Ranges TS

* Working Draft: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4685.pdf
* Nice (although incomplete) documentation: http://en.cppreference.com/w/cpp/experimental/ranges
* Contains
    * Concepts
        * `Destructible`, `Copyable`, `Assignable`, etc.
        * `Iterator`, `InputIterator`, `Range`, `View`, etc.
    * Range algorithms
        * `vector<int> v{3,2,1}; sort(v);`
* Does *NOT* contain
    * Views and Range Adaptors
    * `operator|` overloads
* Very likely to be merged into the IS in time for C++20

### Range Adaptors and Utilities Proposal

* Current revision: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0789r2.pdf
* Contains
    * (Some) range adaptors
        * `view::filter`, `view::transform`, etc.
    * Chaining using `|`
* Not sure how likely to get into C++20