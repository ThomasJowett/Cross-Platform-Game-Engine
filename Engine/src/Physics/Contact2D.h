#pragma once

#include "box2d/box2d.h"
#include "math/Vector2f.h"

#include <vector>

struct Contact2D
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;

	bool triggeredA = false;
	bool triggeredB = false;
	bool old = false;

	Vector2f localNormal;
	Vector2f localPoint;

	Contact2D(b2Fixture* A, b2Fixture* B)
		:fixtureA(A), fixtureB(B)
	{}

	bool operator==(const Contact2D& other) const
	{
		return (fixtureA == other.fixtureA) && (fixtureB == other.fixtureB);
	}
};

class ContactListener2D : public b2ContactListener
{
	
public:
	ContactListener2D() = default;
	~ContactListener2D() = default;

	std::vector<Contact2D> m_Contacts;

	virtual void BeginContact(b2Contact* contact) override
	{
		m_Contacts.push_back(Contact2D(contact->GetFixtureA(), contact->GetFixtureB()));
	}
	virtual void EndContact(b2Contact* contact) override
	{
		Contact2D oldContact(contact->GetFixtureA(), contact->GetFixtureB());

		auto currentContact = std::find(m_Contacts.begin(), m_Contacts.end(), oldContact);
		if (currentContact != m_Contacts.end())
			currentContact->old = true;
	}
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
	{
		auto currentContact = std::find(m_Contacts.begin(), m_Contacts.end(), Contact2D(contact->GetFixtureA(), contact->GetFixtureB()));
		if (currentContact != m_Contacts.end())
		{
			currentContact->localNormal = Vector2f(oldManifold->localNormal.x, oldManifold->localNormal.y);
			currentContact->localPoint = Vector2f(oldManifold->localPoint.x, oldManifold->localPoint.y);
		}
	}

	void RemoveOld()
	{
		if (m_Contacts.size() > 0)
			m_Contacts.erase(std::remove_if(m_Contacts.begin(), m_Contacts.end(), [](Contact2D c) {return c.old; }), m_Contacts.end());
	}
};