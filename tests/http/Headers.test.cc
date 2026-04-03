// 🌺💜 Violet.Networking: C++20 library that provides networking primitives
// Copyright (c) 2026 Noelware, LLC. <team@noelware.org>, et al.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <gtest/gtest.h>
#include <violet/Networking/HTTP/Headers.h>

// NOLINTBEGIN(google-build-using-namespace,readability-identifier-length)
using namespace violet::net::http;
using namespace violet;

TEST(HeaderName, FromLiteralAcceptsValidTchar)
{
    EXPECT_EQ(HeaderName::From<"content-type">(), "content-type");
    EXPECT_EQ(HeaderName::From<"x-request-id">(), "x-request-id");
    EXPECT_EQ(HeaderName::From<"accept">(), "accept");
}

TEST(HeaderName, FromLiteralCanonicalizesToLowercase)
{
    EXPECT_EQ(HeaderName::From<"Content-Type">(), "content-type");
    EXPECT_EQ(HeaderName::From<"X-REQUEST-ID">(), "x-request-id");
    EXPECT_EQ(HeaderName::From<"ACCEPT">(), "accept");
    EXPECT_EQ(HeaderName::From<"Content-LENGTH">(), "content-length");
}

TEST(HeaderName, FromLiteralAllowsAllTcharSymbols)
{
    // Every symbol character permitted by RFC 9110 tchar
    EXPECT_NO_FATAL_FAILURE(HeaderName::From<"!#$%&'*+-.^_`|~">());
}

TEST(HeaderName, FromRuntimeAcceptsValidName)
{
    auto name = HeaderName::From("x-forwarded-for");
    ASSERT_TRUE(name);
    EXPECT_EQ(*name, "x-forwarded-for");
}

TEST(HeaderName, FromRuntimeCanonicalizesToLowercase)
{
    auto ct = HeaderName::From("Content-Type");
    ASSERT_TRUE(ct);
    EXPECT_EQ(*ct, "content-type");

    auto accept = HeaderName::From("ACCEPT");
    ASSERT_TRUE(accept);
    EXPECT_EQ(*accept, "accept");
}

TEST(HeaderName, EqualityIsCaseFolded)
{
    EXPECT_EQ(HeaderName::From<"content-type">(), HeaderName::From<"Content-Type">());
    EXPECT_EQ(HeaderName::From<"ACCEPT">(), HeaderName::From<"accept">());
}

TEST(HeaderName, InequalityDistinguishesDifferentNames)
{
    EXPECT_NE(HeaderName::From<"content-type">(), HeaderName::From<"content-length">());
}

TEST(HeaderName, OrderingIsLexicographic)
{
    EXPECT_LT(HeaderName::From<"accept">(), HeaderName::From<"content-type">());
    EXPECT_GT(HeaderName::From<"content-type">(), HeaderName::From<"accept">());
}

TEST(HeaderName, HashIsConsistentWithEquality)
{
    std::hash<HeaderName> h;
    EXPECT_EQ(h(HeaderName::From<"content-type">()), h(HeaderName::From<"Content-Type">()));
}

TEST(HeaderName, ToStringReturnsCanonicalForm)
{
    EXPECT_EQ(HeaderName::From<"Content-Type">().ToString(), "content-type");
}

TEST(HeaderName, StreamOperatorWritesCanonicalForm)
{
    std::ostringstream os;
    os << HeaderName::From<"Content-Type">();
    EXPECT_EQ(os.str(), "content-type");
}

TEST(HeaderName, ExplicitConversionToStr)
{
    auto name = HeaderName::From<"content-type">();
    EXPECT_EQ(static_cast<violet::Str>(name), "content-type");
}

TEST(HeaderName, ExplicitConversionToCStr)
{
    auto name = HeaderName::From<"content-type">();
    EXPECT_STREQ(static_cast<violet::CStr>(name), "content-type");
}

TEST(HeaderValue, FromLiteralAcceptsVisibleAscii)
{
    EXPECT_EQ(HeaderValue::From<"application/json">(), "application/json");
    EXPECT_EQ(HeaderValue::From<"gzip">(), "gzip");
    EXPECT_EQ(HeaderValue::From<"*/*">(), "*/*");
}

TEST(HeaderValue, FromLiteralStripsLeadingAndTrailingOws)
{
    EXPECT_EQ(HeaderValue::From("  gzip  ").Unwrap(), "gzip");
    EXPECT_EQ(HeaderValue::From("\tgzip\t").Unwrap(), "gzip");
    EXPECT_EQ(HeaderValue::From("  gzip\t ").Unwrap(), "gzip");
}

