#include "ExampleLayer3D.h"
#include "imgui/imgui.h"

class Rotator : public ScriptableEntity
{
public:
	Rotator() : m_Rotation() {}
	Rotator(Vector3f rotation) : m_Rotation(rotation) {}
	void OnFixedUpdate() override
	{
		GetComponent<TransformComponent>().rotation += m_Rotation;
	}
	void OnUpdate(float deltaTime)override
	{
		GetComponent<TransformComponent>().rotation += m_Rotation * deltaTime;
	}

	void SetRotation(Vector3f rotation) { m_Rotation = rotation; }
private:
	Vector3f m_Rotation;
	static ScriptRegister<Rotator> reg;
};

ScriptRegister<Rotator> Rotator::reg("Rotator Script");



ExampleLayer3D::ExampleLayer3D()
	:Layer("Example 3D")
{
	m_FOV = new float(1.0f);
}

void ExampleLayer3D::OnAttach()
{
	SceneManager::ChangeScene(std::string("Example 3D"));

	m_CubeVertexArray = GeometryGenerator::CreateCube(1.0f, 1.0f, 1.0f);
	m_SphereVertexArray = GeometryGenerator::CreateSphere(0.5f, 50, 50);
	m_GridVertexArray = GeometryGenerator::CreateGrid(100.0f, 100.0f, 5, 5, 2.0f, 2.0f);
	m_CylinderVertexArray = GeometryGenerator::CreateCylinder(0.25f, 0.1f, 1.0f, 20, 1);
	m_TorusVertexArray = GeometryGenerator::CreateTorus(0.7f, 0.25f, 30);

	m_Texture = Texture2D::Create(Application::Get().GetWorkingDirectory() / "resources" / "UVChecker.png");

	m_CameraController.SetPosition({ 0.0,0.0,10.0 });

	Ref<Material> material = CreateRef<Material>("Standard", Colours::WHITE);
	material->AddTexture(m_Texture, 0);
	
	Entity entity = SceneManager::CurrentScene()->CreateEntity("Cube");
	entity.AddComponent<StaticMeshComponent>(CreateRef<Mesh>(), material);
	entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cube);
	entity.GetComponent<TransformComponent>().position = { m_Position[0], m_Position[1] - 1.0f, m_Position[2] };
	entity.AddComponent<NativeScriptComponent>().Bind<Rotator>(Vector3f(0.0011f, 0.0014f, 0.002f));

	entity = SceneManager::CurrentScene()->CreateEntity("Sphere");
	entity.AddComponent<StaticMeshComponent>(CreateRef<Mesh>(), material);
	entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Sphere);
	entity.GetComponent<TransformComponent>().position = { m_Position[0], m_Position[1] + 1.0f, m_Position[2] };
	entity.AddComponent<NativeScriptComponent>().Bind<Rotator>(Vector3f(-0.0011f, 0.0014f, 0.002f));

	entity = SceneManager::CurrentScene()->CreateEntity("Cylinder");
	entity.AddComponent<StaticMeshComponent>(CreateRef<Mesh>(), material);
	entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cylinder);
	entity.GetComponent<TransformComponent>().position = { m_Position[0] - 2.0f, m_Position[1] + 1.0f, m_Position[2] };
	entity.AddComponent<NativeScriptComponent>().Bind<Rotator>(Vector3f(0.0011f, -0.0014f, 0.002f));

	entity = SceneManager::CurrentScene()->CreateEntity("Torus");
	entity.AddComponent<StaticMeshComponent>(CreateRef<Mesh>(), material);
	entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Torus);
	entity.GetComponent<TransformComponent>().position = { m_Position[0] - 2.0f, m_Position[1] - 1.0f, m_Position[2] };
	entity.AddComponent<NativeScriptComponent>().Bind<Rotator>(Vector3f(0.0011f, 0.0014f, -0.002f));

	entity = SceneManager::CurrentScene()->CreateEntity("Plane");
	entity.AddComponent<StaticMeshComponent>(CreateRef<Mesh>(), material);
	entity.AddComponent<PrimitiveComponent>(100.0f, 100.0f, 5, 5, 2.0f, 2.0f);
	entity.GetComponent<TransformComponent>().position = { m_Position[0], m_Position[1], m_Position[2] - 0.5f };
	entity.GetComponent<TransformComponent>().rotation = { (float)PI / 2.0f, 0.0f, 0.0f };

	entity = SceneManager::CurrentScene()->CreateEntity("Camera");
	SceneCamera camera;
	camera.SetPerspective(45, 1, 100);
	entity.AddComponent<CameraComponent>(camera, true, false);
	entity.GetComponent<TransformComponent>().position = { 0.0,0.0,10.0 };
}

void ExampleLayer3D::OnDetach()
{
}

void ExampleLayer3D::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	SceneManager::CurrentScene()->Render(nullptr);

	/*

	m_CameraController.SetFovY(*m_FOV);
	m_CameraController.SetNearAndFarDepth(m_Nearfar[0], m_Nearfar[1]);

	if (m_ControlCamera)
	{
		m_CameraController.OnUpdate(deltaTime);
	}

	Renderer::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera().GetProjectionMatrix());
	*/
}

void ExampleLayer3D::OnFixedUpdate()
{
}

void ExampleLayer3D::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);
}

void ExampleLayer3D::OnImGuiRender()
{
	//TODO: get the information from the scene
	ImGui::Begin("Settings 3D");
	ImGui::Checkbox("Control Camera", &m_ControlCamera);
	ImGui::Text("%s", std::to_string(m_CameraController.GetTranslationSpeed()).c_str());
	ImGui::DragFloat3("Cube Position", m_Position, 0.01f);
	ImGui::DragFloat3("Cube Rotation", m_Rotation, 0.001f);
	ImGui::DragFloat("Fov", m_FOV, 0.01f, 0.0f, (float)PI);
	ImGui::DragFloat2("near far depth", m_Nearfar, 1.0f);
	ImGui::End();
}