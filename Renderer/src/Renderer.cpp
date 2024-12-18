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

void Renderer::renderScene(void)
{

	if (camera.deltaMove)
		camera.computePos(camera.deltaMove);
	if (camera.deltaAngle)
		camera.computeDir(camera.deltaAngle);

	glViewport(0, 0, Metrics::Width, Metrics::Height);
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

	/*_sampleManager.UpdateSample();
	DrawAllGraphicsData();*/


	Run();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	glutSwapBuffers();
	glutPostRedisplay();
}

void Renderer::SetUp() {
#ifdef __FREEGLUT_EXT_H__
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Metrics::Width, Metrics::Height);
	glutCreateWindow("ouais");

	// enter GLUT event processing cycle
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = { Metrics::Width,Metrics::Height };
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableSetMousePos;     // Enable Keyboard Controls
	ImGui::StyleColorsDark();

	_sampleManager.SetUp();
}

void Renderer::TearDown() const noexcept {
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();
}

void Renderer::Run() noexcept {

	_sampleManager.GiveMousePositionToSample(MousePos);

	_sampleManager.UpdateSample();

	DrawAllGraphicsData();


	//calculate delta time not used yet
	//currentTime = glutGet(GLUT_ELAPSED_TIME);
	//deltaTime = (currentTime - prevTime) / 1000.f;
	//prevTime = currentTime;

	bool quit = false;

	bool adjustWindow = true;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();

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


#ifdef TRACY_ENABLE
	FrameMark;
#endif

}

void Renderer::DrawSphere(const XMVECTOR center, const float radius,
	const int segments, const Color& col) noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	glPushMatrix();
	glColor3f(col.r / 255.f, col.g / 255.f, col.b / 255.f);
	glTranslatef(XMVectorGetX(center), XMVectorGetY(center), XMVectorGetZ(center));
	glutSolidSphere(radius, segments, segments);
	glPopMatrix();
}

void Renderer::Drawcuboid(const XMVECTOR minBound,
	const XMVECTOR maxBound,
	const Color& col) noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif

	// Extract min and max coordinates from XMVECTOR
	float minX = DirectX::XMVectorGetX(minBound);
	float minY = DirectX::XMVectorGetY(minBound);
	float minZ = DirectX::XMVectorGetZ(minBound);

	float maxX = DirectX::XMVectorGetX(maxBound);
	float maxY = DirectX::XMVectorGetY(maxBound);
	float maxZ = DirectX::XMVectorGetZ(maxBound);

	// Calculate the dimensions of the cuboid
	float width = maxX - minX;
	float height = maxY - minY;
	float depth = maxZ - minZ;

	glPushMatrix();

	// Set the color for the cuboid (normalized to [0, 1])
	glColor3f(col.r / 255.f, col.g / 255.f, col.b / 255.f);

	// Draw the cuboid (using quads to form each face)
	glBegin(GL_QUADS);

	// Front face
	glVertex3f(minX, minY, maxZ);  // Top-left
	glVertex3f(maxX, minY, maxZ);  // Top-right
	glVertex3f(maxX, maxY, maxZ);  // Bottom-right
	glVertex3f(minX, maxY, maxZ);  // Bottom-left

	// Back face
	glVertex3f(minX, minY, minZ);  // Top-left
	glVertex3f(minX, maxY, minZ);  // Bottom-left
	glVertex3f(maxX, maxY, minZ);  // Bottom-right
	glVertex3f(maxX, minY, minZ);  // Top-right

	// Left face
	glVertex3f(minX, minY, minZ);  // Top-front
	glVertex3f(minX, minY, maxZ);  // Bottom-front
	glVertex3f(minX, maxY, maxZ);  // Bottom-back
	glVertex3f(minX, maxY, minZ);  // Top-back

	// Right face
	glVertex3f(maxX, minY, minZ);  // Top-front
	glVertex3f(maxX, maxY, minZ);  // Top-back
	glVertex3f(maxX, maxY, maxZ);  // Bottom-back
	glVertex3f(maxX, minY, maxZ);  // Bottom-front

	// Top face
	glVertex3f(minX, maxY, minZ);  // Back-left
	glVertex3f(minX, maxY, maxZ);  // Front-left
	glVertex3f(maxX, maxY, maxZ);  // Front-right
	glVertex3f(maxX, maxY, minZ);  // Back-right

	// Bottom face
	glVertex3f(minX, minY, minZ);  // Back-left
	glVertex3f(maxX, minY, minZ);  // Back-right
	glVertex3f(maxX, minY, maxZ);  // Front-right
	glVertex3f(minX, minY, maxZ);  // Front-left

	glEnd();  // End drawing the cuboid faces

	glPopMatrix();
}

void Renderer::DrawcuboidBorder(const XMVECTOR minBound, const XMVECTOR maxBound,
	const Color& col) noexcept {
	// Calculate the width and height of the cuboid
	//float width = XMVectorGetX(maxBound) - XMVectorGetX(minBound);
	//float height = XMVectorGetY(maxBound) - XMVectorGetY(minBound);

	//// Create an SFML cuboid shape
	//sf::cuboidShape cuboid(sf::Vector2f(width, height));

	//// Set the position and color of the cuboid
	//cuboid.setPosition(XMVectorGetX(minBound), XMVectorGetY(minBound));
	//cuboid.setOutlineColor(col);
	//cuboid.setOutlineThickness(1);
	//cuboid.setFillColor(sf::Color::Transparent);

	//// Draw the cuboid to the window
	//_window.draw(cuboid);
}

void Renderer::DrawAllGraphicsData() noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for (auto& bd : _sampleManager.GetSampleData()) {
		if (bd.Shape.index() == (int)ShapeType::Sphere) {
			auto& sphere = std::get<SphereF>(bd.Shape);
			DrawSphere(sphere.Center(), sphere.Radius(), 30, { bd.Color });
		}
		else if (bd.Shape.index() == (int)ShapeType::Cuboid) {
			auto& rect = std::get<CuboidF>(bd.Shape);
			if (!bd.Filled) {
				DrawcuboidBorder(rect.MinBound(), rect.MaxBound(), { bd.Color });
			}
			else {
				Drawcuboid(rect.MinBound(), rect.MaxBound(), { bd.Color });
			}
		}
	}
}
