#ifndef IMPORTER_CLASS
#define IMPORTER_CLASS

#include "meshFormat.h"
#include <string.h>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <iostream>

struct animationKeyFrame
{
	Meshformat::animatedJoint* animatedSkeleton;
};

struct Animation
{
	Meshformat::AnimationHeader animationInfo;
	animationKeyFrame* keyFrames;
};

struct BlendShape
{
	Meshformat::BlendShapeHeader blendShapeInfo;
	Meshformat::Vertex* vertices;
};

struct LoadedMesh
{
	Meshformat::FormatMesh mHeader;
	Meshformat::Material material;
	Meshformat::Vertex* vertices;
	Meshformat::Joint* skeleton;
	Animation animation;
	BlendShape* blendShapes;
};

struct Group
{
	Meshformat::GroupHeader header;
	Meshformat::GroupChildren* children;
};

class Importer {
private:

	Meshformat::MyFormat header;
	LoadedMesh* loadedMeshes;
	Meshformat::FormatCamera* loadedCameras;
	Meshformat::DummyLight* loadedLights;
	Group* loadedGroups;

public:

	Importer();
	~Importer();

	void clone(const Importer& other);

	//load functions
	bool loadMesh(const char* filename);
	bool loadLights(const char* filename);
	bool loadCameras(const char* filename);
	bool loadGroups(const char* filename);
	bool loadScene(const char* filename);

	//Meshes
	int getMeshCount() const;
	LoadedMesh getMesh() const;

	int getVertexCount() const;
	int getVertexCount(const char* meshName) const;
	int getVertexCount(int meshID) const;

	Meshformat::Vertex* getVertices() const;
	Meshformat::Vertex* getVertices(const char* meshName) const;
	Meshformat::Vertex* getVertices(int meshID) const;

	void getMinBBox(float &minX, float &minY, float &minZ);
	void getMaxBBox(float &maxX, float &maxY, float &maxZ);

	void getMinBBox(float &minX, float &minY, float &minZ, int meshID);
	void getMaxBBox(float &maxX, float &maxY, float &maxZ, int meshID);


	//Material
	char* getMaterialName();
	char* getMaterialName(int meshID);

	char* getTextureName();
	char* getTextureName(int meshID);

	char* getNormalMapName();
	char* getNormalMapName(int meshID);

	char* getSpecularMapName();
	char* getSpecularMapName(int meshID);

	void getDiffuseColor(float &r, float &g, float &b);
	void getDiffuseColor(float &r, float &g, float &b, int meshID);

	void getAmbientColor(float &r, float &g, float &b);
	void getAmbientColor(float &r, float &g, float &b, int meshID);

	void getEmissiveColor(float &r, float &g, float &b);
	void getEmissiveColor(float &r, float &g, float &b, int meshID);

	void getTransparencyColor(float &r, float &g, float &b);
	void getTransparencyColor(float &r, float &g, float &b, int meshID);

	//Phong Values
	void getSpecularColor(float &r, float &g, float &b);
	void getSpecularColor(float &r, float &g, float &b, int meshID);

	void getReflectionColor(float &r, float &g, float &b);
	void getReflectionColor(float &r, float &g, float &b, int meshID);



	//Skeletal Animations
	Meshformat::Joint* getJoints() const;
	Meshformat::animatedJoint* getAnimatedJointsAtKey(int keyFrame);
	int getJointCount() const;
	int getAnimationLength();
	int getNrOfAnimations();

	//BlendShapes
	int getBlendShapeCount();

	//Cameras
	int getCameraCount();

	void getCameraPosition(float &x, float &y, float &z);
	void getCameraPosition(float &x, float &y, float &z, int cameraID);

	void getCameraLookAt(float &x, float &y, float &z);
	void getCameraLookAt(float &x, float &y, float &z, int cameraID);

	void getCameraUP(float &x, float &y, float &z);
	void getCameraUP(float &x, float &y, float &z, int cameraID);

	//Lights
	int getLightCount();

	char* getLightType();

	void getLightPosition(float &x, float &y, float &z);
	void getLightPosition(float &x, float &y, float &z, int lightID);

	void getLightColor(float &r, float &g, float &b);
	void getLightColor(float &r, float &g, float &b, int lightID);

	float getLightIntensity();
	float getLightIntensity(int lightID);
	//Groups

	void destroyMesh();
};

#endif // !IMPORTER_CLASS