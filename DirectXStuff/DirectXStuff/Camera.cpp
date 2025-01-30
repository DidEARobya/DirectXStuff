#include "Camera.h"
#include "imgui/imgui.h"

namespace dirx = DirectX;

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const auto pos = dirx::XMVector3Transform(dirx::XMVectorSet(0.0f, 0.0f, -_r, 0.0f),dirx::XMMatrixRotationRollPitchYaw(_phi, -_theta, 0.0f));

	return dirx::XMMatrixLookAtLH(pos, dirx::XMVectorZero(),dirx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) * dirx::XMMatrixRotationRollPitchYaw(_pitch, -_yaw, _roll);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &_r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &_theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &_phi, -89.0f, 89.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &_roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &_pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &_yaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	//zoom
	_r = 20.0f;
	//orbit horizontally
	_theta = 0.0f;
	//orbit vertically
	_phi = 0.0f;

	_pitch = 0.0f;
	_yaw = 0.0f;
	_roll = 0.0f;
}
