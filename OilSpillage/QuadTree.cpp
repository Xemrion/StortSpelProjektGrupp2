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

void QuadTree::insert(StaticGameObject* o)
{
	root->insert(o);
}

void QuadTree::Node::insert(StaticGameObject* o)
{
	if (depth > 0)
	{
		if (o->getAABB().intersect(nwChild->boundingBox))
		{
			nwChild->insert(o);
		}
		else if (o->getAABB().intersect(neChild->boundingBox))
		{
			neChild->insert(o);
		}
		else if (o->getAABB().intersect(swChild->boundingBox))
		{
			swChild->insert(o);
		}
		else if (o->getAABB().intersect(seChild->boundingBox))
		{
			seChild->insert(o);
		}
	}
	else
	{
		objects.push_back(o);
	}
}

void QuadTree::getObjects(std::vector<StaticGameObject*>& objects, Frustum viewFrustum, float frustumBias)
{
	root->getObjects(objects, viewFrustum, frustumBias);
}

void QuadTree::Node::getObjects(std::vector<StaticGameObject*>& objects, Frustum& viewFrustum, float& frustumBias)
{
	if (viewFrustum.intersect(boundingBox, frustumBias))
	{
		if (depth > 0)
		{
			nwChild->getObjects(objects, viewFrustum, frustumBias);
			neChild->getObjects(objects, viewFrustum, frustumBias);
			swChild->getObjects(objects, viewFrustum, frustumBias);
			seChild->getObjects(objects, viewFrustum, frustumBias);
		}
		else
		{
			for (StaticGameObject* o : this->objects)
			{
				if (viewFrustum.intersect(o->getAABB(), frustumBias))
				{
					objects.push_back(o);
				}
			}
		}
	}
}


void QuadTree::createVertexBuffers(ID3D11Device* device)
{
	root->createVertexBuffers(device);
}

void QuadTree::Node::createVertexBuffers(ID3D11Device* device)
{
	if (depth > 0)
	{
		nwChild->createVertexBuffers(device);
		neChild->createVertexBuffers(device);
		swChild->createVertexBuffers(device);
		seChild->createVertexBuffers(device);
	}
	else
	{
		std::vector<Vertex3D> vertices;
		D3D11_BUFFER_DESC vBufferDesc;
		ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));
		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.CPUAccessFlags = 0;
		vBufferDesc.MiscFlags = 0;

		for (StaticGameObject* o : objects)
		{
			for (Vertex3D v : o->vertices)
			{
				vBufferDesc.ByteWidth += sizeof(Vertex3D);
				vertices.push_back(std::move(v));
			}
		}

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		subData.pSysMem = vertices.data();

		device->CreateBuffer(&vBufferDesc, &subData, vertexBuffer.ReleaseAndGetAddressOf());
	}
}

void QuadTree::getVertexBuffers(std::vector<ID3D11Buffer**>& vertexBuffers, Frustum viewFrustum, float frustumBias)
{
	root->getVertexBuffers(vertexBuffers, viewFrustum, frustumBias);
}

void QuadTree::Node::getVertexBuffers(std::vector<ID3D11Buffer**>& vertexBuffers, Frustum& viewFrustum, float& frustumBias)
{
	if (viewFrustum.intersect(boundingBox, frustumBias))
	{
		if (depth > 0)
		{
			nwChild->getVertexBuffers(vertexBuffers, viewFrustum, frustumBias);
			neChild->getVertexBuffers(vertexBuffers, viewFrustum, frustumBias);
			swChild->getVertexBuffers(vertexBuffers, viewFrustum, frustumBias);
			seChild->getVertexBuffers(vertexBuffers, viewFrustum, frustumBias);
		}
		else
		{
			vertexBuffers.push_back(vertexBuffer.GetAddressOf());
		}
	}
}

void QuadTree::clearContents()
{
	root->clearContents();
}

void QuadTree::Node::clearContents()
{
	if (depth > 0)
	{
		nwChild->clearContents();
		neChild->clearContents();
		swChild->clearContents();
		seChild->clearContents();
	}
	else
	{
		objects.clear();
		vertexBuffer.ReleaseAndGetAddressOf();
	}
}
