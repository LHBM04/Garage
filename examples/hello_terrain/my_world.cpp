#include "my_world.hpp"

#include "garage/core/memory/allocation_trace.hpp"

#include "camera_controller.hpp"

void MyWorld::OnEnter(SystemContext& context)
{
    ContentSubsystem* contentSubsystem = context.GetAppSubsystem<ContentSubsystem>();
    if (contentSubsystem == nullptr)
    {
        LOG_ERROR("콘텐츠 서브시스템을 가져오지 못했습니다.");
        return;
    }

    const TerrainContent* terrainContent =
            contentSubsystem->LoadContent<TerrainContent>("content/Terrains/Terrain_LevelTank.json");
    const MaterialContent* terrainMaterial =
            contentSubsystem->LoadContent<MaterialContent>("content/Materials/ForrestGround.json");
    const CubemapContent* skyboxCubemap =
            contentSubsystem->LoadContent<CubemapContent>("content/Cubemaps/Polyverse Skies - Blue Sky.dds");
    const MaterialContent* skyboxMaterial =
            contentSubsystem->LoadContent<MaterialContent>("content/Materials/Skybox.json");
#if defined(GARAGE_DEBUG)
    Memory::DumpAllocationTrace();
#endif
    if (terrainContent == nullptr || terrainMaterial == nullptr || skyboxCubemap == nullptr ||
        skyboxMaterial == nullptr)
    {
        LOG_ERROR("터레인 또는 스카이박스 콘텐츠를 로드하지 못했습니다.");
        return;
    }

    ActorBase* terrainActor = Instantiate();
    terrainActor->SetName("Terrain");
    terrainActor->SetTag("Terrain");
    Terrain* terrain = terrainActor->AddComponent<Terrain>();
    terrain->SetTerrain(terrainContent);
    MeshRenderer* terrainRenderer = terrainActor->AddComponent<MeshRenderer>();
    terrainRenderer->SetMaterial(terrainMaterial);

    ActorBase* skyboxActor = Instantiate();
    skyboxActor->SetName("Skybox");
    skyboxActor->SetTag("Skybox");
    SkyboxRenderer* skyboxRenderer = skyboxActor->AddComponent<SkyboxRenderer>();
    skyboxRenderer->SetCubemap(skyboxCubemap);
    skyboxRenderer->SetMaterial(skyboxMaterial);

    ActorBase* lightActor =
            Instantiate(Vector3D<Float32>{0.0f, 150.0f, 0.0f}, Quaternion<Float32>::Euler(55.0f, -45.0f, 0.0f));
    lightActor->SetName("Directional Light");
    lightActor->SetTag("Light");
    DirectionalLight* light = lightActor->AddComponent<DirectionalLight>();
    light->SetColor(ColorRGBA<Float32>{1.0f, 0.98f, 0.92f, 1.0f});
    light->SetIntensity(1.2f);
    light->SetAmbientColor(ColorRGBA<Float32>{0.22f, 0.24f, 0.28f, 1.0f});

    const Vector3D<Float32> INITIAL_CAMERA_XZ{0.0f, 0.0f, -40.0f};
    const Float32 TERRAIN_HEIGHT = terrain->IsValid() ? terrain->SampleHeightAtWorldPosition(INITIAL_CAMERA_XZ) : 0.0f;

    ActorBase* cameraActor = Instantiate(Vector3D<Float32>{0.0f, TERRAIN_HEIGHT + 18.0f, -40.0f},
                                         Quaternion<Float32>::Euler(10.0f, 0.0f, 0.0f));
    cameraActor->SetName("Main Camera");
    cameraActor->SetTag("Main Camera");

    Camera* camera = cameraActor->AddComponent<Camera>();
    camera->SetClearColor(ColorRGBA<Float32>{0.35f, 0.55f, 0.85f, 1.0f});
    camera->SetViewport(GPUViewport{0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f});
    camera->SetScissorRect(GPURect{0, 0, 1280, 720});
    camera->SetFarPlane(2000.0f);

    CameraController* cameraController = cameraActor->AddComponent<CameraController>();
    cameraController->SetTerrain(terrain);
}
