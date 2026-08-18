#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "subsystem.hpp"

namespace Garage
{
    /**
     * @class Engine
     * @brief 애플리케이션 내 서브시스템을 관리하는 엔진을 정의합니다.
     */
    class Engine
    {
    public:
        /**
         * @brief  해당 엔진에 지정한 타입의 서브시스템 인스턴스를 추가합니다.
         * @tparam TSubsystem 지정할 서브시스템 타입
         */
        template <FromSubsystem TSubsystem>
        void AddSubsystem();

        /**
         * @brief 해당 엔진으로부터 지정한 타입의 서브시스템 인스턴스를 가져옵니다.
         * @tparam TSubsystem 지정할 서브시스템 타입
         * @return TSubsystem* 해당 타입의 서브시스템 인스턴스
         */
        template <FromSubsystem TSubsystem>
        TSubsystem* GetSubsystem();
        
        /**
         * @brief 해당 엔진으로부터 지정한 타입의 서브시스템 인스턴스를 가져옵니다.
         * @tparam TSubsystem 지정할 서브시스템 타입
         * @return TSubsystem* 해당 타입의 서브시스템 인스턴스
         */
        template <FromSubsystem TSubsystem>
        const TSubsystem* GetSubsystem() const;
        
    private:
        /**
         * @brief 해당 엔진이 관리할 모든 서브시스템.
         */
        std::pmr::vector<std::unique_ptr<Subsystem>> subsystems;

        /**
         * @brief 타입별 서브시스템 매핑.
         */
        std::pmr::unordered_map<std::type_index, Subsystem*> subsystemsByType;
    };
    
    template <FromSubsystem TSubsystem>
    void Engine::AddSubsystem()
    {
        subsystems.emplace_back(std::make_unique<TSubsystem>());
    }
    
    template <FromSubsystem TSubsystem>
    TSubsystem* Engine::GetSubsystem()
    {
        return dynamic_cast<TSubsystem*>(subsystemsByType.at(std::type_index(typeid(TSubsystem))));
    }
    
    template <FromSubsystem TSubsystem>
    const TSubsystem* Engine::GetSubsystem() const
    {
        return dynamic_cast<const TSubsystem*>(subsystemsByType.at(std::type_index(typeid(TSubsystem))));
    }
}
