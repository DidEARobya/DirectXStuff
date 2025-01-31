#pragma once
#include "DrawableBase.h"

template <class T>
class BaseObject : public DrawableBase<T>
{
public:
	BaseObject(Graphics& graphics, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist)
		:
		_r(rdist(rng)),
		_droll(ddist(rng)),
		_dpitch(ddist(rng)),
		_dyaw(ddist(rng)),
		_dphi(odist(rng)),
		_dtheta(odist(rng)),
		_dchi(odist(rng)),
		_chi(adist(rng)),
		_theta(adist(rng)),
		_phi(adist(rng))
	{

	}
	void Update(float deltaTime) noexcept
	{
		_roll += _droll * deltaTime;
		_pitch += _dpitch * deltaTime;
		_yaw += _dyaw * deltaTime;
		_theta += _dtheta * deltaTime;
		_phi += _dphi * deltaTime;
		_chi += _dchi * deltaTime;
	}
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override
	{
		namespace dirx = DirectX;
		return dirx::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) *
			dirx::XMMatrixTranslation(_r, 0.0f, 0.0f) *
			dirx::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi);
	}
private:
	//position
	float _r;
	float _roll = 0.0f;
	float _pitch = 0.0f;
	float _yaw = 0.0f;
	float _theta;
	float _phi;
	float _chi;

	//speed
	float _droll;
	float _dpitch;
	float _dyaw;
	float _dtheta;
	float _dphi;
	float _dchi;
};