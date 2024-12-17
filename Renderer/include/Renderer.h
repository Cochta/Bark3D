#pragma once
#include "Camera.h"
#include "SampleManager.h"

//#include <imgui-SFML.h>
//#include <imgui.h>

class Renderer
{
public:
	Camera camera;
	SampleManager _sampleManager;

	void Resize(int w, int h);

	void SetUp();

	void TearDown() const noexcept;

	void Run() noexcept;

	void drawSnowMan();
	void drawSphere();

	void renderScene(void);

private:

	void DrawCircle(XMVECTOR center, float radius, int segments, const Color& col) noexcept;

	void DrawRectangle(XMVECTOR minBound, XMVECTOR maxBound, const Color& col) noexcept;

	void DrawRectangleBorder(XMVECTOR minBound, XMVECTOR maxBound, const Color& col) noexcept;

	void DrawAllGraphicsData() noexcept;

};
