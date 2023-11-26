// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2021 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <evmc/evmc.hpp>
#include <intx/intx.hpp>
#include <cassert>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace evmone::rlp
{
using bytes = std::basic_string<uint8_t>;
using bytes_view = std::basic_string_view<uint8_t>;

namespace internal
{
template <uint8_t ShortBase, uint8_t LongBase>
inline bytes encode_length(size_t l)
{
    static constexpr uint8_t short_cutoff = 55;
    static_assert(ShortBase + short_cutoff <= 0xff);
    assert(l <= 0xffffff);

    if (l <= short_cutoff)
        return {static_cast<uint8_t>(ShortBase + l)};
    else if (const auto l0 = static_cast<uint8_t>(l); l <= 0xff)
        return {LongBase + 1, l0};
    else if (const auto l1 = static_cast<uint8_t>(l >> 8); l <= 0xffff)
        return {LongBase + 2, l1, l0};
    else
        return {LongBase + 3, static_cast<uint8_t>(l >> 16), l1, l0};
}

inline bytes wrap_list(const bytes& content)
{
    return internal::encode_length<192, 247>(content.size()) + content;
}

template <typename InputIterator>
inline bytes encode_container(InputIterator begin, InputIterator end);
}  // namespace internal

inline bytes_view trim(bytes_view b) noexcept
{
    b.remove_prefix(std::min(b.find_first_not_of(uint8_t{0x00}), b.size()));
    return b;
}

template <typename T>
inline decltype(rlp_encode(std::declval<T>())) encode(const T& v)
{
    return rlp_encode(v);
}

inline bytes encode(bytes_view data)
{
    static constexpr uint8_t short_base = 128;
    if (data.size() == 1 && data[0] < short_base)
        return {data[0]};

    return internal::encode_length<short_base, 183>(data.size()) += data;  // Op + not available.
}

inline bytes encode(uint64_t x)
{
    uint8_t b[sizeof(x)];
    intx::be::store(b, x);
    return encode(trim({b, sizeof(b)}));
}

inline bytes encode(const intx::uint256& x)
{
    uint8_t b[sizeof(x)];
    intx::be::store(b, x);
    return encode(trim({b, sizeof(b)}));
}

template <typename T>
inline bytes encode(const std::vector<T>& v)
{
    return internal::encode_container(v.begin(), v.end());
}

template <typename T, size_t N>
inline bytes encode(const T (&v)[N])
{
    return internal::encode_container(std::begin(v), std::end(v));
}

/// Encodes the fixed-size collection of heterogeneous values as RLP list.
template <typename... Types>
inline bytes encode_tuple(const Types&... elements)
{
    return internal::wrap_list((encode(elements) + ...));
}

/// Encodes a pair of values as RPL list.
template <typename T1, typename T2>
inline bytes encode(const std::pair<T1, T2>& p)
{
    return encode_tuple(p.first, p.second);
}

/// Encodes the container as RLP list.
///
/// @tparam InputIterator  Type of the input iterator.
/// @param  begin          Begin iterator.
/// @param  end            End iterator.
/// @return                Bytes of the RLP list.
template <typename InputIterator>
inline bytes internal::encode_container(InputIterator begin, InputIterator end)
{
    bytes content;
    for (auto it = begin; it != end; ++it)
        content += encode(*it);
    return wrap_list(content);
}

template <class T>
concept UnsignedIntegral =
    std::unsigned_integral<T> || std::same_as<T, intx::uint128> || std::same_as<T, intx::uint256> ||
    std::same_as<T, intx::uint512> || std::same_as<T, intx::uint<2048>>;

// Load unsigned integral from bytes_view. The destination size must not be smaller than input data.
template <UnsignedIntegral T>
[[nodiscard]] inline T load(bytes_view input)
{
    if (input.size() > sizeof(T))
        throw std::runtime_error("load: input too big");

    T x{};
    std::memcpy(&intx::as_bytes(x)[sizeof(T) - input.size()], input.data(), input.size());
    x = intx::to_big_endian(x);
    return x;
}

// RLP decoding implementation based on
// https://ethereum.org/en/developers/docs/data-structures-and-encoding/rlp/#definition
// and https://github.com/torquem-ch/silkworm/blob/master/silkworm/core/rlp/decode.hpp
template <typename T>
inline void decode(bytes_view& input, T& to)
{
    rlp_decode(input, to);
}

struct Header
{
    uint64_t payload_length = 0;
    bool is_list = false;
};

[[nodiscard]] Header decode_header(bytes_view& input);

template <UnsignedIntegral T>
void decode(bytes_view& from, T& to)
{
    constexpr auto to_size = sizeof(T);
    const auto h = decode_header(from);

    if (h.is_list)
        throw std::runtime_error("rlp decoding error: unexpected list type");

    if (to_size < h.payload_length)
        throw std::runtime_error("rlp decoding error: unexpected type");

    to = load<T>(from.substr(0, static_cast<size_t>(h.payload_length)));
    from.remove_prefix(static_cast<size_t>(h.payload_length));
}

void decode(bytes_view& from, bytes& to);
void decode(bytes_view& from, evmc::bytes32& to);
void decode(bytes_view& from, evmc::address& to);

template <size_t N>
void decode(bytes_view& from, std::span<uint8_t, N> to)
{
    const auto h = decode_header(from);

    if (h.is_list)
        throw std::runtime_error("rlp decoding error: unexpected list type");

    if (to.size() < h.payload_length)
        throw std::runtime_error("rlp decoding error: payload too big");

    auto d = to.size() - h.payload_length;
    std::memcpy(to.data() + d, from.data(), static_cast<size_t>(h.payload_length));
    from.remove_prefix(static_cast<size_t>(h.payload_length));
}

template <size_t N>
void decode(bytes_view& from, uint8_t (&to)[N])
{
    decode(from, std::span<uint8_t, N>(to));
}

template <typename T1, typename T2>
void decode(bytes_view& from, std::pair<T1, T2>& p);

template <typename T>
void decode(bytes_view& from, std::vector<T>& to)
{
    const auto h = decode_header(from);

    if (!h.is_list)
        throw std::runtime_error("rlp decoding error: unexpected type. list expected");

    auto payload_view = from.substr(0, static_cast<size_t>(h.payload_length));

    while (!payload_view.empty())
    {
        to.emplace_back();
        decode(payload_view, to.back());
    }

    from.remove_prefix(static_cast<size_t>(h.payload_length));
}

template <typename T1, typename T2>
void decode(bytes_view& from, std::pair<T1, T2>& p)
{
    const auto h = decode_header(from);

    if (!h.is_list)
        throw std::runtime_error("rlp decoding error: unexpected type. list expected");

    decode(from, p.first);
    decode(from, p.second);
}

}  // namespace evmone::rlp
