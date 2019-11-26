#include "QuadTree.h"

using namespace std;
using namespace DirectX::SimpleMath;

QuadTree::QuadTree(Vector2 minBound, Vector2 maxBound, UINT treeDepth)
{
	AABB rootBounds = AABB{ Vector3(minBound.x, -10.0, minBound.y), Vector3(maxBound.x, 10.0, maxBound.y) };
	root = make_unique<Node>(move(rootBounds), treeDepth);
}

QuadTree::Node::Node(AABB&& boundingBox, UINT treeDepth)
{
	this->boundingBox = boundingBox;
	this->depth = treeDepth;

	if (treeDepth == 0) return;

	Vector3 middle = Vector3(boundingBox.minPos.x + boundingBox.maxPos.x, 0.0, boundingBox.minPos.z + boundingBox.maxPos.z) * 0.5;


	AABB nwBoundingBox{ 
		Vector3(boundingBox.minPos.x, -10.0, middle.z), 
		Vector3(middle.x, 10.0, boundingBox.maxPos.z)
	};
	AABB neBoundingBox{ 
		Vector3(middle.x, -10.0, middle.z),
		Vector3(boundingBox.maxPos.x, 10.0, boundingBox.maxPos.z)
	};
	AABB swBoundingBox{
		Vector3(boundingBox.minPos.x, -10.0, boundingBox.minPos.z),
		Vector3(middle.x, 10.0, middle.z)
	};
	AABB seBoundingBox{
		Vector3(middle.x, -10.0, boundingBox.minPos.z),
		Vector3(boundingBox.maxPos.x, 10.0, middle.z)
	};

	nwChild = make_unique<Node>(move(nwBoundingBox), treeDepth - 1);
	neChild = make_unique<Node>(move(neBoundingBox), treeDepth - 1);
	swChild = make_unique<Node>(move(swBoundingBox), treeDepth - 1);
	seChild = make_unique<Node>(move(seBoundingBox), treeDepth - 1);
}

void QuadTree::insert(GameObject* o)
{
	root->insert(o);
}

void QuadTree::Node::insert(GameObject* o)
{
	if (depth > 0)
	{
		
		if (nwChild->boundingBox.intersectXZ(o->getAABB()))
		{
			nwChild->insert(o);
		}
		else if (neChild->boundingBox.intersectXZ(o->getAABB()))
		{
			neChild->insert(o);
		}
		else if (swChild->boundingBox.intersectXZ(o->getAABB()))
		{
			swChild->insert(o);
		}
		else if (seChild->boundingBox.intersectXZ(o->getAABB()))
		{
			seChild->insert(o);
		}
	}
	else
	{
		objects.push_back(o);
		auto comp = [](GameObject* a, GameObject* b) {return a->getAABB().maxPos.y < b->getAABB().maxPos.y; };
		std::sort(objects.begin(), objects.end(), comp);
	}
}

void QuadTree::getGameObjects(std::vector<GameObject*>& objects, Frustum viewFrustum, float frustumBias)
{
	root->getGameObjects(objects, viewFrustum, frustumBias);
}

void QuadTree::Node::getGameObjects(std::vector<GameObject*>& objects, Frustum& viewFrustum, float& frustumBias)
{
	if (viewFrustum.intersect(boundingBox, frustumBias, true))
	{
		if (depth > 0)
		{
			nwChild->getGameObjects(objects, viewFrustum, frustumBias);
			neChild->getGameObjects(objects, viewFrustum, frustumBias);
			swChild->getGameObjects(objects, viewFrustum, frustumBias);
			seChild->getGameObjects(objects, viewFrustum, frustumBias);
		}
		else
		{
			for (GameObject* o : this->objects)
			{
				if (viewFrustum.intersect(o->getAABB(), 10.0, true))
				{
					objects.push_back(o);
				}
			}
		}
	}
}

void QuadTree::clearGameObjects()
{
	root->clearGameObjects();
}

void QuadTree::Node::clearGameObjects()
{
	if (depth > 0)
	{
		nwChild->clearGameObjects();
		neChild->clearGameObjects();
		swChild->clearGameObjects();
		seChild->clearGameObjects();
	}
	else
	{
		objects.clear();
	}
}
