#ifndef GARAGE_CORE_TYPES_HPP
#define GARAGE_CORE_TYPES_HPP

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "defines.hpp"

namespace Garage
{
    using Int8  = std::int8_t;
    using Int16 = std::int16_t;
    using Int32 = std::int32_t;
    using Int64 = std::int64_t;

    using UInt8  = std::uint8_t;
    using UInt16 = std::uint16_t;
    using UInt32 = std::uint32_t;
    using UInt64 = std::uint64_t;

    using Float32 = float;
    using Float64 = double;

    using USize = std::size_t;
    using ISize = std::ptrdiff_t;

    using Byte = std::byte;

    template <class TResult, class... TArgs>
    using Func = std::function<TResult(TArgs...)>;

    template <class... TArgs>
    using Action = std::function<void(TArgs...)>;

    template <class... TArgs>
    using Predicate = std::function<bool(TArgs...)>;

    struct UnexpectedTag final
    {
        explicit constexpr UnexpectedTag() noexcept = default;
    };

    inline constexpr UnexpectedTag UNEXPECTED{};

    template <class TError>
    class Unexpected final
    {
        static_assert(!std::is_reference_v<TError>, "Unexpected 에러 타입은 참조일 수 없습니다.");
        static_assert(!std::is_void_v<TError>, "Unexpected 에러 타입은 void일 수 없습니다.");

    public:
        using ErrorType = TError;

        Unexpected() = delete;
        explicit constexpr Unexpected(const TError& error) noexcept(std::is_nothrow_copy_constructible_v<TError>)
            requires std::is_copy_constructible_v<TError>;
        explicit constexpr Unexpected(TError&& error) noexcept(std::is_nothrow_move_constructible_v<TError>)
            requires std::is_move_constructible_v<TError>;

        template <class... TArgs>
            requires std::is_constructible_v<TError, TArgs...>
        explicit constexpr Unexpected(std::in_place_t,
                                      TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TError, TArgs...>);

        constexpr Unexpected(const Unexpected&) noexcept(std::is_nothrow_copy_constructible_v<TError>) = default;
        constexpr Unexpected(Unexpected&&) noexcept(std::is_nothrow_move_constructible_v<TError>)      = default;
        constexpr Unexpected&
                operator=(const Unexpected&) noexcept(std::is_nothrow_copy_constructible_v<TError> &&
                                                      std::is_nothrow_copy_assignable_v<TError>)          = default;
        constexpr Unexpected& operator=(Unexpected&&) noexcept(std::is_nothrow_move_constructible_v<TError> &&
                                                               std::is_nothrow_move_assignable_v<TError>) = default;
        constexpr ~Unexpected() noexcept                                                                  = default;

        [[nodiscard]] constexpr const TError&  GetError() const& noexcept;
        [[nodiscard]] constexpr TError&        GetError() & noexcept;
        [[nodiscard]] constexpr const TError&& GetError() const&& noexcept;
        [[nodiscard]] constexpr TError&&       GetError() && noexcept;

        constexpr void Swap(Unexpected& other) noexcept(std::is_nothrow_swappable_v<TError>);

    private:
        TError error;
    };

    template <class TError>
    Unexpected(TError) -> Unexpected<TError>;

    template <class TError>
    [[nodiscard]] constexpr Unexpected<std::decay_t<TError>>
            MakeUnexpected(TError&& error) noexcept(std::is_nothrow_constructible_v<std::decay_t<TError>, TError>);

    template <class TError, class TOtherError>
    [[nodiscard]] constexpr bool
            operator==(const Unexpected<TError>&      lhs,
                       const Unexpected<TOtherError>& rhs) noexcept(noexcept(lhs.GetError() == rhs.GetError()));

    template <class TValue, class TError>
    class Result final
    {
        static_assert(!std::is_reference_v<TValue>, "Result 값 타입은 참조일 수 없습니다.");
        static_assert(!std::is_reference_v<TError>, "Result 에러 타입은 참조일 수 없습니다.");
        static_assert(!std::is_void_v<TError>, "Result 에러 타입은 void일 수 없습니다.");

    public:
        using ValueType      = TValue;
        using ErrorType      = TError;
        using UnexpectedType = Unexpected<TError>;

    private:
        using Storage = std::variant<TValue, TError>;

    public:
        constexpr Result() noexcept(std::is_nothrow_default_constructible_v<TValue>)
            requires std::is_default_constructible_v<TValue>;

        constexpr Result(const Result&) noexcept(std::is_nothrow_copy_constructible_v<Storage>)         = default;
        constexpr Result(Result&&) noexcept(std::is_nothrow_move_constructible_v<Storage>)              = default;
        constexpr Result& operator=(const Result&) noexcept(std::is_nothrow_copy_constructible_v<Storage> &&
                                                            std::is_nothrow_copy_assignable_v<Storage>) = default;
        constexpr Result& operator=(Result&&) noexcept(std::is_nothrow_move_constructible_v<Storage> &&
                                                       std::is_nothrow_move_assignable_v<Storage>)      = default;
        constexpr ~Result() noexcept                                                                    = default;

        explicit constexpr Result(const TValue& value) noexcept(std::is_nothrow_copy_constructible_v<TValue>)
            requires std::is_copy_constructible_v<TValue>;
        explicit constexpr Result(TValue&& value) noexcept(std::is_nothrow_move_constructible_v<TValue>)
            requires std::is_move_constructible_v<TValue>;

        explicit constexpr Result(const Unexpected<TError>& error) noexcept(
                std::is_nothrow_copy_constructible_v<TError>)
            requires std::is_copy_constructible_v<TError>;
        explicit constexpr Result(Unexpected<TError>&& error) noexcept(std::is_nothrow_move_constructible_v<TError>)
            requires std::is_move_constructible_v<TError>;

        template <class... TArgs>
            requires std::is_constructible_v<TValue, TArgs...>
        explicit constexpr Result(std::in_place_t,
                                  TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TValue, TArgs...>);

        template <class... TArgs>
            requires std::is_constructible_v<TError, TArgs...>
        explicit constexpr Result(UnexpectedTag,
                                  TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TError, TArgs...>);

        [[nodiscard]] static constexpr Result
                Ok(const TValue& value) noexcept(std::is_nothrow_copy_constructible_v<TValue>)
            requires std::is_copy_constructible_v<TValue>;
        [[nodiscard]] static constexpr Result Ok(TValue&& value) noexcept(std::is_nothrow_move_constructible_v<TValue>)
            requires std::is_move_constructible_v<TValue>;

