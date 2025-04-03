#pragma once

#include "math/Vector2f.h"
#include "Core/Colour.h"
#include "Renderer/Camera.h"
#include "Core/ObjectPool.h"
#include "Core/core.h"
#include "Asset/Shader.h"

struct ParticleProps
{
	Vector2f position;
	Vector2f velocity, velocityVariation;
	Colour beginColour, endColour;
	float sizeBegin, sizeEnd, sizeVariation;
	float lifeTime;
};

class ParticleSystem
{
public:
	ParticleSystem();

	~ParticleSystem();

	void OnUpdate(float deltaTime);
	void OnRender(Camera& camera);

	void Emit(const ParticleProps& particleProps);
private:

	struct Particle
	{
		Vector2f Position;
		Vector2f Velocity;
		Colour ColourBegin, ColourEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd = 1.0f;

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;
	};

	std::vector<Particle> m_ParticlePool;

	uint32_t m_PoolIndex = 999;

	uint32_t m_QuadVA = 0;
	Scope<Shader> m_ParticleShader;
};
