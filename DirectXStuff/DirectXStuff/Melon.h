#pragma once
#include "DrawableBase.h"

class Melon : public DrawableBase<Melon>
{
public:
	Melon(Graphics& graphics, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_int_distribution<int>& longdist,
		std::uniform_int_distribution<int>& latdist);
	void Update(float deltaTime) noexcept override;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;
private:
	// positional
	float _r;
	float _roll = 0.0f;
	float _pitch = 0.0f;
	float _yaw = 0.0f;
	float _theta;
	float _phi;
	float _chi;
	// speed
	float _droll;
	float _dpitch;
	float _dyaw;
	float _dtheta;
	float _dphi;
	float _dchi;
};

