#include "my_app.hpp"

#include "my_world.hpp"

ESystemStatus MyApp::Startup(SystemContext& context)
{
    eventSubsystem = context.GetEngineSubsystem<EventSubsystem>();
    if (eventSubsystem == nullptr)
    {
        LOG_ERROR("이벤트 서브시스템을 가져오지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    displaySubsystem = context.GetEngineSubsystem<DisplaySubsystem>();
    if (displaySubsystem == nullptr)
    {
        LOG_ERROR("디스플레이 서브시스템을 가져오지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    renderSubsystem = context.GetEngineSubsystem<RenderSubsystem>();
    if (renderSubsystem == nullptr)
    {
        LOG_ERROR("렌더 서브시스템을 가져오지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    context.OnAppSubsystemAdded<WorldSubsystem>(this, &MyApp::OnWorldSubsystemAdded);
    context.OnAppSubsystemAdded<AssetSubsystem>(this, &MyApp::OnAssetSubsystemAdded);

    worldSubsystem = context.AddAppSubsystem<WorldSubsystem>();
    if (worldSubsystem == nullptr)
    {
        LOG_ERROR("월드 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    assetSubsystem = context.AddAppSubsystem<AssetSubsystem>();
    if (assetSubsystem == nullptr)
    {
        LOG_ERROR("에셋 서브시스템을 추가하지 못했습니다.");
        return ESystemStatus::FAILURE;
    }

    if (const ESystemStatus STATUS = App::Startup(context); STATUS != ESystemStatus::CONTINUE)
    {
        return STATUS;
    }

    DisplayOptions options{};
    options.title    = "Garage Example #1 - Hello, World";
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

void MyApp::OnWorldSubsystemAdded(WorldSubsystem* subsystem)
{
    ASSERT(subsystem != nullptr);

    WorldBase*                  world   = subsystem->AddWorld(MakeUnique<MyWorld>());
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
    subsystem->AddImporter<MaterialAssetImporter>(*device, EGPUTextureFormat::B8G8R8A8_UNORM);
    subsystem->AddImporter<ShaderAssetImporter>(*device);
}
