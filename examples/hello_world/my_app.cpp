#include "my_app.hpp"

ESystemStatus MyApp::Startup()
{
    Engine* engine = Engine::GetInstance();
    if (engine == nullptr)
    {
        LOG_ERROR("엔진 인스턴스를 가져오지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    eventSubsystem = engine->GetSubsystem<EventSubsystem>();
    if (eventSubsystem == nullptr)
    {
        LOG_ERROR("이벤트 서브시스템을 가져오지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    displaySubsystem = engine->GetSubsystem<DisplaySubsystem>();
    if (displaySubsystem == nullptr)
    {
        LOG_ERROR("디스플레이 서브시스템을 가져오지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    if (const ESystemStatus STATUS = App::Startup(); STATUS != ESystemStatus::CONTINUE)
    {
        return STATUS;
    }

    DisplayOptions options{};
    options.title    = Allocator::MakeString("Garage Example #1 - Hello, World");
    options.position = Vector2DInt{100, 100};
    options.size     = Vector2DInt{800, 600};
    options.centered = true;
    options.visible  = true;

    IDisplay* display = displaySubsystem->CreateDisplay(options);
    if (display == nullptr)
    {
        LOG_ERROR("디스플레이 생성에 실패했습니다.");
        return ESystemStatus::FAILURE;
    }

    return ESystemStatus::CONTINUE;
}