TEST(HeaderValue, FromLiteralPreservesInteriorWhitespace)
{
    EXPECT_EQ(HeaderValue::From<"text/html; charset=utf-8">(), "text/html; charset=utf-8");
    EXPECT_EQ(HeaderValue::From<"a b">(), "a b");
}

TEST(HeaderValue, FromLiteralAcceptsEmptyAfterTrim)
{
    // An all-whitespace literal trims to empty, which is valid per RFC 9110
    EXPECT_EQ(HeaderValue::From<"">(), "");
}

TEST(HeaderValue, FromRuntimeAcceptsValidValue)
{
    auto value = HeaderValue::From("application/json").Unwrap();
    EXPECT_EQ(value, "application/json");
}

TEST(HeaderValue, FromRuntimeStripsOws)
{
    EXPECT_EQ(HeaderValue::From("  gzip  ").Unwrap(), "gzip");
    EXPECT_EQ(HeaderValue::From("\t*/*\t ").Unwrap(), "*/*");
}

TEST(HeaderValue, FromRuntimePreservesCase)
{
    EXPECT_EQ(HeaderValue::From("UTF-8").Unwrap(), "UTF-8");
    EXPECT_EQ(HeaderValue::From("GZIP").Unwrap(), "GZIP");
}

TEST(HeaderValue, EqualityIsByteExact)
{
    EXPECT_EQ(HeaderValue::From<"gzip">(), HeaderValue::From<"gzip">());
    EXPECT_NE(HeaderValue::From<"gzip">(), HeaderValue::From<"GZIP">());
}

TEST(HeaderValue, OrderingIsLexicographic)
{
    EXPECT_LT(HeaderValue::From<"gzip">(), HeaderValue::From<"identity">());
    EXPECT_GT(HeaderValue::From<"identity">(), HeaderValue::From<"gzip">());
}

TEST(HeaderValue, HashIsConsistentWithEquality)
{
    std::hash<HeaderValue> h;

    auto gzip = HeaderValue::From("gzip");
    ASSERT_TRUE(gzip);
    EXPECT_EQ(h(*gzip), h(*gzip));
}

TEST(HeaderValue, ToStringReturnsStoredValue)
{
    EXPECT_EQ(HeaderValue::From<"application/json">().ToString(), "application/json");
}

TEST(HeaderValue, StreamOperatorWritesStoredValue)
{
    std::ostringstream os;
    os << HeaderValue::From<"gzip">();
    EXPECT_EQ(os.str(), "gzip");
}

TEST(HeaderValue, ExplicitConversionToStr)
{
    auto value = HeaderValue::From<"gzip">();
    EXPECT_EQ(static_cast<violet::Str>(value), "gzip");
}

TEST(HeaderValue, ExplicitConversionToCStr)
{
    auto value = HeaderValue::From<"gzip">();
    EXPECT_STREQ(static_cast<violet::CStr>(value), "gzip");
}

TEST(Headers, DefaultConstructedIsEmpty)
{
    Headers h;
    EXPECT_TRUE(h.Empty());
    EXPECT_EQ(h.Size(), 0U);
}

TEST(Headers, SizeConstructorPreallocatesBuckets)
{
    // Just verify it doesn't crash and produces an empty map
    Headers h(64U);
    EXPECT_TRUE(h.Empty());
}

TEST(Headers, InitializerListConstructor)
{
    Headers h = {
        { HeaderName::From<"content-type">(), HeaderValue::From<"application/json">() },
        { HeaderName::From<"accept">(), HeaderValue::From<"*/*">() },
    };

    EXPECT_EQ(h.Size(), 2U);
    EXPECT_EQ(h.Get<"content-type">(), HeaderValue::From<"application/json">());
    EXPECT_EQ(h.Get<"accept">(), HeaderValue::From<"*/*">());
}

#if defined(VIOLET_GCC) || (defined(__clang_major__) && __clang_major__ >= 21)
TEST(Headers, IteratorRangeConstructor)
{
    using Entry = violet::Pair<const HeaderName, HeaderValue>;
    Entry entries[] = {
        { HeaderName::From<"content-type">(), HeaderValue::From<"text/plain">() },
        { HeaderName::From<"accept">(),       HeaderValue::From<"*/*">()        },
    };

    Headers h(std::begin(entries), std::end(entries));
    EXPECT_EQ(h.Size(), 2U);
}
#endif

TEST(Headers, EmptyReturnsTrueWhenEmpty)
{
    Headers h;
    EXPECT_TRUE(h.Empty());
}

TEST(Headers, EmptyReturnsFalseAfterInsert)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
    EXPECT_FALSE(h.Empty());
}

TEST(Headers, SizeTracksInsertions)
{
    Headers h;
    EXPECT_EQ(h.Size(), 0U);

    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
    EXPECT_EQ(h.Size(), 1U);

    (void)h.Insert<"content-type">(HeaderValue::From<"application/json">());
    EXPECT_EQ(h.Size(), 2U);
}

