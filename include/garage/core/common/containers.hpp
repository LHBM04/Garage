#ifndef GARAGE_CORE_COMMON_CONTAINERS_HPP
#define GARAGE_CORE_COMMON_CONTAINERS_HPP

#include <array>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <functional>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <span>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../memory/allocator.hpp"

namespace Garage
{
    template <class TChar, class TTraits = std::char_traits<TChar>>
    using BasicString = std::basic_string<TChar, TTraits, Memory::StdAllocator<TChar>>;

    using String    = BasicString<char>;
    using WString   = BasicString<wchar_t>;
    using U8String  = BasicString<char8_t>;
    using U16String = BasicString<char16_t>;
    using U32String = BasicString<char32_t>;

    template <class TChar, class TTraits = std::char_traits<TChar>>
    using BasicStringView = std::basic_string_view<TChar, TTraits>;

    using StringView    = std::string_view;
    using WStringView   = std::wstring_view;
    using U8StringView  = std::u8string_view;
    using U16StringView = std::u16string_view;
    using U32StringView = std::u32string_view;

    template <class T>
    struct Hash
    {
        [[nodiscard]] std::size_t operator()(const T& value) const noexcept(noexcept(std::hash<T>{}(value)))
        {
            return std::hash<T>{}(value);
        }
    };

    template <class TChar, class TTraits>
    struct Hash<BasicString<TChar, TTraits>>
    {
        [[nodiscard]] std::size_t operator()(const BasicString<TChar, TTraits>& value) const noexcept
        {
            return std::hash<std::basic_string_view<TChar, TTraits>>{}(value);
        }
    };

    template <class T, std::size_t Size>
    using Array = std::array<T, Size>;

    template <class T>
    using Vector = std::vector<T, Memory::StdAllocator<T>>;

    template <class T>
    using Deque = std::deque<T, Memory::StdAllocator<T>>;

    template <class T>
    using ForwardList = std::forward_list<T, Memory::StdAllocator<T>>;

    template <class T>
    using List = std::list<T, Memory::StdAllocator<T>>;

    template <class TKey, class TCompare = std::less<TKey>>
    using Set = std::set<TKey, TCompare, Memory::StdAllocator<TKey>>;

    template <class TKey, class TCompare = std::less<TKey>>
    using MultiSet = std::multiset<TKey, TCompare, Memory::StdAllocator<TKey>>;

    template <class TKey, class TValue, class TCompare = std::less<TKey>>
    using Map = std::map<TKey, TValue, TCompare, Memory::StdAllocator<std::pair<const TKey, TValue>>>;

    template <class TKey, class TValue, class TCompare = std::less<TKey>>
    using MultiMap = std::multimap<TKey, TValue, TCompare, Memory::StdAllocator<std::pair<const TKey, TValue>>>;

    template <class TKey, class THash = Hash<TKey>, class TKeyEqual = std::equal_to<TKey>>
    using UnorderedSet = std::unordered_set<TKey, THash, TKeyEqual, Memory::StdAllocator<TKey>>;

    template <class TKey, class THash = Hash<TKey>, class TKeyEqual = std::equal_to<TKey>>
    using UnorderedMultiSet = std::unordered_multiset<TKey, THash, TKeyEqual, Memory::StdAllocator<TKey>>;

    template <class TKey, class TValue, class THash = Hash<TKey>, class TKeyEqual = std::equal_to<TKey>>
    using UnorderedMap =
            std::unordered_map<TKey, TValue, THash, TKeyEqual, Memory::StdAllocator<std::pair<const TKey, TValue>>>;

    template <class TKey, class TValue, class THash = Hash<TKey>, class TKeyEqual = std::equal_to<TKey>>
    using UnorderedMultiMap = std::
            unordered_multimap<TKey, TValue, THash, TKeyEqual, Memory::StdAllocator<std::pair<const TKey, TValue>>>;

    template <class T, class TContainer = Deque<T>>
    using Stack = std::stack<T, TContainer>;

    template <class T, class TContainer = Deque<T>>
    using Queue = std::queue<T, TContainer>;

    template <class T, class TContainer = Vector<T>, class TCompare = std::less<typename TContainer::value_type>>
    using PriorityQueue = std::priority_queue<T, TContainer, TCompare>;

    template <class T, std::size_t Extent = std::dynamic_extent>
    using Span = std::span<T, Extent>;
} // namespace Garage

#endif // GARAGE_CORE_COMMON_CONTAINERS_HPP
