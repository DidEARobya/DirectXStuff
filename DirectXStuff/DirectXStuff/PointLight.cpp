#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(Graphics& graphics, float radius) : _mesh(graphics, radius), _cBuffer(graphics)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &_cBufferData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &_cBufferData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &_cBufferData.pos.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Intensity/Colour");
		ImGui::SliderFloat("Intensity", &_cBufferData.diffuseIntensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit3("Diffuse Colour", &_cBufferData.diffuseColour.x);
		ImGui::ColorEdit3("Ambient", &_cBufferData.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &_cBufferData.attenuationConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Linear", &_cBufferData.attenuationLin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Quadratic", &_cBufferData.attenuationQuad, 0.0000001f, 10.0f, "%.7f", ImGuiSliderFlags_Logarithmic);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}

	ImGui::End();
}

void PointLight::Reset() noexcept
{
	_cBufferData = 
	{ 
		{ 0.0f,0.0f,0.0f},
		{ 0.05f,0.05f,0.05f},
		{ 1.0f,1.0f,1.0f },
		1.0f, 
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Draw(Graphics& graphics) const noexcept(!IS_DEBUG)
{
	_mesh.SetPos(_cBufferData.pos);
	_mesh.Draw(graphics);
}

void PointLight::Bind(Graphics& graphics, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = _cBufferData;
	const auto pos = DirectX::XMLoadFloat3(&dataCopy.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));

	_cBuffer.Update(graphics, dataCopy);
	_cBuffer.Bind(graphics);
}