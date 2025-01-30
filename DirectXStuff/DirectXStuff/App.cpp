#include "App.h"
#include "Box.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Sheet.h"
#include "TexturedBox.h"
#include <memory>
#include <algorithm>
#include "WinMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"

GDIPlusManager gdipManager;

App::App() : _window(800, 600, "Main App")
{
	class Factory
	{
	public:
		Factory(Graphics& graphics) : _graphics(graphics) {}

		std::unique_ptr<Drawable> operator()()
		{
			switch (_typedist(_rng))
			{
			case 0:
				return std::make_unique<Pyramid>(_graphics, _rng, _adist, _ddist, _odist, _rdist);
			case 1:
				return std::make_unique<Box>(_graphics, _rng, _adist, _ddist, _odist, _rdist, _bdist);
			case 2:
				return std::make_unique<Melon>(_graphics, _rng, _adist, _ddist, _odist, _rdist, _longdist, _latdist);
			case 3:
				return std::make_unique<Sheet>(_graphics, _rng, _adist, _ddist, _odist, _rdist);
			case 4:
				return std::make_unique<TexturedBox>(_graphics, _rng, _adist, _ddist, _odist, _rdist);
			default:
				assert(false && "Bad drawable type in factory");
				return {};
			}
		}

	private:
		Graphics& _graphics;
		std::mt19937 _rng{ std::random_device{}() };
		std::uniform_real_distribution<float> _adist{ 0.0f, PI * 2.0f };
		std::uniform_real_distribution<float> _ddist{ 0.0f, PI * 0.5f };
		std::uniform_real_distribution<float> _odist{ 0.0f, PI * 0.08f };
		std::uniform_real_distribution<float> _rdist{ 6.0f, 20.0f};
		std::uniform_real_distribution<float> _bdist{ 0.4f, 3.0f };
		std::uniform_int_distribution<int> _latdist{ 5, 20 };
		std::uniform_int_distribution<int> _longdist{ 10, 40 };
		std::uniform_int_distribution<int> _typedist{ 0, 4 };
	};

	Factory factory(_window.GetGraphics());
	_drawables.reserve(_nDrawable);
	std::generate_n(std::back_inserter(_drawables), _nDrawable, factory);

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
	auto deltaTime = _timer.Mark();
	_window.GetGraphics().ClearBuffer(0.07f, 0.0f, 0.12f);

	for (auto& cube : _drawables)
	{
		cube->Update(deltaTime);
		cube->Draw(_window.GetGraphics());
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool show_demo_window = true;
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	_window.GetGraphics().EndFrame();
}
