#include "my_engine.hpp"

using namespace Garage;

ESystemStatus MyEngine::Startup()
{
    if (AddSubsystem<TimeSubsystem>() == nullptr)
    {
        LOG_ERROR("타임 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    displays = AddSubsystem<DisplaySubsystem>();
    if (displays == nullptr)
    {
        LOG_ERROR("디스플레이 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    events = AddSubsystem<EventSubsystem>();
    if (events == nullptr)
    {
        LOG_ERROR("이벤트 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    rendering = AddSubsystem<RenderSubsystem>();
    if (rendering == nullptr)
    {
        LOG_ERROR("렌더링 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    return Engine::Startup();
}
