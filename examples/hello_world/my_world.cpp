#include "my_world.hpp"

void MyWorld::OnEnter(SystemContext& context)
{
    ActorBase* cameraActor = Instantiate(Vector3D<Float32>{0.0f, 0.0f, -2.5f});
    Camera*    camera      = cameraActor->AddComponent<Camera>();
    camera->SetClearColor(ColorRGBA<Float32>{0.1f, 0.14f, 0.2f, 1.0f});
    camera->SetViewport(GPUViewport{0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f});
    camera->SetScissorRect(GPURect{0, 0, 800, 600});

    AssetSubsystem* assets = context.GetAppSubsystem<AssetSubsystem>();
    if (assets == nullptr)
    {
        LOG_ERROR("에셋 서브시스템을 가져오지 못했습니다.");
        return;
    }

    const MeshAsset*     triangleMesh     = assets->LoadAsset<MeshAsset>("Resources/Meshes/Triangle.json");
    const MaterialAsset* triangleMaterial = assets->LoadAsset<MaterialAsset>("Resources/Materials/Triangle.json");
    if (triangleMesh == nullptr || triangleMaterial == nullptr)
    {
        LOG_ERROR("삼각형 에셋(Meshes/Triangle.json / Materials/Triangle.json)을 로드하지 못했습니다.");
        return;
    }

    ActorBase*    triangleActor = Instantiate();
    MeshRenderer* renderer      = triangleActor->AddComponent<MeshRenderer>();
    renderer->SetMesh(triangleMesh);
    renderer->SetMaterial(triangleMaterial);
}