        template <class... TArgs>
            requires std::is_constructible_v<TValue, TArgs...>
        [[nodiscard]] static constexpr Result
                EmplaceOk(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TValue, TArgs...>);

        [[nodiscard]] static constexpr Result
                Err(const TError& error) noexcept(std::is_nothrow_copy_constructible_v<TError>)
            requires std::is_copy_constructible_v<TError>;
        [[nodiscard]] static constexpr Result Err(TError&& error) noexcept(std::is_nothrow_move_constructible_v<TError>)
            requires std::is_move_constructible_v<TError>;

        template <class... TArgs>
            requires std::is_constructible_v<TError, TArgs...>
        [[nodiscard]] static constexpr Result
                EmplaceErr(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TError, TArgs...>);

        [[nodiscard]] constexpr explicit operator bool() const noexcept;
        [[nodiscard]] constexpr bool     HasValue() const noexcept;
        [[nodiscard]] constexpr bool     HasError() const noexcept;

        [[nodiscard]] constexpr const TValue* operator->() const noexcept;
        [[nodiscard]] constexpr TValue*       operator->() noexcept;

        [[nodiscard]] constexpr const TValue&  operator*() const& noexcept;
        [[nodiscard]] constexpr TValue&        operator*() & noexcept;
        [[nodiscard]] constexpr const TValue&& operator*() const&& noexcept;
        [[nodiscard]] constexpr TValue&&       operator*() && noexcept;

        [[nodiscard]] constexpr const TValue&  GetValue() const& noexcept;
        [[nodiscard]] constexpr TValue&        GetValue() & noexcept;
        [[nodiscard]] constexpr const TValue&& GetValue() const&& noexcept;
        [[nodiscard]] constexpr TValue&&       GetValue() && noexcept;

        [[nodiscard]] constexpr const TError&  GetError() const& noexcept;
        [[nodiscard]] constexpr TError&        GetError() & noexcept;
        [[nodiscard]] constexpr const TError&& GetError() const&& noexcept;
        [[nodiscard]] constexpr TError&&       GetError() && noexcept;

        template <class TDefault>
            requires std::is_copy_constructible_v<TValue> && std::is_convertible_v<TDefault&&, TValue>
        [[nodiscard]] constexpr TValue
                ValueOr(TDefault&& defaultValue) const& noexcept(std::is_nothrow_copy_constructible_v<TValue> &&
                                                                 std::is_nothrow_constructible_v<TValue, TDefault>);

        template <class TDefault>
            requires std::is_move_constructible_v<TValue> && std::is_convertible_v<TDefault&&, TValue>
        [[nodiscard]] constexpr TValue
                ValueOr(TDefault&& defaultValue) && noexcept(std::is_nothrow_move_constructible_v<TValue> &&
                                                             std::is_nothrow_constructible_v<TValue, TDefault>);

        template <class TFallback>
            requires std::is_copy_constructible_v<TError> && std::is_convertible_v<TFallback&&, TError>
        [[nodiscard]] constexpr TError
                ErrorOr(TFallback&& defaultError) const& noexcept(std::is_nothrow_copy_constructible_v<TError> &&
                                                                  std::is_nothrow_constructible_v<TError, TFallback>);

        template <class TFallback>
            requires std::is_move_constructible_v<TError> && std::is_convertible_v<TFallback&&, TError>
        [[nodiscard]] constexpr TError
                ErrorOr(TFallback&& defaultError) && noexcept(std::is_nothrow_move_constructible_v<TError> &&
                                                              std::is_nothrow_constructible_v<TError, TFallback>);

        template <class... TArgs>
            requires std::is_constructible_v<TValue, TArgs...>
        constexpr TValue& Emplace(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TValue, TArgs...>);

        constexpr void Swap(Result& other) noexcept(std::is_nothrow_swappable_v<Storage>);

        template <class TCallable>
            requires std::invocable<TCallable, const TValue&>
        [[nodiscard]] constexpr auto AndThen(TCallable&& func) const&;

        template <class TCallable>
            requires std::invocable<TCallable, TValue&>
        [[nodiscard]] constexpr auto AndThen(TCallable&& func) &;

        template <class TCallable>
            requires std::invocable<TCallable, TValue&&>
        [[nodiscard]] constexpr auto AndThen(TCallable&& func) &&;

        template <class TCallable>
            requires std::invocable<TCallable, const TValue&&>
        [[nodiscard]] constexpr auto AndThen(TCallable&& func) const&&;

        template <class TCallable>
            requires std::invocable<TCallable, const TValue&>
        [[nodiscard]] constexpr auto Transform(TCallable&& func) const&;

        template <class TCallable>
            requires std::invocable<TCallable, TValue&>
        [[nodiscard]] constexpr auto Transform(TCallable&& func) &;

        template <class TCallable>
            requires std::invocable<TCallable, TValue&&>
        [[nodiscard]] constexpr auto Transform(TCallable&& func) &&;

        template <class TCallable>
            requires std::invocable<TCallable, const TValue&&>
        [[nodiscard]] constexpr auto Transform(TCallable&& func) const&&;

        template <class TCallable>
            requires std::invocable<TCallable, const TError&>
        [[nodiscard]] constexpr auto OrElse(TCallable&& func) const&;

        template <class TCallable>
            requires std::invocable<TCallable, TError&>
        [[nodiscard]] constexpr auto OrElse(TCallable&& func) &;

        template <class TCallable>
            requires std::invocable<TCallable, TError&&>
        [[nodiscard]] constexpr auto OrElse(TCallable&& func) &&;

        template <class TCallable>
            requires std::invocable<TCallable, const TError&&>
        [[nodiscard]] constexpr auto OrElse(TCallable&& func) const&&;

        template <class TCallable>
            requires std::invocable<TCallable, const TError&>
        [[nodiscard]] constexpr auto TransformError(TCallable&& func) const&;

        template <class TCallable>
            requires std::invocable<TCallable, TError&>
        [[nodiscard]] constexpr auto TransformError(TCallable&& func) &;

        template <class TCallable>
            requires std::invocable<TCallable, TError&&>
        [[nodiscard]] constexpr auto TransformError(TCallable&& func) &&;

        template <class TCallable>
            requires std::invocable<TCallable, const TError&&>
        [[nodiscard]] constexpr auto TransformError(TCallable&& func) const&&;

    private:
        Storage storage;
    };

    template <class TError>
    class Result<void, TError> final
    {
        static_assert(!std::is_reference_v<TError>, "Result 에러 타입은 참조일 수 없습니다.");
        static_assert(!std::is_void_v<TError>, "Result 에러 타입은 void일 수 없습니다.");

