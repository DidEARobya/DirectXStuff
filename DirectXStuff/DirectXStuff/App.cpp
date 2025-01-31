#include "App.h"
#include "Box.h"
#include "Cylinder.h"
#include <memory>
#include <algorithm>
#include "WinMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

GDIPlusManager gdipManager;

App::App() : _window(800, 600, "Main App"), _pointLight(_window.GetGraphics())
{
	class Factory
	{
	public:
		Factory(Graphics& graphics) : _graphics(graphics) {}

		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 material = {_cdist(_rng), _cdist(_rng), _cdist(_rng)};

			switch (_typedist(_rng))
			{
			case 0:
				return std::make_unique<Box>(_graphics, _rng, _adist, _ddist, _odist, _rdist, _bdist, material);

			case 1:
				return std::make_unique<Cylinder>(_graphics, _rng, _adist, _ddist, _odist, _rdist, _bdist, _tdist);

			default:
				assert(false && "impossible drawable option in factory");
				return {};
			}
		}

	private:
		Graphics& _graphics;
		std::mt19937 _rng{ std::random_device{}() };
		std::uniform_int_distribution<int> _typedist{ 0, 1 };

		std::uniform_real_distribution<float> _adist{ 0.0f, PI * 2.0f };
		std::uniform_real_distribution<float> _ddist{ 0.0f, PI * 0.5f };
		std::uniform_real_distribution<float> _odist{ 0.0f, PI * 0.08f };
		std::uniform_real_distribution<float> _rdist{ 6.0f, 20.0f};
		std::uniform_real_distribution<float> _bdist{ 0.4f, 3.0f };
		std::uniform_real_distribution<float> _cdist{ 0.0f, 1.0f };
		std::uniform_int_distribution<int> _tdist{ 3, 30 };
	};

	_drawables.reserve(_nDrawable);
	std::generate_n(std::back_inserter(_drawables), _nDrawable, Factory(_window.GetGraphics()));

	_window.GetGraphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
{
}

int App::StartApp()
{
	while (true)
	{
		//If ecode has a value, there is an error or the application wants to quit, else call Update()
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		Update();
	}
}
//Executes frame logic
void App::Update()
{
	const auto deltaTime = _timer.Mark() * _speedFactor;
	_window.GetGraphics().BeginFrame(0.07f, 0.0f, 0.12f);
	_window.GetGraphics().SetCamera(_camera.GetMatrix());
	_pointLight.Bind(_window.GetGraphics(), _camera.GetMatrix());

	const bool _isSpacePressed = _window.keyboard.IsKeyPressed(VK_SPACE);

	for (auto& cube : _drawables)
	{
		cube->Update(_isSpacePressed ? 0.0f : deltaTime);
		cube->Draw(_window.GetGraphics());
	}

	_pointLight.Draw(_window.GetGraphics());

	static char buffer[1024];

	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &_speedFactor, 0.0f, 4.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", _isSpacePressed ? "PAUSED" : "RUNNING (hold spacebar to pause)");
	}

	ImGui::End();

	_camera.SpawnControlWindow();
	_pointLight.SpawnControlWindow();

	_window.GetGraphics().EndFrame();
}
