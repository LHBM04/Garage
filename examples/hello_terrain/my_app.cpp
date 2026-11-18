#include "my_app.hpp"

#include "my_world.hpp"

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

    renderSubsystem = engine->GetSubsystem<RenderSubsystem>();
    if (renderSubsystem == nullptr)
    {
        LOG_ERROR("렌더 서브시스템을 가져오지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    OnSubsystemAdded<InputSubsystem>(this, &MyApp::OnInputSubsystemAdded);
    OnSubsystemAdded<WorldSubsystem>(this, &MyApp::OnWorldSubsystemAdded);
    OnSubsystemAdded<AssetSubsystem>(this, &MyApp::OnAssetSubsystemAdded);

    inputSubsystem = AddSubsystem<InputSubsystem>();
    if (inputSubsystem == nullptr)
    {
        LOG_ERROR("입력 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    worldSubsystem = AddSubsystem<WorldSubsystem>();
    if (worldSubsystem == nullptr)
    {
        LOG_ERROR("월드 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    assetSubsystem = AddSubsystem<AssetSubsystem>();
    if (assetSubsystem == nullptr)
    {
        LOG_ERROR("에셋 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    if (const ESystemStatus STATUS = App::Startup(); STATUS != ESystemStatus::CONTINUE)
    {
        return STATUS;
    }

    DisplayOptions options{};
    options.title    = Allocator::MakeString("Garage Example #3 - Hello, Terrain");
    options.position = Vector2DInt{100, 100};
    options.size     = Vector2DInt{1280, 720};
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

void MyApp::OnInputSubsystemAdded(InputSubsystem* subsystem)
{
    ASSERT(subsystem != nullptr);

    if (InputAction* moveAction = subsystem->AddAction("Move"); moveAction != nullptr)
    {
        moveAction->AddVector2DBinding(EKeyCode::W, EKeyCode::S, EKeyCode::A, EKeyCode::D);
    }

    if (InputAction* lookAction = subsystem->AddAction("Look"); lookAction != nullptr)
    {
        lookAction->AddVector2DBinding(EKeyCode::UP, EKeyCode::DOWN, EKeyCode::LEFT, EKeyCode::RIGHT);
    }
}

void MyApp::OnWorldSubsystemAdded(WorldSubsystem* subsystem)
{
    ASSERT(subsystem != nullptr);

    WorldBase*                  world   = subsystem->AddWorld(Allocator::MakeUnique<MyWorld>());
    [[maybe_unused]] const bool CHANGED = subsystem->RequestWorldChange(world);
}

void MyApp::OnAssetSubsystemAdded(AssetSubsystem* subsystem)
{
    ASSERT(subsystem != nullptr);
    ASSERT(renderSubsystem != nullptr);

    IGPUDevice* device = renderSubsystem->GetDevice();
    ASSERT(device != nullptr);

    subsystem->AddImporter<MeshAssetImporter>(*device);
    subsystem->AddImporter<TextureAssetImporter>(*device);
    subsystem->AddImporter<CubemapAssetImporter>(*device);
    subsystem->AddImporter<MaterialAssetImporter>(*device, EGPUTextureFormat::B8G8R8A8_UNORM);
    subsystem->AddImporter<ShaderAssetImporter>(*device);
    subsystem->AddImporter<TerrainAssetImporter>(*device);
}