    public:
        using ValueType      = void;
        using ErrorType      = TError;
        using UnexpectedType = Unexpected<TError>;

    private:
        using Storage = std::variant<std::monostate, TError>;

    public:
        constexpr Result() noexcept = default;

        constexpr Result(const Result&) noexcept(std::is_nothrow_copy_constructible_v<Storage>)         = default;
        constexpr Result(Result&&) noexcept(std::is_nothrow_move_constructible_v<Storage>)              = default;
        constexpr Result& operator=(const Result&) noexcept(std::is_nothrow_copy_constructible_v<Storage> &&
                                                            std::is_nothrow_copy_assignable_v<Storage>) = default;
        constexpr Result& operator=(Result&&) noexcept(std::is_nothrow_move_constructible_v<Storage> &&
                                                       std::is_nothrow_move_assignable_v<Storage>)      = default;
        constexpr ~Result() noexcept                                                                    = default;

        explicit constexpr Result(const Unexpected<TError>& error) noexcept(
                std::is_nothrow_copy_constructible_v<TError>)
            requires std::is_copy_constructible_v<TError>;
        explicit constexpr Result(Unexpected<TError>&& error) noexcept(std::is_nothrow_move_constructible_v<TError>)
            requires std::is_move_constructible_v<TError>;

        template <class... TArgs>
            requires std::is_constructible_v<TError, TArgs...>
        explicit constexpr Result(UnexpectedTag,
                                  TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TError, TArgs...>);

        [[nodiscard]] static constexpr Result Ok() noexcept;

        [[nodiscard]] static constexpr Result
                Err(const TError& error) noexcept(std::is_nothrow_copy_constructible_v<TError>)
            requires std::is_copy_constructible_v<TError>;
        [[nodiscard]] static constexpr Result Err(TError&& error) noexcept(std::is_nothrow_move_constructible_v<TError>)
            requires std::is_move_constructible_v<TError>;

        template <class... TArgs>
            requires std::is_constructible_v<TError, TArgs...>
        [[nodiscard]] static constexpr Result
                EmplaceErr(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TError, TArgs...>);

        [[nodiscard]] constexpr explicit operator bool() const noexcept;
        [[nodiscard]] constexpr bool     HasValue() const noexcept;
        [[nodiscard]] constexpr bool     HasError() const noexcept;

        constexpr void operator*() const noexcept;
        constexpr void GetValue() const noexcept;

        [[nodiscard]] constexpr const TError&  GetError() const& noexcept;
        [[nodiscard]] constexpr TError&        GetError() & noexcept;
        [[nodiscard]] constexpr const TError&& GetError() const&& noexcept;
        [[nodiscard]] constexpr TError&&       GetError() && noexcept;

        template <class TFallback>
            requires std::is_copy_constructible_v<TError> && std::is_convertible_v<TFallback&&, TError>
        [[nodiscard]] constexpr TError
                ErrorOr(TFallback&& defaultError) const& noexcept(std::is_nothrow_copy_constructible_v<TError> &&
                                                                  std::is_nothrow_constructible_v<TError, TFallback>);

        template <class TFallback>
            requires std::is_move_constructible_v<TError> && std::is_convertible_v<TFallback&&, TError>
        [[nodiscard]] constexpr TError
                ErrorOr(TFallback&& defaultError) && noexcept(std::is_nothrow_move_constructible_v<TError> &&
                                                              std::is_nothrow_constructible_v<TError, TFallback>);

        constexpr void Emplace() noexcept;
        constexpr void Swap(Result& other) noexcept(std::is_nothrow_swappable_v<Storage>);

        template <class TCallable>
            requires std::invocable<TCallable>
        [[nodiscard]] constexpr auto AndThen(TCallable&& func) const&;

        template <class TCallable>
            requires std::invocable<TCallable>
        [[nodiscard]] constexpr auto AndThen(TCallable&& func) &&;

        template <class TCallable>
            requires std::invocable<TCallable>
        [[nodiscard]] constexpr auto Transform(TCallable&& func) const&;

        template <class TCallable>
            requires std::invocable<TCallable>
        [[nodiscard]] constexpr auto Transform(TCallable&& func) &&;

        template <class TCallable>
            requires std::invocable<TCallable, const TError&>
        [[nodiscard]] constexpr auto OrElse(TCallable&& func) const&;

        template <class TCallable>
            requires std::invocable<TCallable, TError&>
        [[nodiscard]] constexpr auto OrElse(TCallable&& func) &;

        template <class TCallable>
            requires std::invocable<TCallable, TError&&>
        [[nodiscard]] constexpr auto OrElse(TCallable&& func) &&;

        template <class TCallable>
            requires std::invocable<TCallable, const TError&&>
        [[nodiscard]] constexpr auto OrElse(TCallable&& func) const&&;

        template <class TCallable>
            requires std::invocable<TCallable, const TError&>
        [[nodiscard]] constexpr auto TransformError(TCallable&& func) const&;

        template <class TCallable>
            requires std::invocable<TCallable, TError&>
        [[nodiscard]] constexpr auto TransformError(TCallable&& func) &;

        template <class TCallable>
            requires std::invocable<TCallable, TError&&>
        [[nodiscard]] constexpr auto TransformError(TCallable&& func) &&;

        template <class TCallable>
            requires std::invocable<TCallable, const TError&&>
        [[nodiscard]] constexpr auto TransformError(TCallable&& func) const&&;

    private:
        Storage storage;
    };

    template <class T>
    struct IsUnexpected : std::false_type
    {
    };

    template <class TError>
    struct IsUnexpected<Unexpected<TError>> : std::true_type
    {
    };

    template <class T>
    struct IsResult : std::false_type
    {
    };

    template <class TValue, class TError>
    struct IsResult<Result<TValue, TError>> : std::true_type
    {
    };

    template <class TValue, class TError, class TOtherValue, class TOtherError>
        requires(!std::is_void_v<TValue> && !std::is_void_v<TOtherValue>)
    [[nodiscard]] constexpr bool
            operator==(const Result<TValue, TError>& lhs, const Result<TOtherValue, TOtherError>& rhs) noexcept(
                    noexcept(lhs.GetValue() == rhs.GetValue()) && noexcept(lhs.GetError() == rhs.GetError()));

    template <class TError, class TOtherError>
    [[nodiscard]] constexpr bool
            operator==(const Result<void, TError>&      lhs,
                       const Result<void, TOtherError>& rhs) noexcept(noexcept(lhs.GetError() == rhs.GetError()));

    template <class TValue, class TError, class TOtherError>
    [[nodiscard]] constexpr bool
            operator==(const Result<TValue, TError>&  lhs,
                       const Unexpected<TOtherError>& rhs) noexcept(noexcept(lhs.GetError() == rhs.GetError()));

