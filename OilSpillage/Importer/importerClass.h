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
	animatedJoint* animatedSkeleton;
};

struct Animation
{
	AnimationHeader animationInfo;
	animationKeyFrame* keyFrames;
};

struct BlendShape
{
	BlendShapeHeader blendShapeInfo;
	Vertex* vertices;
};

struct LoadedMesh
{
	Mesh mHeader;
	Material material;
	Vertex* vertices;
	Joint* skeleton;
	Animation animation;
	BlendShape* blendShapes;
};

struct Group
{
	GroupHeader header;
	GroupChildren* children;
};

class Importer {
private:

	MyFormat header;
	LoadedMesh* loadedMeshes;
	Camera* loadedCameras;
	Light* loadedLights;
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

	Vertex* getVertices() const;
	Vertex* getVertices(const char* meshName) const;
	Vertex* getVertices(int meshID) const;

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
	Joint* getJoints() const;
	animatedJoint* getAnimatedJointsAtKey(int keyFrame);
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