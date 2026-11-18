#include "cube_world.hpp"

#include "cube_controller.hpp"

void CubeWorld::OnEnter()
{
    ActorBase* cameraActor =
            Instantiate(Vector3D<Float32>{0.0f, 0.8f, -3.2f}, Quaternion<Float32>::Euler(14.0f, 0.0f, 0.0f));
    Camera* camera = cameraActor->AddComponent<Camera>();
    camera->SetClearColor(ColorRGBA<Float32>{0.1f, 0.14f, 0.2f, 1.0f});
    camera->SetViewport(GPUViewport{0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f});
    camera->SetScissorRect(GPURect{0, 0, 800, 600});

    ActorBase*        lightActor = Instantiate();
    DirectionalLight* light      = lightActor->AddComponent<DirectionalLight>();
    light->SetUseTransformRotation(false);
    light->SetDirection(Vector3D<Float32>{-0.4f, -0.8f, 0.7f});
    light->SetIntensity(1.0f);
    light->SetColor(ColorRGBA<Float32>::GetWhite());
    light->SetAmbientColor(ColorRGBA<Float32>{0.28f, 0.28f, 0.32f, 1.0f});

    App*            app    = App::GetInstance();
    AssetSubsystem* assets = (app != nullptr) ? app->GetSubsystem<AssetSubsystem>() : nullptr;
    if (assets == nullptr)
    {
        LOG_ERROR("에셋 서브시스템을 가져오지 못했습니다.");
        return;
    }

    const MeshAsset*     cubeMesh     = assets->LoadAsset<MeshAsset>("Resources/Meshes/Cube.tukmesh");
    const MaterialAsset* cubeMaterial = assets->LoadAsset<MaterialAsset>("Resources/Materials/Cube.tukmat");
    if (cubeMesh == nullptr || cubeMaterial == nullptr)
    {
        LOG_ERROR("큐브 에셋(Cube.tukmesh / Cube.tukmat)을 로드하지 못했습니다.");
        return;
    }

    ActorBase* cubeActor =
            Instantiate(Vector3D<Float32>{0.0f, 0.0f, 0.0f}, Quaternion<Float32>::Euler(20.0f, 35.0f, 0.0f));
    MeshRenderer* renderer = cubeActor->AddComponent<MeshRenderer>();
    renderer->SetMesh(cubeMesh);
    renderer->SetMaterial(cubeMaterial);
    cubeActor->AddComponent<CubeController>();
}
