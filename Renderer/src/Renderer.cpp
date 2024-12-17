#include "Renderer.h"


void Renderer::Resize(int w, int h)
{

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 1000.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void Renderer::drawSnowMan()
{

	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw Body

	glTranslatef(0.0f, 0.75f, 0.0f);
	glutSolidSphere(0.75f, 20, 20);

	// Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f, 20, 20);

	// Draw Eyes
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f, 10, 10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();

	// Draw Nose
	glColor3f(1.0f, 0.5f, 0.5f);
	glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f, 0.5f, 10, 2);
}

void Renderer::drawSphere()
{
	glColor3f(0.f, 0.f, 1.0f);
	glTranslatef(0.0f, 0.75f, 0.0f);
	glutSolidSphere(0.75f, 20, 20);
}

void Renderer::renderScene(void)
{

	if (camera.deltaMove)
		camera.computePos(camera.deltaMove);
	if (camera.deltaAngle)
		camera.computeDir(camera.deltaAngle);

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	camera.Init();

	// Draw ground

	//glColor3f(0.9f, 0.9f, 0.9f);
	//glBegin(GL_QUADS);
	//glVertex3f(-100.0f, 0.0f, -100.0f);
	//glVertex3f(-100.0f, 0.0f, 100.0f);
	//glVertex3f(100.0f, 0.0f, 100.0f);
	//glVertex3f(100.0f, 0.0f, -100.0f);
	//glEnd();

	// Draw 36 SnowMen

	//for (int i = -3; i < 3; i++)
	//	for (int j = -3; j < 3; j++) {
	//		glPushMatrix();
	//		glTranslatef(i * 10.0, 0, j * 10.0);
	//		//drawSnowMan();
	//		drawSphere();
	//		glPopMatrix();
	//	}
	_sampleManager.UpdateSample();
	DrawAllGraphicsData();

	glutSwapBuffers();
}

void Renderer::SetUp() {

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Metrics::Width, Metrics::Height);
	glutCreateWindow("Lighthouse3D - GLUT Tutorial");
	//ImGui::SFML::Init(_window);

	_sampleManager.SetUp();
}

void Renderer::TearDown() const noexcept {
	//ImGui::SFML::Shutdown();
}

void Renderer::Run() noexcept {
	bool quit = false;

	bool adjustWindow = true;

	//sf::Event e;

	while (!quit) {
		/*while (_window.pollEvent(e)) {
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
		}*/

		//ImGui::SFML::Update(_window, _deltaClock.restart());

		/*if (adjustWindow) {
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
		}*/

		/*ImGui::Spacing();

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
		_sampleManager.UpdateSample();*/

		DrawAllGraphicsData();

		/*ImGui::SFML::Render(_window);

		_window.display();*/

#ifdef TRACY_ENABLE
		FrameMark;
#endif
	}
}

void Renderer::DrawCircle(const XMVECTOR center, const float radius,
	const int segments, const Color& col) noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	glPushMatrix();
	glColor3f(col.r / 255.f, col.g / 255.f, col.b / 255.f);
	glTranslatef(XMVectorGetX(center), XMVectorGetY(center), XMVectorGetZ(center));
	glutSolidSphere(radius, segments, segments);
	glPopMatrix();

	//sf::CircleShape circle = sf::CircleShape(radius, segments);
	//circle.setFillColor(col);
	//circle.setOrigin(radius, radius);
	//circle.setPosition(XMVectorGetX(center), XMVectorGetY(center));

	//_window.draw(circle);
}

void Renderer::DrawRectangle(const XMVECTOR minBound,
	const XMVECTOR maxBound,
	const Color& col) noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif

	//// Calculate the width and height of the rectangle
	//float width = XMVectorGetX(maxBound) - XMVectorGetX(minBound);
	//float height = XMVectorGetY(maxBound) - XMVectorGetY(minBound);

	//// Create an SFML rectangle shape
	//sf::RectangleShape rectangle(sf::Vector2f(width, height));

	//// Set the position and color of the rectangle
	//rectangle.setPosition(XMVectorGetX(minBound), XMVectorGetY(minBound));
	//rectangle.setFillColor(col);

	//// Draw the rectangle to the window
	//_window.draw(rectangle);
}

void Renderer::DrawRectangleBorder(const XMVECTOR minBound, const XMVECTOR maxBound,
	const Color& col) noexcept {
	// Calculate the width and height of the rectangle
	//float width = XMVectorGetX(maxBound) - XMVectorGetX(minBound);
	//float height = XMVectorGetY(maxBound) - XMVectorGetY(minBound);

	//// Create an SFML rectangle shape
	//sf::RectangleShape rectangle(sf::Vector2f(width, height));

	//// Set the position and color of the rectangle
	//rectangle.setPosition(XMVectorGetX(minBound), XMVectorGetY(minBound));
	//rectangle.setOutlineColor(col);
	//rectangle.setOutlineThickness(1);
	//rectangle.setFillColor(sf::Color::Transparent);

	//// Draw the rectangle to the window
	//_window.draw(rectangle);
}

void Renderer::DrawAllGraphicsData() noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for (auto& bd : _sampleManager.GetSampleData()) {
		if (bd.Shape.index() == (int)ShapeType::Circle) {
			auto& circle = std::get<CircleF>(bd.Shape);
			DrawCircle(circle.Center(), circle.Radius(), 30, { bd.Color });
		}
		else if (bd.Shape.index() == (int)ShapeType::Rectangleee) {
			auto& rect = std::get<RectangleF>(bd.Shape);
			if (!bd.Filled) {
				DrawRectangleBorder(rect.MinBound(), rect.MaxBound(), { bd.Color });
			}
			else {
				DrawRectangle(rect.MinBound(), rect.MaxBound(), { bd.Color });
			}
		}
	}
}