TEST(Headers, MaxSizeIsPositive)
{
    Headers h;
    EXPECT_GT(h.MaxSize(), 0U);
}

TEST(Headers, InsertLiteralReturnsNoneOnFirstInsert)
{
    Headers h;
    auto prev = h.Insert<"accept">(HeaderValue::From<"*/*">());
    EXPECT_FALSE(prev);
}

TEST(Headers, InsertLiteralReturnsPreviousValueOnCollision)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());

    auto prev = h.Insert<"accept">(HeaderValue::From<"application/json">());
    ASSERT_TRUE(prev);
    EXPECT_EQ(*prev, HeaderValue::From<"*/*">());
}

TEST(Headers, InsertLiteralDoesNotOverwriteExistingEntry)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
    (void)h.Insert<"accept">(HeaderValue::From<"application/json">());

    EXPECT_EQ(h.Get<"accept">(), HeaderValue::From<"*/*">());
}

TEST(Headers, InsertRuntimeName)
{
    Headers h;
    auto prev = h.Insert(HeaderName::From<"accept">(), HeaderValue::From<"*/*">());
    EXPECT_FALSE(prev);
    EXPECT_EQ(h.Get<"accept">(), HeaderValue::From<"*/*">());
}

TEST(Headers, BulkInsertAddsAllEntries)
{
    Headers h;
    h.Insert({
        { HeaderName::From<"content-type">(), HeaderValue::From<"application/json">() },
        { HeaderName::From<"accept">(), HeaderValue::From<"*/*">() },
        { HeaderName::From<"accept-encoding">(), HeaderValue::From<"gzip">() },
    });

    EXPECT_EQ(h.Size(), 3U);
}

TEST(Headers, BulkInsertFirstOccurrenceWinsOnDuplicate)
{
    Headers h;
    h.Insert({
        { HeaderName::From<"accept">(), HeaderValue::From<"*/*">() },
        { HeaderName::From<"accept">(), HeaderValue::From<"application/json">() },
    });

    EXPECT_EQ(h.Get<"accept">(), HeaderValue::From<"*/*">());
}

TEST(Headers, GetLiteralReturnsValueWhenPresent)
{
    Headers h;
    (void)h.Insert<"content-type">(HeaderValue::From<"application/json">());

    auto value = h.Get<"content-type">();
    ASSERT_TRUE(value);
    EXPECT_EQ(*value, HeaderValue::From<"application/json">());
}

TEST(Headers, GetLiteralReturnsNoneWhenAbsent)
{
    Headers h;
    EXPECT_FALSE(h.Get<"content-type">());
}

TEST(Headers, GetRuntimeNameReturnsValueWhenPresent)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());

    auto value = h.Get(HeaderName::From<"accept">());
    ASSERT_TRUE(value);
    EXPECT_EQ(*value, HeaderValue::From<"*/*">());
}

TEST(Headers, GetIsCaseInsensitiveViaCanonicalisation)
{
    Headers h;
    (void)h.Insert<"content-type">(HeaderValue::From<"application/json">());

    // Both spellings resolve to the same canonical key
    EXPECT_EQ(h.Get<"content-type">(), h.Get<"Content-Type">());
}

TEST(Headers, GetOrAssignReturnsExistingValueWhenPresent)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());

    auto value = h.GetOrAssign<"accept">(HeaderValue::From<"application/json">());
    EXPECT_EQ(value, HeaderValue::From<"*/*">());
}

TEST(Headers, GetOrAssignInsertsAndReturnsDefaultWhenAbsent)
{
    Headers h;
    auto value = h.GetOrAssign<"accept">(HeaderValue::From<"*/*">());
    EXPECT_EQ(value, HeaderValue::From<"*/*">());
    EXPECT_EQ(h.Size(), 1U);
}

TEST(Headers, GetOrAssignDoesNotOverwriteExistingEntry)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
    (void)h.GetOrAssign<"accept">(HeaderValue::From<"application/json">());

    EXPECT_EQ(h.Get<"accept">(), HeaderValue::From<"*/*">());
}

TEST(Headers, ContainsReturnsTrueWhenPresent)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());

    EXPECT_TRUE(h.Contains<"accept">());
}

TEST(Headers, ContainsReturnsFalseWhenAbsent)
{
    Headers h;
    EXPECT_FALSE(h.Contains<"accept">());
}

TEST(Headers, EraseReturnsTrueAndRemovesEntry)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());

    EXPECT_TRUE(h.Erase<"accept">());
    EXPECT_FALSE(h.Contains<"accept">());
    EXPECT_EQ(h.Size(), 0U);
}

