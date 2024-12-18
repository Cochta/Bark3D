#include "SFMLApp.h"
#include "Renderer.h"
#include <iostream>

Renderer renderer;

static void resize(int x, int y) { renderer.Resize(x, y); }
static void renderScene(void) { renderer.renderScene(); }

void pressKey(int key, int xx, int yy) {

	switch (key) {
	case GLUT_KEY_LEFT: renderer.camera.deltaAngle = -0.05f; break;
	case GLUT_KEY_RIGHT: renderer.camera.deltaAngle = 0.05f; break;
	case GLUT_KEY_UP: renderer.camera.deltaMove = 2.5f; break;
	case GLUT_KEY_DOWN: renderer.camera.deltaMove = -2.5f; break;
	}
}

void releaseKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT: renderer.camera.deltaAngle = 0.0f; break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: renderer.camera.deltaMove = 0; break;
	}
}

// Mouse button callback
void mouseButtonCallback(int button, int state, int x, int y) {
	// Send mouse inputs to GUI
	ImGui_ImplGLUT_MouseFunc(button, state, x, y);

	// Update ImGui mouse state
	ImGuiIO& io = ImGui::GetIO();

	if (button == GLUT_LEFT_BUTTON) {
		io.MouseDown[0] = (state == GLUT_DOWN); // Left mouse button
		renderer._sampleManager.GiveLeftMouseClickToSample();
		
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		io.MouseDown[1] = (state == GLUT_DOWN); // Right mouse button
		renderer._sampleManager.GiveRightMouseClickToSample();
	}

	if (button == GLUT_MIDDLE_BUTTON) {
		io.MouseDown[2] = (state == GLUT_DOWN); // Middle mouse button
		
	}
}

// Mouse motion callback
void mouseMotionCallback(int x, int y) {
	ImGui_ImplGLUT_MotionFunc(x, y);

	// Update ImGui mouse position
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((double)x, (double)y);
	renderer.MousePos = { (float)x,(float)y };

}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
	//SFMLApp app("Samples SFML", Metrics::Width, Metrics::Height);

	//app.SetUp();

	//app.Run();

	//app.TearDown();

	// init GLUT and create window
	glutInit(&argc, argv);
	renderer.SetUp();

	// register callbacks
	glutDisplayFunc(renderScene);

	ImGui_ImplGLUT_Init();
	ImGui_ImplOpenGL2_Init();
	ImGui_ImplGLUT_InstallFuncs();

	glutReshapeFunc(resize);
	glutIdleFunc(renderScene);

	glutSpecialFunc(pressKey);

	// here are the new entries
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);

	// Register mouse button callback
	glutMouseFunc(mouseButtonCallback);

	// Register mouse motion callback
	glutMotionFunc(mouseMotionCallback);
	glutPassiveMotionFunc(mouseMotionCallback);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();

	renderer.TearDown();

	return EXIT_SUCCESS;
}