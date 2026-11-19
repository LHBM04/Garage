#include "my_engine.hpp"

using namespace Garage;

ESystemStatus MyEngine::Startup(SystemContext& context)
{
    if (context.AddEngineSubsystem<TimeSubsystem>() == nullptr)
    {
        LOG_ERROR("타임 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    displays = context.AddEngineSubsystem<DisplaySubsystem>();
    if (displays == nullptr)
    {
        LOG_ERROR("디스플레이 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    events = context.AddEngineSubsystem<EventSubsystem>();
    if (events == nullptr)
    {
        LOG_ERROR("이벤트 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    rendering = context.AddEngineSubsystem<RenderSubsystem>();
    if (rendering == nullptr)
    {
        LOG_ERROR("렌더링 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    return Engine::Startup(context);
}