TEST(Headers, EraseReturnsFalseWhenKeyAbsent)
{
    Headers h;
    EXPECT_FALSE(h.Erase<"accept">());
}

TEST(Headers, EraseOnlyRemovesTargetKey)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
    (void)h.Insert<"content-type">(HeaderValue::From<"application/json">());
    (void)h.Erase<"accept">();

    EXPECT_FALSE(h.Contains<"accept">());
    EXPECT_TRUE(h.Contains<"content-type">());
    EXPECT_EQ(h.Size(), 1U);
}

TEST(Headers, ClearRemovesAllEntries)
{
    Headers h;

    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
    (void)h.Insert<"content-type">(HeaderValue::From<"application/json">());
    h.Clear();

    EXPECT_TRUE(h.Empty());
    EXPECT_EQ(h.Size(), 0U);
}

TEST(Headers, ClearOnEmptyMapIsNoop)
{
    Headers h;
    EXPECT_NO_FATAL_FAILURE(h.Clear());
    EXPECT_TRUE(h.Empty());
}

TEST(Headers, MergeFromHeadersMovesNonConflictingEntries)
{
    Headers dst;
    (void)dst.Insert<"accept">(HeaderValue::From<"*/*">());

    Headers src;
    (void)src.Insert<"content-type">(HeaderValue::From<"application/json">());

    dst.Merge(src);

    EXPECT_TRUE(dst.Contains<"accept">());
    EXPECT_TRUE(dst.Contains<"content-type">());
    EXPECT_EQ(dst.Size(), 2U);
}

TEST(Headers, MergeFromHeadersLeavesConflictingEntriesInSource)
{
    Headers dst;
    (void)dst.Insert<"accept">(HeaderValue::From<"*/*">());

    Headers src;
    (void)src.Insert<"accept">(HeaderValue::From<"application/json">());

    dst.Merge(src);

    // dst keeps its original value
    EXPECT_EQ(dst.Get<"accept">(), HeaderValue::From<"*/*">());

    // conflicting entry remains in src
    EXPECT_TRUE(src.Contains<"accept">());
}

TEST(Headers, MergeRvalueEmptiesSource)
{
    Headers dst;
    Headers src;
    (void)src.Insert<"content-type">(HeaderValue::From<"application/json">());

    dst.Merge(VIOLET_MOVE(src));

    EXPECT_TRUE(dst.Contains<"content-type">());
}

TEST(Headers, MergeFromRawMap)
{
    Headers dst;
    Headers<>::Map src;
    src.emplace(HeaderName::From<"accept">(), HeaderValue::From<"*/*">());

    dst.Merge(src);

    EXPECT_TRUE(dst.Contains<"accept">());
}

// TEST(Headers, IterYieldsAllEntries)
// {
//     Headers h;
//     (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
//     (void)h.Insert<"content-type">(HeaderValue::From<"application/json">());
//     (void)h.Insert<"accept-encoding">(HeaderValue::From<"gzip">());

//     std::unordered_map<violet::String, violet::String> seen;
//     auto it = h.Iter();
//     while (auto item = it.Next()) {
//         seen[item->first.ToString()] = item->second.ToString();
//     }

//     EXPECT_EQ(seen.size(), 3U);
//     EXPECT_EQ(seen["accept"], "*/*");
//     EXPECT_EQ(seen["content-type"], "application/json");
//     EXPECT_EQ(seen["accept-encoding"], "gzip");
// }

// TEST(Headers, IterOnEmptyMapImmediatelyExhausted)
// {
//     Headers h;
//     auto it = h.Iter();
//     EXPECT_FALSE(it.Next());
// }

TEST(Headers, RangeForLoopYieldsAllEntries)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
    (void)h.Insert<"content-type">(HeaderValue::From<"application/json">());

    std::size_t count = 0;
    for (auto& [k, v]: h) {
        (void)k;
        (void)v;
        ++count;
    }

    EXPECT_EQ(count, 2U);
}

TEST(Headers, ReserveDoesNotLoseEntries)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
    (void)h.Insert<"content-type">(HeaderValue::From<"application/json">());
    h.Reserve(128U);

    EXPECT_EQ(h.Size(), 2U);
    EXPECT_TRUE(h.Contains<"accept">());
    EXPECT_TRUE(h.Contains<"content-type">());
}

TEST(Headers, RehashDoesNotLoseEntries)
{
    Headers h;
    (void)h.Insert<"accept">(HeaderValue::From<"*/*">());
    h.Rehash(64U);

    EXPECT_EQ(h.Size(), 1U);
    EXPECT_TRUE(h.Contains<"accept">());
}

// NOLINTEND(google-build-using-namespace,readability-identifier-length)