    template <class TValue, class TError, class TOtherValue>
        requires(!std::is_void_v<TValue> && !IsResult<std::remove_cvref_t<TOtherValue>>::value &&
                 !IsUnexpected<std::remove_cvref_t<TOtherValue>>::value)
    [[nodiscard]] constexpr bool operator==(const Result<TValue, TError>& lhs,
                                            const TOtherValue& rhs) noexcept(noexcept(lhs.GetValue() == rhs));

    template <class TError>
    constexpr Unexpected<TError>::Unexpected(const TError& error) noexcept(std::is_nothrow_copy_constructible_v<TError>)
        requires std::is_copy_constructible_v<TError>
        : error(error)
    {
    }

    template <class TError>
    constexpr Unexpected<TError>::Unexpected(TError&& error) noexcept(std::is_nothrow_move_constructible_v<TError>)
        requires std::is_move_constructible_v<TError>
        : error(std::move(error))
    {
    }

    template <class TError>
    template <class... TArgs>
        requires std::is_constructible_v<TError, TArgs...>
    constexpr Unexpected<TError>::Unexpected(std::in_place_t, TArgs&&... args) noexcept(
            std::is_nothrow_constructible_v<TError, TArgs...>)
        : error(std::forward<TArgs>(args)...)
    {
    }

    template <class TError>
    constexpr const TError& Unexpected<TError>::GetError() const& noexcept
    {
        return error;
    }

    template <class TError>
    constexpr TError& Unexpected<TError>::GetError() & noexcept
    {
        return error;
    }

    template <class TError>
    constexpr const TError&& Unexpected<TError>::GetError() const&& noexcept
    {
        return std::move(error);
    }

    template <class TError>
    constexpr TError&& Unexpected<TError>::GetError() && noexcept
    {
        return std::move(error);
    }

    template <class TError>
    constexpr void Unexpected<TError>::Swap(Unexpected& other) noexcept(std::is_nothrow_swappable_v<TError>)
    {
        using std::swap;
        swap(error, other.error);
    }

    template <class TError>
    constexpr Unexpected<std::decay_t<TError>>
            MakeUnexpected(TError&& error) noexcept(std::is_nothrow_constructible_v<std::decay_t<TError>, TError>)
    {
        return Unexpected<std::decay_t<TError>>(std::forward<TError>(error));
    }

