#include "stdafx.h"
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	m_ParticlePool.resize(1000);
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::OnUpdate(float deltaTime)
{
	for each (Particle particle in m_ParticlePool)
	{
		if (!particle.Active)
			continue;

		if (particle.LifeRemaining <= 0.0f)
		{
			particle.Active = false;
			continue;
		}

		particle.LifeRemaining -= deltaTime;
		particle.Position += particle.Velocity * (float)deltaTime;
		particle.Rotation += 0.01f * deltaTime;
	}
}

void ParticleSystem::OnRender(Camera& camera)
{
}

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
}
