#include "SFMLApp.h"

#ifdef TRACY_ENABLE
#include "Tracy.hpp"
#include "TracyC.h"
#endif

void SFMLApp::SetUp() {
	_window.create(sf::VideoMode(Width, Height), Title);
	ImGui::SFML::Init(_window);

	_sampleManager.SetUp();
}

void SFMLApp::TearDown() const noexcept { ImGui::SFML::Shutdown(); }

void SFMLApp::Run() noexcept {
	bool quit = false;

	bool adjustWindow = true;

	sf::Event e;

	while (!quit) {
		while (_window.pollEvent(e)) {
			ImGui::SFML::ProcessEvent(e);
			switch (e.type) {
			case sf::Event::Closed:
				quit = true;
				break;
			case sf::Event::KeyReleased:
				switch (e.key.code) {
				case sf::Keyboard::Left:
					_sampleManager.PreviousSample();
					break;
				case sf::Keyboard::Right:
					_sampleManager.NextSample();
					break;
				case sf::Keyboard::Space:
					_sampleManager.RegenerateSample();
					break;
				}
				break;
			case sf::Event::MouseButtonReleased:
				if (e.mouseButton.button == sf::Mouse::Left) {
					_sampleManager.GiveLeftMouseClickToSample();
				}
				else if (e.mouseButton.button == sf::Mouse::Right) {
					_sampleManager.GiveRightMouseClickToSample();
				}
				break;
			}
		}

		ImGui::SFML::Update(_window, _deltaClock.restart());

		if (adjustWindow) {
			ImGui::SetNextWindowSize(ImVec2(Metrics::Width / 3, Metrics::Height / 5));
			adjustWindow = false;
		}

		ImGui::Begin("Sample Manager");

		if (ImGui::BeginCombo(
			"Select a Sample",
			_sampleManager.GetSampleName(_sampleManager.GetCurrentIndex())
			.c_str())) {
			for (std::size_t index = 0; index < _sampleManager.GetSampleNbr();
				index++) {
				if (ImGui::Selectable(_sampleManager.GetSampleName(index).c_str(),
					_sampleManager.GetCurrentIndex() == index)) {
					_sampleManager.ChangeSample(index);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Spacing();

		ImGui::TextWrapped(
			_sampleManager.GetSampleDescription(_sampleManager.GetCurrentIndex())
			.c_str());

		ImGui::Spacing();

		ImGui::SetCursorPosY(ImGui::GetWindowHeight() -
			(ImGui::GetFrameHeightWithSpacing()));

		if (ImGui::ArrowButton("PreviousSample", ImGuiDir_Left)) {
			_sampleManager.PreviousSample();
		}

		ImGui::SameLine();

		if (ImGui::Button("Regenerate")) {
			_sampleManager.RegenerateSample();
		}

		ImGui::SameLine();

		if (ImGui::ArrowButton("NextSample", ImGuiDir_Right)) {
			_sampleManager.NextSample();
		}

		ImGui::End();

		_window.clear(sf::Color::Black);

		sf::Vector2i sfMousePos = sf::Mouse::getPosition(_window);
		MousePos = XMVectorSet(sfMousePos.x, sfMousePos.y, 0, 0);
		_sampleManager.GiveMousePositionToSample(MousePos);
		_sampleManager.UpdateSample();

		DrawAllGraphicsData();

		ImGui::SFML::Render(_window);

		_window.display();

#ifdef TRACY_ENABLE
		FrameMark;
#endif
	}
}

void SFMLApp::DrawSphere(const XMVECTOR center, const float radius,
	const int segments, const sf::Color& col) noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif

	sf::CircleShape sphere = sf::CircleShape(radius, segments);
	sphere.setFillColor(col);
	sphere.setOrigin(radius, radius);
	sphere.setPosition(XMVectorGetX(center), XMVectorGetY(center));

	_window.draw(sphere);
}

void SFMLApp::Drawcuboid(const XMVECTOR minBound,
	const XMVECTOR maxBound,
	const sf::Color& col) noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif

	// Calculate the width and height of the cuboid
	float width = XMVectorGetX(maxBound) - XMVectorGetX(minBound);
	float height = XMVectorGetY(maxBound) - XMVectorGetY(minBound);

	// Create an SFML cuboid shape
	sf::RectangleShape cuboid(sf::Vector2f(width, height));

	// Set the position and color of the cuboid
	cuboid.setPosition(XMVectorGetX(minBound), XMVectorGetY(minBound));
	cuboid.setFillColor(col);

	// Draw the cuboid to the window
	_window.draw(cuboid);
}

void SFMLApp::DrawcuboidBorder(const XMVECTOR minBound, const XMVECTOR maxBound,
	const sf::Color& col) noexcept {
	// Calculate the width and height of the cuboid
	float width = XMVectorGetX(maxBound) - XMVectorGetX(minBound);
	float height = XMVectorGetY(maxBound) - XMVectorGetY(minBound);

	// Create an SFML cuboid shape
	sf::RectangleShape cuboid(sf::Vector2f(width, height));

	// Set the position and color of the cuboid
	cuboid.setPosition(XMVectorGetX(minBound), XMVectorGetY(minBound));
	cuboid.setOutlineColor(col);
	cuboid.setOutlineThickness(1);
	cuboid.setFillColor(sf::Color::Transparent);

	// Draw the cuboid to the window
	_window.draw(cuboid);
}

void SFMLApp::DrawAllGraphicsData() noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for (auto& bd : _sampleManager.GetSampleData()) {
		if (bd.Shape.index() == (int)ShapeType::Sphere) {
			auto& sphere = std::get<SphereF>(bd.Shape);
			DrawSphere(sphere.Center(), sphere.Radius(), 30,
				{ static_cast<sf::Uint8>(bd.Color.r),
				 static_cast<sf::Uint8>(bd.Color.g),
				 static_cast<sf::Uint8>(bd.Color.b),
				 static_cast<sf::Uint8>(bd.Color.a) });
		}
		else if (bd.Shape.index() == (int)ShapeType::Cuboid) {
			auto& rect = std::get<CuboidF>(bd.Shape);
			if (!bd.Filled) {
				DrawcuboidBorder(rect.MinBound(), rect.MaxBound(),
					{ static_cast<sf::Uint8>(bd.Color.r),
					 static_cast<sf::Uint8>(bd.Color.g),
					 static_cast<sf::Uint8>(bd.Color.b),
					 static_cast<sf::Uint8>(bd.Color.a) });
			}
			else {
				Drawcuboid(rect.MinBound(), rect.MaxBound(),
					{ static_cast<sf::Uint8>(bd.Color.r),
					 static_cast<sf::Uint8>(bd.Color.g),
					 static_cast<sf::Uint8>(bd.Color.b),
					 static_cast<sf::Uint8>(bd.Color.a) });
			}
		}
	}
}
