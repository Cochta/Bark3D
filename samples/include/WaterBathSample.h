#pragma once

#include <iostream>
#include "Sample.h"
#include "Random.h"


static constexpr float GRAV = -500;
static constexpr int NBPARTICLES = 2000;
static constexpr float WALLSIZE = Metrics::MetersToPixels(0.1f);
static constexpr float WALLDIST = Metrics::MetersToPixels(1.0f);
static constexpr float PARTICLESIZE = Metrics::MetersToPixels(0.05f);

class WaterBathSample : public Sample, public ContactListener
{
private:
	std::vector<GraphicsData> _quadTreeGraphicsData;
public:
	std::string GetName() noexcept override;
	std::string GetDescription() noexcept override;

	void OnTriggerEnter(ColliderRef col1, ColliderRef col2) noexcept override {};

	void OnTriggerExit(ColliderRef col1, ColliderRef col2) noexcept override {};

	void OnCollisionEnter(ColliderRef col1, ColliderRef col2) noexcept override;

	void OnCollisionExit(ColliderRef col1, ColliderRef col2) noexcept override;

protected:
	void SampleSetUp() noexcept override;

	void SampleUpdate() noexcept override;

	void SampleTearDown() noexcept override;

private:
	void CreateBall(XMVECTOR position) noexcept;

	void DrawQuadtree(const BVHNode& node) noexcept;

};