#include "SFMLApp.h"
#include "Renderer.h"

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
	glutReshapeFunc(resize);
	glutIdleFunc(renderScene);

	glutSpecialFunc(pressKey);

	// here are the new entries
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return EXIT_SUCCESS;
}