#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include "structs.h"
#include "GameObject.h"
#include "Frustum.h"

class QuadTree
{
	class Node
	{
		std::unique_ptr<Node> nwChild;
		std::unique_ptr<Node> neChild;
		std::unique_ptr<Node> swChild;
		std::unique_ptr<Node> seChild;
		std::vector<GameObject*> objects;
		AABB boundingBox;
		UINT depth;
	public:
		Node(AABB&& boundingBox, UINT treeDepth);
		void insert(GameObject* o);
		void getGameObjects(std::vector<GameObject*>& objects, Frustum& viewFrustum, float& frustumBias);
		void clearGameObjects();
	};
	std::unique_ptr<Node> root;
public:
	explicit  QuadTree(DirectX::SimpleMath::Vector2 minBound, DirectX::SimpleMath::Vector2 maxBound, UINT treeDepth);
	QuadTree()                            = delete;
	QuadTree( QuadTree const &)           = delete;
	QuadTree( QuadTree &&)                = delete;
	QuadTree &operator=(QuadTree const &) = delete;
	QuadTree &operator=(QuadTree &&)      = delete;
	void insert(GameObject* o);
	void getGameObjects(std::vector<GameObject*>& objects, Frustum viewFrustum, float frustumBias);
	void clearGameObjects();
};
