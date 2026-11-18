#include "terrain_world.hpp"

#include "terrain_camera_controller.hpp"

void TerrainWorld::OnEnter()
{
    App*            app    = App::GetInstance();
    AssetSubsystem* assets = (app != nullptr) ? app->GetSubsystem<AssetSubsystem>() : nullptr;
    if (assets == nullptr)
    {
        LOG_ERROR("에셋 서브시스템을 가져오지 못했습니다.");
        return;
    }

    const TerrainAsset*  terrainAsset    = assets->LoadAsset<TerrainAsset>("Resources/Terrains/Terrain_LevelTank.json");
    const MaterialAsset* terrainMaterial = assets->LoadAsset<MaterialAsset>("Resources/Materials/ForrestGround.json");
    const CubemapAsset*  skyboxCubemap =
            assets->LoadAsset<CubemapAsset>("Resources/Cubemaps/Polyverse Skies - Blue Sky.dds");
    const MaterialAsset* skyboxMaterial = assets->LoadAsset<MaterialAsset>("Resources/Materials/Skybox.json");
    if (terrainAsset == nullptr || terrainMaterial == nullptr || skyboxCubemap == nullptr || skyboxMaterial == nullptr)
    {
        LOG_ERROR("터레인 또는 스카이박스 에셋을 로드하지 못했습니다.");
        return;
    }

    ActorBase* terrainActor = Instantiate();
    terrainActor->SetName("Terrain");
    terrainActor->SetTag("Terrain");
    Terrain* terrain = terrainActor->AddComponent<Terrain>();
    terrain->SetTerrain(terrainAsset);
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

    TerrainCameraController* cameraController = cameraActor->AddComponent<TerrainCameraController>();
    cameraController->SetTerrain(terrain);
}
