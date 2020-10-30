#pragma once

#include <list>
#include <array>
#include <memory>

template<class T>
class ObjectPool
{
public:
	ObjectPool(T* prefab)
		:mPrefab(prefab)
	{
	}
	~ObjectPool()
	{
		CleanUp();
	}

	T* AcquireObject()
	{
		if (mObjects.empty())
		{
			return new T(*mPrefab);
		}
		else
		{
			T* newObject = mObjects.front();
			mObjects.pop_front();
			return newObject;
		}
	}

	void ReturnObjectToPool(T* object)
	{
		mObjects.push_back(object);
	}

	void CleanUp()
	{
		for (T* object : mObjects)
			delete object;

		mObjects.clear();
	}

	void PreLoadObjects(int numberToLoad)
	{
		for (int i = 0; i < numberToLoad; i++)
		{
			ReturnObjectToPool(new T(*mPrefab));
		}
	}

	T* GetPrefab() { return mPrefab; }

private:
	std::list<T*> mObjects;
	T* mPrefab;
};
