#pragma once

#include <concepts>

namespace Garage
{
    /**
     * @class Subsystem
     * @brief 라이브러리 내 서브시스템을 정의합니다.
     */
    class Subsystem
    {
    public:
        virtual ~Subsystem() noexcept = default;
    };
    
    /**
     * @brief 서브시스템 타입을 확인하는 개념입니다.
     * @tparam TSubsystem 확인할 타입
     */
    template <class TSubsystem>
    concept FromSubsystem = std::derived_from<TSubsystem, Subsystem>;
}
