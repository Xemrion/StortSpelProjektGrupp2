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
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		std::vector<StaticGameObject*> objects;
		AABB boundingBox;
		UINT depth;
	public:
		Node(AABB&& boundingBox, UINT treeDepth);
		void insert(StaticGameObject* o);
		void getObjects(std::vector<StaticGameObject*>& objects, Frustum& viewFrustum, float& frustumBias);
		void createVertexBuffers(ID3D11Device* device);
		void getVertexBuffers(std::vector<ID3D11Buffer**>& vertexBuffers, Frustum& viewFrustum, float& frustumBias);
		void clearContents();
	};
	std::unique_ptr<Node> root;
public:
	QuadTree(DirectX::SimpleMath::Vector2 minBound, DirectX::SimpleMath::Vector2 maxBound, UINT treeDepth);
	void insert(StaticGameObject* o);
	void getObjects(std::vector<StaticGameObject*>& objects, Frustum viewFrustum, float frustumBias);
	void createVertexBuffers(ID3D11Device* device);
	void getVertexBuffers(std::vector<ID3D11Buffer**>& vertexBuffers, Frustum viewFrustum, float frustumBias);
	void clearContents();
};