    template <class TError, class TOtherError>
    constexpr bool operator==(const Unexpected<TError>&      lhs,
                              const Unexpected<TOtherError>& rhs) noexcept(noexcept(lhs.GetError() == rhs.GetError()))
    {
        return lhs.GetError() == rhs.GetError();
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>::Result() noexcept(std::is_nothrow_default_constructible_v<TValue>)
        requires std::is_default_constructible_v<TValue>
        : storage(std::in_place_index<0>)
    {
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>::Result(const TValue& value) noexcept(std::is_nothrow_copy_constructible_v<TValue>)
        requires std::is_copy_constructible_v<TValue>
        : storage(std::in_place_index<0>, value)
    {
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>::Result(TValue&& value) noexcept(std::is_nothrow_move_constructible_v<TValue>)
        requires std::is_move_constructible_v<TValue>
        : storage(std::in_place_index<0>, std::move(value))
    {
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>::Result(const Unexpected<TError>& error) noexcept(
            std::is_nothrow_copy_constructible_v<TError>)
        requires std::is_copy_constructible_v<TError>
        : storage(std::in_place_index<1>, error.GetError())
    {
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>::Result(Unexpected<TError>&& error) noexcept(
            std::is_nothrow_move_constructible_v<TError>)
        requires std::is_move_constructible_v<TError>
        : storage(std::in_place_index<1>, std::move(error).GetError())
    {
    }

    template <class TValue, class TError>
    template <class... TArgs>
        requires std::is_constructible_v<TValue, TArgs...>
    constexpr Result<TValue, TError>::Result(std::in_place_t, TArgs&&... args) noexcept(
            std::is_nothrow_constructible_v<TValue, TArgs...>)
        : storage(std::in_place_index<0>, std::forward<TArgs>(args)...)
    {
    }

    template <class TValue, class TError>
    template <class... TArgs>
        requires std::is_constructible_v<TError, TArgs...>
    constexpr Result<TValue, TError>::Result(UnexpectedTag, TArgs&&... args) noexcept(
            std::is_nothrow_constructible_v<TError, TArgs...>)
        : storage(std::in_place_index<1>, std::forward<TArgs>(args)...)
    {
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>
            Result<TValue, TError>::Ok(const TValue& value) noexcept(std::is_nothrow_copy_constructible_v<TValue>)
        requires std::is_copy_constructible_v<TValue>
    {
        return Result(value);
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>
            Result<TValue, TError>::Ok(TValue&& value) noexcept(std::is_nothrow_move_constructible_v<TValue>)
        requires std::is_move_constructible_v<TValue>
    {
        return Result(std::move(value));
    }

    template <class TValue, class TError>
    template <class... TArgs>
        requires std::is_constructible_v<TValue, TArgs...>
    constexpr Result<TValue, TError> Result<TValue, TError>::EmplaceOk(TArgs&&... args) noexcept(
            std::is_nothrow_constructible_v<TValue, TArgs...>)
    {
        return Result(std::in_place, std::forward<TArgs>(args)...);
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>
            Result<TValue, TError>::Err(const TError& error) noexcept(std::is_nothrow_copy_constructible_v<TError>)
        requires std::is_copy_constructible_v<TError>
    {
        return Result(UNEXPECTED, error);
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>
            Result<TValue, TError>::Err(TError&& error) noexcept(std::is_nothrow_move_constructible_v<TError>)
        requires std::is_move_constructible_v<TError>
    {
        return Result(UNEXPECTED, std::move(error));
    }

    template <class TValue, class TError>
    template <class... TArgs>
        requires std::is_constructible_v<TError, TArgs...>
    constexpr Result<TValue, TError> Result<TValue, TError>::EmplaceErr(TArgs&&... args) noexcept(
            std::is_nothrow_constructible_v<TError, TArgs...>)
    {
        return Result(UNEXPECTED, std::forward<TArgs>(args)...);
    }

    template <class TValue, class TError>
    constexpr Result<TValue, TError>::operator bool() const noexcept
    {
        return HasValue();
    }

    template <class TValue, class TError>
    constexpr bool Result<TValue, TError>::HasValue() const noexcept
    {
        return storage.index() == 0;
    }

    template <class TValue, class TError>
    constexpr bool Result<TValue, TError>::HasError() const noexcept
    {
        return storage.index() == 1;
    }

    template <class TValue, class TError>
    constexpr const TValue* Result<TValue, TError>::operator->() const noexcept
    {
        return &GetValue();
    }

    template <class TValue, class TError>
    constexpr TValue* Result<TValue, TError>::operator->() noexcept
    {
        return &GetValue();
    }

    template <class TValue, class TError>
    constexpr const TValue& Result<TValue, TError>::operator*() const& noexcept
    {
        return GetValue();
    }

    template <class TValue, class TError>
    constexpr TValue& Result<TValue, TError>::operator*() & noexcept
    {
        return GetValue();
    }

    template <class TValue, class TError>
    constexpr const TValue&& Result<TValue, TError>::operator*() const&& noexcept
    {
        return std::move(GetValue());
    }

    template <class TValue, class TError>
    constexpr TValue&& Result<TValue, TError>::operator*() && noexcept
    {
        return std::move(GetValue());
    }

    template <class TValue, class TError>
    constexpr const TValue& Result<TValue, TError>::GetValue() const& noexcept
    {
        ASSERT(HasValue());
        return *std::get_if<0>(&storage);
    }

    template <class TValue, class TError>
    constexpr TValue& Result<TValue, TError>::GetValue() & noexcept
    {
        ASSERT(HasValue());
        return *std::get_if<0>(&storage);
    }

    template <class TValue, class TError>
    constexpr const TValue&& Result<TValue, TError>::GetValue() const&& noexcept
    {
        ASSERT(HasValue());
        return std::move(*std::get_if<0>(&storage));
    }

    template <class TValue, class TError>
    constexpr TValue&& Result<TValue, TError>::GetValue() && noexcept
    {
        ASSERT(HasValue());
        return std::move(*std::get_if<0>(&storage));
    }

    template <class TValue, class TError>
    constexpr const TError& Result<TValue, TError>::GetError() const& noexcept
    {
        ASSERT(HasError());
        return *std::get_if<1>(&storage);
    }

    template <class TValue, class TError>
    constexpr TError& Result<TValue, TError>::GetError() & noexcept
    {
        ASSERT(HasError());
        return *std::get_if<1>(&storage);
    }

    template <class TValue, class TError>
    constexpr const TError&& Result<TValue, TError>::GetError() const&& noexcept
    {
        ASSERT(HasError());
        return std::move(*std::get_if<1>(&storage));
    }

    template <class TValue, class TError>
    constexpr TError&& Result<TValue, TError>::GetError() && noexcept
    {
        ASSERT(HasError());
        return std::move(*std::get_if<1>(&storage));
    }

    template <class TValue, class TError>
    template <class TDefault>
        requires std::is_copy_constructible_v<TValue> && std::is_convertible_v<TDefault&&, TValue>
    constexpr TValue Result<TValue, TError>::ValueOr(TDefault&& defaultValue) const& noexcept(
            std::is_nothrow_copy_constructible_v<TValue> && std::is_nothrow_constructible_v<TValue, TDefault>)
    {
        if (HasValue())
        {
            return *std::get_if<0>(&storage);
        }

        return static_cast<TValue>(std::forward<TDefault>(defaultValue));
    }

    template <class TValue, class TError>
    template <class TDefault>
        requires std::is_move_constructible_v<TValue> && std::is_convertible_v<TDefault&&, TValue>
    constexpr TValue Result<TValue, TError>::ValueOr(TDefault&& defaultValue) && noexcept(
            std::is_nothrow_move_constructible_v<TValue> && std::is_nothrow_constructible_v<TValue, TDefault>)
    {
        if (HasValue())
        {
            return std::move(*std::get_if<0>(&storage));
        }

        return static_cast<TValue>(std::forward<TDefault>(defaultValue));
    }

    template <class TValue, class TError>
    template <class TFallback>
        requires std::is_copy_constructible_v<TError> && std::is_convertible_v<TFallback&&, TError>
    constexpr TError Result<TValue, TError>::ErrorOr(TFallback&& defaultError) const& noexcept(
            std::is_nothrow_copy_constructible_v<TError> && std::is_nothrow_constructible_v<TError, TFallback>)
    {
        if (HasError())
        {
            return *std::get_if<1>(&storage);
        }

        return static_cast<TError>(std::forward<TFallback>(defaultError));
    }

    template <class TValue, class TError>
    template <class TFallback>
        requires std::is_move_constructible_v<TError> && std::is_convertible_v<TFallback&&, TError>
    constexpr TError Result<TValue, TError>::ErrorOr(TFallback&& defaultError) && noexcept(
            std::is_nothrow_move_constructible_v<TError> && std::is_nothrow_constructible_v<TError, TFallback>)
    {
        if (HasError())
        {
            return std::move(*std::get_if<1>(&storage));
        }

        return static_cast<TError>(std::forward<TFallback>(defaultError));
    }

    template <class TValue, class TError>
    template <class... TArgs>
        requires std::is_constructible_v<TValue, TArgs...>
    constexpr TValue&
            Result<TValue, TError>::Emplace(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TValue, TArgs...>)
    {
        return storage.template emplace<0>(std::forward<TArgs>(args)...);
    }

    template <class TValue, class TError>
    constexpr void Result<TValue, TError>::Swap(Result& other) noexcept(std::is_nothrow_swappable_v<Storage>)
    {
        storage.swap(other.storage);
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TValue&>
    constexpr auto Result<TValue, TError>::AndThen(TCallable&& func) const&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, const TValue&>>;
        if (HasValue())
        {
            return std::invoke(std::forward<TCallable>(func), GetValue());
        }

        return NextResult::Err(GetError());
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TValue&>
    constexpr auto Result<TValue, TError>::AndThen(TCallable&& func) &
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, TValue&>>;
        if (HasValue())
        {
            return std::invoke(std::forward<TCallable>(func), GetValue());
        }

        return NextResult::Err(GetError());
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TValue&&>
    constexpr auto Result<TValue, TError>::AndThen(TCallable&& func) &&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, TValue&&>>;
        if (HasValue())
        {
            return std::invoke(std::forward<TCallable>(func), std::move(*this).GetValue());
        }

        return NextResult::Err(std::move(*this).GetError());
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TValue&&>
    constexpr auto Result<TValue, TError>::AndThen(TCallable&& func) const&&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, const TValue&&>>;
        if (HasValue())
        {
            return std::invoke(std::forward<TCallable>(func), std::move(*this).GetValue());
        }

        return NextResult::Err(std::move(*this).GetError());
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TValue&>
    constexpr auto Result<TValue, TError>::Transform(TCallable&& func) const&
    {
        using Transformed = std::remove_cv_t<std::invoke_result_t<TCallable, const TValue&>>;
        if constexpr (std::is_void_v<Transformed>)
        {
            if (HasValue())
            {
                std::invoke(std::forward<TCallable>(func), GetValue());
                return Result<void, TError>::Ok();
            }

            return Result<void, TError>::Err(GetError());
        }
        else
        {
            if (HasValue())
            {
                return Result<Transformed, TError>::Ok(std::invoke(std::forward<TCallable>(func), GetValue()));
            }

            return Result<Transformed, TError>::Err(GetError());
        }
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TValue&>
    constexpr auto Result<TValue, TError>::Transform(TCallable&& func) &
    {
        using Transformed = std::remove_cv_t<std::invoke_result_t<TCallable, TValue&>>;
        if constexpr (std::is_void_v<Transformed>)
        {
            if (HasValue())
            {
                std::invoke(std::forward<TCallable>(func), GetValue());
                return Result<void, TError>::Ok();
            }

            return Result<void, TError>::Err(GetError());
        }
        else
        {
            if (HasValue())
            {
                return Result<Transformed, TError>::Ok(std::invoke(std::forward<TCallable>(func), GetValue()));
            }

            return Result<Transformed, TError>::Err(GetError());
        }
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TValue&&>
    constexpr auto Result<TValue, TError>::Transform(TCallable&& func) &&
    {
        using Transformed = std::remove_cv_t<std::invoke_result_t<TCallable, TValue&&>>;
        if constexpr (std::is_void_v<Transformed>)
        {
            if (HasValue())
            {
                std::invoke(std::forward<TCallable>(func), std::move(*this).GetValue());
                return Result<void, TError>::Ok();
            }

            return Result<void, TError>::Err(std::move(*this).GetError());
        }
        else
        {
            if (HasValue())
            {
                return Result<Transformed, TError>::Ok(
                        std::invoke(std::forward<TCallable>(func), std::move(*this).GetValue()));
            }

            return Result<Transformed, TError>::Err(std::move(*this).GetError());
        }
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TValue&&>
    constexpr auto Result<TValue, TError>::Transform(TCallable&& func) const&&
    {
        using Transformed = std::remove_cv_t<std::invoke_result_t<TCallable, const TValue&&>>;
        if constexpr (std::is_void_v<Transformed>)
        {
            if (HasValue())
            {
                std::invoke(std::forward<TCallable>(func), std::move(*this).GetValue());
                return Result<void, TError>::Ok();
            }

            return Result<void, TError>::Err(std::move(*this).GetError());
        }
        else
        {
            if (HasValue())
            {
                return Result<Transformed, TError>::Ok(
                        std::invoke(std::forward<TCallable>(func), std::move(*this).GetValue()));
            }

            return Result<Transformed, TError>::Err(std::move(*this).GetError());
        }
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TError&>
    constexpr auto Result<TValue, TError>::OrElse(TCallable&& func) const&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, const TError&>>;
        if (HasValue())
        {
            return NextResult::Ok(GetValue());
        }

        return std::invoke(std::forward<TCallable>(func), GetError());
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TError&>
    constexpr auto Result<TValue, TError>::OrElse(TCallable&& func) &
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, TError&>>;
        if (HasValue())
        {
            return NextResult::Ok(GetValue());
        }

        return std::invoke(std::forward<TCallable>(func), GetError());
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TError&&>
    constexpr auto Result<TValue, TError>::OrElse(TCallable&& func) &&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, TError&&>>;
        if (HasValue())
        {
            return NextResult::Ok(std::move(*this).GetValue());
        }

        return std::invoke(std::forward<TCallable>(func), std::move(*this).GetError());
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TError&&>
    constexpr auto Result<TValue, TError>::OrElse(TCallable&& func) const&&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, const TError&&>>;
        if (HasValue())
        {
            return NextResult::Ok(std::move(*this).GetValue());
        }

        return std::invoke(std::forward<TCallable>(func), std::move(*this).GetError());
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TError&>
    constexpr auto Result<TValue, TError>::TransformError(TCallable&& func) const&
    {
        using NextError = std::remove_cv_t<std::invoke_result_t<TCallable, const TError&>>;
        if (HasValue())
        {
            return Result<TValue, NextError>::Ok(GetValue());
        }

        return Result<TValue, NextError>::Err(std::invoke(std::forward<TCallable>(func), GetError()));
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TError&>
    constexpr auto Result<TValue, TError>::TransformError(TCallable&& func) &
    {
        using NextError = std::remove_cv_t<std::invoke_result_t<TCallable, TError&>>;
        if (HasValue())
        {
            return Result<TValue, NextError>::Ok(GetValue());
        }

        return Result<TValue, NextError>::Err(std::invoke(std::forward<TCallable>(func), GetError()));
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TError&&>
    constexpr auto Result<TValue, TError>::TransformError(TCallable&& func) &&
    {
        using NextError = std::remove_cv_t<std::invoke_result_t<TCallable, TError&&>>;
        if (HasValue())
        {
            return Result<TValue, NextError>::Ok(std::move(*this).GetValue());
        }

        return Result<TValue, NextError>::Err(std::invoke(std::forward<TCallable>(func), std::move(*this).GetError()));
    }

    template <class TValue, class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TError&&>
    constexpr auto Result<TValue, TError>::TransformError(TCallable&& func) const&&
    {
        using NextError = std::remove_cv_t<std::invoke_result_t<TCallable, const TError&&>>;
        if (HasValue())
        {
            return Result<TValue, NextError>::Ok(std::move(*this).GetValue());
        }

        return Result<TValue, NextError>::Err(std::invoke(std::forward<TCallable>(func), std::move(*this).GetError()));
    }

    template <class TError>
    constexpr Result<void, TError>::Result(const Unexpected<TError>& error) noexcept(
            std::is_nothrow_copy_constructible_v<TError>)
        requires std::is_copy_constructible_v<TError>
        : storage(std::in_place_index<1>, error.GetError())
    {
    }

    template <class TError>
    constexpr Result<void, TError>::Result(Unexpected<TError>&& error) noexcept(
            std::is_nothrow_move_constructible_v<TError>)
        requires std::is_move_constructible_v<TError>
        : storage(std::in_place_index<1>, std::move(error).GetError())
    {
    }

    template <class TError>
    template <class... TArgs>
        requires std::is_constructible_v<TError, TArgs...>
    constexpr Result<void, TError>::Result(UnexpectedTag,
                                           TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TError, TArgs...>)
        : storage(std::in_place_index<1>, std::forward<TArgs>(args)...)
    {
    }

    template <class TError>
    constexpr Result<void, TError> Result<void, TError>::Ok() noexcept
    {
        return Result();
    }

    template <class TError>
    constexpr Result<void, TError>
            Result<void, TError>::Err(const TError& error) noexcept(std::is_nothrow_copy_constructible_v<TError>)
        requires std::is_copy_constructible_v<TError>
    {
        return Result(UNEXPECTED, error);
    }

    template <class TError>
    constexpr Result<void, TError>
            Result<void, TError>::Err(TError&& error) noexcept(std::is_nothrow_move_constructible_v<TError>)
        requires std::is_move_constructible_v<TError>
    {
        return Result(UNEXPECTED, std::move(error));
    }

    template <class TError>
    template <class... TArgs>
        requires std::is_constructible_v<TError, TArgs...>
    constexpr Result<void, TError> Result<void, TError>::EmplaceErr(TArgs&&... args) noexcept(
            std::is_nothrow_constructible_v<TError, TArgs...>)
    {
        return Result(UNEXPECTED, std::forward<TArgs>(args)...);
    }

    template <class TError>
    constexpr Result<void, TError>::operator bool() const noexcept
    {
        return HasValue();
    }

    template <class TError>
    constexpr bool Result<void, TError>::HasValue() const noexcept
    {
        return storage.index() == 0;
    }

    template <class TError>
    constexpr bool Result<void, TError>::HasError() const noexcept
    {
        return storage.index() == 1;
    }

    template <class TError>
    constexpr void Result<void, TError>::operator*() const noexcept
    {
        ASSERT(HasValue());
    }

    template <class TError>
    constexpr void Result<void, TError>::GetValue() const noexcept
    {
        ASSERT(HasValue());
    }

    template <class TError>
    constexpr const TError& Result<void, TError>::GetError() const& noexcept
    {
        ASSERT(HasError());
        return *std::get_if<1>(&storage);
    }

    template <class TError>
    constexpr TError& Result<void, TError>::GetError() & noexcept
    {
        ASSERT(HasError());
        return *std::get_if<1>(&storage);
    }

    template <class TError>
    constexpr const TError&& Result<void, TError>::GetError() const&& noexcept
    {
        ASSERT(HasError());
        return std::move(*std::get_if<1>(&storage));
    }

    template <class TError>
    constexpr TError&& Result<void, TError>::GetError() && noexcept
    {
        ASSERT(HasError());
        return std::move(*std::get_if<1>(&storage));
    }

    template <class TError>
    template <class TFallback>
        requires std::is_copy_constructible_v<TError> && std::is_convertible_v<TFallback&&, TError>
    constexpr TError Result<void, TError>::ErrorOr(TFallback&& defaultError) const& noexcept(
            std::is_nothrow_copy_constructible_v<TError> && std::is_nothrow_constructible_v<TError, TFallback>)
    {
        if (HasError())
        {
            return *std::get_if<1>(&storage);
        }

        return static_cast<TError>(std::forward<TFallback>(defaultError));
    }

    template <class TError>
    template <class TFallback>
        requires std::is_move_constructible_v<TError> && std::is_convertible_v<TFallback&&, TError>
    constexpr TError Result<void, TError>::ErrorOr(TFallback&& defaultError) && noexcept(
            std::is_nothrow_move_constructible_v<TError> && std::is_nothrow_constructible_v<TError, TFallback>)
    {
        if (HasError())
        {
            return std::move(*std::get_if<1>(&storage));
        }

        return static_cast<TError>(std::forward<TFallback>(defaultError));
    }

    template <class TError>
    constexpr void Result<void, TError>::Emplace() noexcept
    {
        storage.template emplace<0>();
    }

    template <class TError>
    constexpr void Result<void, TError>::Swap(Result& other) noexcept(std::is_nothrow_swappable_v<Storage>)
    {
        storage.swap(other.storage);
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable>
    constexpr auto Result<void, TError>::AndThen(TCallable&& func) const&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable>>;
        if (HasValue())
        {
            return std::invoke(std::forward<TCallable>(func));
        }

        return NextResult::Err(GetError());
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable>
    constexpr auto Result<void, TError>::AndThen(TCallable&& func) &&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable>>;
        if (HasValue())
        {
            return std::invoke(std::forward<TCallable>(func));
        }

        return NextResult::Err(std::move(*this).GetError());
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable>
    constexpr auto Result<void, TError>::Transform(TCallable&& func) const&
    {
        using Transformed = std::remove_cv_t<std::invoke_result_t<TCallable>>;
        if constexpr (std::is_void_v<Transformed>)
        {
            if (HasValue())
            {
                std::invoke(std::forward<TCallable>(func));
                return Result<void, TError>::Ok();
            }

            return Result<void, TError>::Err(GetError());
        }
        else
        {
            if (HasValue())
            {
                return Result<Transformed, TError>::Ok(std::invoke(std::forward<TCallable>(func)));
            }

            return Result<Transformed, TError>::Err(GetError());
        }
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable>
    constexpr auto Result<void, TError>::Transform(TCallable&& func) &&
    {
        using Transformed = std::remove_cv_t<std::invoke_result_t<TCallable>>;
        if constexpr (std::is_void_v<Transformed>)
        {
            if (HasValue())
            {
                std::invoke(std::forward<TCallable>(func));
                return Result<void, TError>::Ok();
            }

            return Result<void, TError>::Err(std::move(*this).GetError());
        }
        else
        {
            if (HasValue())
            {
                return Result<Transformed, TError>::Ok(std::invoke(std::forward<TCallable>(func)));
            }

            return Result<Transformed, TError>::Err(std::move(*this).GetError());
        }
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TError&>
    constexpr auto Result<void, TError>::OrElse(TCallable&& func) const&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, const TError&>>;
        if (HasValue())
        {
            return NextResult::Ok();
        }

        return std::invoke(std::forward<TCallable>(func), GetError());
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TError&>
    constexpr auto Result<void, TError>::OrElse(TCallable&& func) &
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, TError&>>;
        if (HasValue())
        {
            return NextResult::Ok();
        }

        return std::invoke(std::forward<TCallable>(func), GetError());
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TError&&>
    constexpr auto Result<void, TError>::OrElse(TCallable&& func) &&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, TError&&>>;
        if (HasValue())
        {
            return NextResult::Ok();
        }

        return std::invoke(std::forward<TCallable>(func), std::move(*this).GetError());
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TError&&>
    constexpr auto Result<void, TError>::OrElse(TCallable&& func) const&&
    {
        using NextResult = std::remove_cvref_t<std::invoke_result_t<TCallable, const TError&&>>;
        if (HasValue())
        {
            return NextResult::Ok();
        }

        return std::invoke(std::forward<TCallable>(func), std::move(*this).GetError());
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TError&>
    constexpr auto Result<void, TError>::TransformError(TCallable&& func) const&
    {
        using NextError = std::remove_cv_t<std::invoke_result_t<TCallable, const TError&>>;
        if (HasValue())
        {
            return Result<void, NextError>::Ok();
        }

        return Result<void, NextError>::Err(std::invoke(std::forward<TCallable>(func), GetError()));
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TError&>
    constexpr auto Result<void, TError>::TransformError(TCallable&& func) &
    {
        using NextError = std::remove_cv_t<std::invoke_result_t<TCallable, TError&>>;
        if (HasValue())
        {
            return Result<void, NextError>::Ok();
        }

        return Result<void, NextError>::Err(std::invoke(std::forward<TCallable>(func), GetError()));
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable, TError&&>
    constexpr auto Result<void, TError>::TransformError(TCallable&& func) &&
    {
        using NextError = std::remove_cv_t<std::invoke_result_t<TCallable, TError&&>>;
        if (HasValue())
        {
            return Result<void, NextError>::Ok();
        }

        return Result<void, NextError>::Err(std::invoke(std::forward<TCallable>(func), std::move(*this).GetError()));
    }

    template <class TError>
    template <class TCallable>
        requires std::invocable<TCallable, const TError&&>
    constexpr auto Result<void, TError>::TransformError(TCallable&& func) const&&
    {
        using NextError = std::remove_cv_t<std::invoke_result_t<TCallable, const TError&&>>;
        if (HasValue())
        {
            return Result<void, NextError>::Ok();
        }

        return Result<void, NextError>::Err(std::invoke(std::forward<TCallable>(func), std::move(*this).GetError()));
    }

    template <class TValue, class TError, class TOtherValue, class TOtherError>
        requires(!std::is_void_v<TValue> && !std::is_void_v<TOtherValue>)
    constexpr bool operator==(const Result<TValue, TError>& lhs, const Result<TOtherValue, TOtherError>& rhs) noexcept(
            noexcept(lhs.GetValue() == rhs.GetValue()) && noexcept(lhs.GetError() == rhs.GetError()))
    {
        if (lhs.HasValue() != rhs.HasValue())
        {
            return false;
        }

        if (lhs.HasValue())
        {
            return lhs.GetValue() == rhs.GetValue();
        }

        return lhs.GetError() == rhs.GetError();
    }

    template <class TError, class TOtherError>
    constexpr bool operator==(const Result<void, TError>&      lhs,
                              const Result<void, TOtherError>& rhs) noexcept(noexcept(lhs.GetError() == rhs.GetError()))
    {
        if (lhs.HasValue() != rhs.HasValue())
        {
            return false;
        }

        if (lhs.HasValue())
        {
            return true;
        }

        return lhs.GetError() == rhs.GetError();
    }

    template <class TValue, class TError, class TOtherError>
    constexpr bool operator==(const Result<TValue, TError>&  lhs,
                              const Unexpected<TOtherError>& rhs) noexcept(noexcept(lhs.GetError() == rhs.GetError()))
    {
        return lhs.HasError() && lhs.GetError() == rhs.GetError();
    }

    template <class TValue, class TError, class TOtherValue>
        requires(!std::is_void_v<TValue> && !IsResult<std::remove_cvref_t<TOtherValue>>::value &&
                 !IsUnexpected<std::remove_cvref_t<TOtherValue>>::value)
    constexpr bool operator==(const Result<TValue, TError>& lhs,
                              const TOtherValue&            rhs) noexcept(noexcept(lhs.GetValue() == rhs))
    {
        return lhs.HasValue() && lhs.GetValue() == rhs;
    }

    template <class... TArgs>
    class Delegate final
    {
    public:
        using Listener   = std::function<void(TArgs...)>;
        using ListenerId = Int32;

        Delegate() noexcept  = default;
        ~Delegate() noexcept = default;

        Delegate(const Delegate&) noexcept            = delete;
        Delegate& operator=(const Delegate&) noexcept = delete;

        Delegate(Delegate&&) noexcept            = default;
        Delegate& operator=(Delegate&&) noexcept = default;

        Delegate& operator+=(Listener callback) noexcept;

        ListenerId AddListener(Listener callback);
        void       RemoveListener(ListenerId id) noexcept;

        void Broadcast(TArgs... args) const;

        void Clear() noexcept;

    private:
        struct Subscription final
        {
            ListenerId id          = 0;
            Listener   callback    = {};
            bool       isConnected = true;
        };

        using SubscriptionPtr  = std::shared_ptr<Subscription>;
        using SubscriptionList = std::vector<SubscriptionPtr>;

        SubscriptionList subscriptions  = {};
        ListenerId       nextListenerId = 1;
    };

    template <class... TArgs>
    Delegate<TArgs...>& Delegate<TArgs...>::operator+=(Listener callback) noexcept
    {
        AddListener(std::move(callback));
        return *this;
    }

    template <class... TArgs>
    typename Delegate<TArgs...>::ListenerId Delegate<TArgs...>::AddListener(Listener callback)
    {
        const ListenerId ID = nextListenerId++;
        subscriptions.push_back(std::make_shared<Subscription>(Subscription{ID, std::move(callback), true}));
        return ID;
    }

    template <class... TArgs>
    void Delegate<TArgs...>::RemoveListener(ListenerId id) noexcept
    {
        const auto SUBSCRIPTION = std::ranges::find(subscriptions, id, &Subscription::id);
        if (SUBSCRIPTION != subscriptions.end())
        {
            (*SUBSCRIPTION)->isConnected = false;
            subscriptions.erase(SUBSCRIPTION);
        }
    }

    template <class... TArgs>
    void Delegate<TArgs...>::Broadcast(TArgs... args) const
    {
        const SubscriptionList SNAPSHOT = subscriptions;
        for (const SubscriptionPtr& subscription : SNAPSHOT)
        {
            if (subscription->isConnected && subscription->callback)
            {
                subscription->callback(args...);
            }
        }
    }

    template <class... TArgs>
    void Delegate<TArgs...>::Clear() noexcept
    {
        for (const SubscriptionPtr& subscription : subscriptions)
        {
            subscription->isConnected = false;
        }
        subscriptions.clear();
    }
} // namespace Garage

#endif // GARAGE_CORE_TYPES_HPP
