#include "ImporterClass.h"

using namespace Meshformat;

Importer::Importer()
{
	this->header.meshCount = 0;
}

Importer::~Importer()
{
	if (this->header.meshCount > 0)
	{
		for (int i = 0; i < this->header.meshCount; i++)
		{
			delete[] this->loadedMeshes[i].vertices;
			if (this->loadedMeshes[i].mHeader.jointCount > 0)
			{
				delete[] this->loadedMeshes[i].skeleton;

				if (this->loadedMeshes[i].animation.animationInfo.animationLength > 0)
				{
					for (int j = 0; j < this->loadedMeshes[i].animation.animationInfo.animationLength; j++)
					{
						delete[] this->loadedMeshes[i].animation.keyFrames[j].animatedSkeleton;
					}

					delete[] this->loadedMeshes[i].animation.keyFrames;
				}
			}

			if (this->loadedMeshes[i].mHeader.blendShapeCount > 0)
			{
				delete[] this->loadedMeshes[i].blendShapes;
			}
		}


		delete[] this->loadedMeshes;

		this->header.meshCount = 0;
	}

	if (this->header.cameraCount > 0)
	{
		delete[] this->loadedCameras;
		this->header.cameraCount = 0;
	}

	if (this->header.lightCount > 0)
	{
		delete[] this->loadedLights;
		this->header.lightCount = 0;
	}
}

void Importer::clone(const Importer & other)
{
	//header = other.header;
	header.meshCount = other.header.meshCount;
	if (other.header.meshCount > 0)
	{
		loadedMeshes = new LoadedMesh[other.header.meshCount];
		for (int i = 0; i < other.header.meshCount; i++)
		{
			//loadedMeshes[i].meshHeader = other.loadedMeshes[i].meshHeader;
			loadedMeshes[i].mHeader.vertexCount = other.loadedMeshes[i].mHeader.vertexCount;
			loadedMeshes[i].mHeader.minX = other.loadedMeshes[i].mHeader.minX;
			loadedMeshes[i].mHeader.minY = other.loadedMeshes[i].mHeader.minY;
			loadedMeshes[i].mHeader.minZ = other.loadedMeshes[i].mHeader.minZ;
			loadedMeshes[i].mHeader.maxX = other.loadedMeshes[i].mHeader.maxX;
			loadedMeshes[i].mHeader.maxY = other.loadedMeshes[i].mHeader.maxY;
			loadedMeshes[i].mHeader.maxZ = other.loadedMeshes[i].mHeader.maxZ;
			loadedMeshes[i].mHeader.jointCount = other.loadedMeshes[i].mHeader.jointCount;
			loadedMeshes[i].mHeader.animationCount = other.loadedMeshes[i].mHeader.animationCount;
			loadedMeshes[i].mHeader.blendShapeCount = other.loadedMeshes[i].mHeader.blendShapeCount;
			loadedMeshes[i].vertices = new Vertex[other.loadedMeshes[i].mHeader.vertexCount];

			loadedMeshes[i].material.diffR = other.loadedMeshes[i].material.diffR;
			loadedMeshes[i].material.diffG = other.loadedMeshes[i].material.diffG;
			loadedMeshes[i].material.diffB = other.loadedMeshes[i].material.diffB;

			loadedMeshes[i].material.ambiR = other.loadedMeshes[i].material.ambiR;
			loadedMeshes[i].material.ambiG = other.loadedMeshes[i].material.ambiG;
			loadedMeshes[i].material.ambiB = other.loadedMeshes[i].material.ambiB;

			loadedMeshes[i].material.emisR = other.loadedMeshes[i].material.emisR;
			loadedMeshes[i].material.emisG = other.loadedMeshes[i].material.emisG;
			loadedMeshes[i].material.emisB = other.loadedMeshes[i].material.emisB;

			memcpy(loadedMeshes[i].vertices, other.loadedMeshes[i].vertices, sizeof(other.loadedMeshes[i].vertices));

			if (loadedMeshes[i].mHeader.jointCount > 0)
			{
				loadedMeshes[i].skeleton = new Joint[other.loadedMeshes[i].mHeader.jointCount];
				memcpy(loadedMeshes[i].skeleton, other.loadedMeshes[i].skeleton, sizeof(other.loadedMeshes[i].skeleton));
			}
			if (loadedMeshes[i].mHeader.animationCount > 0)
			{
				loadedMeshes[i].animation.animationInfo.animationLength = other.loadedMeshes[i].animation.animationInfo.animationLength;
				loadedMeshes[i].animation.keyFrames = new animationKeyFrame[other.loadedMeshes[i].animation.animationInfo.animationLength];
				for (int j = 0; j < other.loadedMeshes[i].animation.animationInfo.animationLength; j++)
				{
					loadedMeshes[i].animation.keyFrames[j].animatedSkeleton = new animatedJoint[loadedMeshes[i].mHeader.jointCount];
					for (int k = 0; k < loadedMeshes[i].mHeader.jointCount; k++)
					{
						loadedMeshes[i].animation.keyFrames[j].animatedSkeleton[k].index = other.loadedMeshes[i].animation.keyFrames[j].animatedSkeleton[k].index;
						loadedMeshes[i].animation.keyFrames[j].animatedSkeleton[k].keyFrame = other.loadedMeshes[i].animation.keyFrames[j].animatedSkeleton[k].keyFrame;
						for (int x = 0; x < 4; x++)
						{
							for (int y = 0; y < 4; y++)
							{
								loadedMeshes[i].animation.keyFrames[j].animatedSkeleton[k].keyFrameTransform[x][y] = other.loadedMeshes[i].animation.keyFrames[j].animatedSkeleton[k].keyFrameTransform[x][y];
							}
						}
						
					}
				}
			}
		}
	}
}

bool Importer::loadMesh(const char * filename)
{
	if (this->header.meshCount > 0)
	{
		for (int i = 0; i < this->header.meshCount; i++)
			delete[] this->loadedMeshes[i].vertices;

		delete[] this->loadedMeshes;

		this->header.meshCount = 0;
	}



	std::ifstream infile(filename, std::ifstream::binary);

	if (infile)
	{
		MyFormat h;

		infile.read((char*)&h, sizeof(MyFormat));

		this->header.meshCount = h.meshCount;
		this->loadedMeshes = new LoadedMesh[h.meshCount];

		for (int i = 0; i < h.meshCount; i++)
		{
			infile.read((char*)&loadedMeshes[i].mHeader, sizeof(FormatMesh));
			infile.read((char*)&loadedMeshes[i].material, sizeof(Material));
			this->loadedMeshes[i].vertices = new Vertex[loadedMeshes[i].mHeader.vertexCount];
			infile.read((char*)loadedMeshes[i].vertices, sizeof(Vertex) * loadedMeshes[i].mHeader.vertexCount);
			if (loadedMeshes[i].mHeader.jointCount > 0)
			{
				this->loadedMeshes[i].skeleton = new Joint[loadedMeshes[i].mHeader.jointCount];
				infile.read((char*)loadedMeshes[i].skeleton, sizeof(Joint) * loadedMeshes[i].mHeader.jointCount);
			}
			infile.read((char*)&loadedMeshes[i].animation.animationInfo, sizeof(AnimationHeader));
			if (loadedMeshes[i].mHeader.animationCount > 0)
			{
				this->loadedMeshes[i].animation.keyFrames = new animationKeyFrame[loadedMeshes[i].animation.animationInfo.animationLength];
				for (int j = 0; j < loadedMeshes[i].animation.animationInfo.animationLength; j++)
				{
					this->loadedMeshes[i].animation.keyFrames[j].animatedSkeleton = new animatedJoint[loadedMeshes[i].mHeader.jointCount];
					infile.read((char*)loadedMeshes[i].animation.keyFrames[j].animatedSkeleton, sizeof(animatedJoint) * loadedMeshes[i].mHeader.jointCount);
				}
			}

			if (loadedMeshes[i].mHeader.blendShapeCount > 0)
			{
				this->loadedMeshes[i].blendShapes = new BlendShape[loadedMeshes[i].mHeader.blendShapeCount];
				for (int j = 0; j < loadedMeshes[i].mHeader.blendShapeCount; j++)
				{
					infile.read((char*)&loadedMeshes[i].blendShapes[j].blendShapeInfo, sizeof(BlendShapeHeader));
					infile.read((char*)loadedMeshes[i].blendShapes[j].vertices, sizeof(Vertex) * loadedMeshes[i].mHeader.vertexCount);
				}
			}

		}
		infile.close();
		return true;
	}
	return false;
}

bool Importer::loadLights(const char * filename)
{
	if (this->header.lightCount > 0)
	{
		delete[] this->loadedLights;

		this->header.lightCount = 0;
	}

	std::ifstream infile(filename, std::ifstream::binary);
	if (infile)
	{
		MyFormat h;

		infile.read((char*)&h, sizeof(MyFormat));

		this->header.lightCount = h.lightCount;
		this->loadedLights = new DummyLight[h.lightCount];

		for (int i = 0; i < h.meshCount; i++)
		{
			LoadedMesh tMesh;
			infile.read((char*)&tMesh.mHeader, sizeof(FormatMesh));
			infile.ignore(sizeof(Material));
			infile.ignore(sizeof(Vertex) * tMesh.mHeader.vertexCount);
			if (tMesh.mHeader.jointCount > 0)
			{
				infile.ignore(sizeof(Joint) * tMesh.mHeader.jointCount);
			}
			AnimationHeader tAnimHeader;
			infile.read((char*)&tAnimHeader, sizeof(AnimationHeader));
			if (tMesh.mHeader.animationCount > 0)
			{
				for (int j = 0; j < tAnimHeader.animationLength; j++)
				{
					infile.ignore(sizeof(animatedJoint) * tMesh.mHeader.jointCount);
				}
			}

			if (tMesh.mHeader.blendShapeCount > 0)
			{
				for (int j = 0; j <tMesh.mHeader.blendShapeCount; j++)
				{
					infile.ignore(sizeof(BlendShapeHeader));
					infile.ignore(sizeof(Vertex) * tMesh.mHeader.vertexCount);
				}
			}

		}

		infile.ignore(sizeof(FormatCamera) * h.cameraCount);
		infile.read((char*)this->loadedLights, sizeof(DummyLight) * h.lightCount);

		infile.close();
		return true;
	}


	return false;
}

bool Importer::loadCameras(const char * filename)
{

	if (this->header.cameraCount > 0)
	{
		delete[] this->loadedCameras;

		this->header.cameraCount = 0;
	}

	std::ifstream infile(filename, std::ifstream::binary);
	if (infile)
	{
		MyFormat h;

		infile.read((char*)&h, sizeof(MyFormat));

		this->header.cameraCount = h.cameraCount;
		this->loadedCameras = new FormatCamera[h.cameraCount];

		for (int i = 0; i < h.meshCount; i++)
		{
			LoadedMesh tMesh;
			infile.read((char*)&tMesh.mHeader, sizeof(FormatMesh));
			infile.ignore(sizeof(Material));
			infile.ignore(sizeof(Vertex) * tMesh.mHeader.vertexCount);
			if (tMesh.mHeader.jointCount > 0)
			{
				infile.ignore(sizeof(Joint) * tMesh.mHeader.jointCount);
			}
			AnimationHeader tAnimHeader;
			infile.read((char*)&tAnimHeader, sizeof(AnimationHeader));
			if (tMesh.mHeader.animationCount > 0)
			{
				for (int j = 0; j < tAnimHeader.animationLength; j++)
				{
					infile.ignore(sizeof(animatedJoint) * tMesh.mHeader.jointCount);
				}
			}

			if (tMesh.mHeader.blendShapeCount > 0)
			{
				for (int j = 0; j <tMesh.mHeader.blendShapeCount; j++)
				{
					infile.ignore(sizeof(BlendShapeHeader));
					infile.ignore(sizeof(Vertex) * tMesh.mHeader.vertexCount);
				}
			}

		}

		infile.read((char*)loadedCameras, sizeof(FormatCamera) * h.cameraCount);

		infile.close();
		return true;
	}

	return false;
}

bool Importer::loadGroups(const char * filename)
{
	return false;
}

bool Importer::loadScene(const char * filename)
{
	return false;
}



int Importer::getVertexCount() const
{
	if (this->header.meshCount > 0)
		return this->loadedMeshes->mHeader.vertexCount;
	return 0;
}


int Importer::getVertexCount(const char* meshName) const
{
	for (int i = 0; i < this->header.meshCount; i++)
	{
		if (!strcmp(this->loadedMeshes[i].mHeader.meshName, meshName))
			return this->loadedMeshes[i].mHeader.vertexCount;
	}

	return -1;
}

int Importer::getVertexCount(int meshID) const
{
	return this->loadedMeshes[meshID].mHeader.vertexCount;
}

int Importer::getMeshCount() const
{
	return this->header.meshCount;
}

void Importer::getMinBBox(float & minX, float & minY, float & minZ)
{
	minX = this->loadedMeshes->mHeader.minX;
	minY = this->loadedMeshes->mHeader.minY;
	minZ = this->loadedMeshes->mHeader.minZ;
}

void Importer::getMaxBBox(float & maxX, float & maxY, float & maxZ)
{
	maxX = this->loadedMeshes->mHeader.maxX;
	maxY = this->loadedMeshes->mHeader.maxY;
	maxZ = this->loadedMeshes->mHeader.maxZ;
}

void Importer::getMinBBox(float & minX, float & minY, float & minZ, int meshID)
{
	minX = this->loadedMeshes[meshID].mHeader.minX;
	minY = this->loadedMeshes[meshID].mHeader.minY;
	minZ = this->loadedMeshes[meshID].mHeader.minZ;
}

void Importer::getMaxBBox(float & maxX, float & maxY, float & maxZ, int meshID)
{
	maxX = this->loadedMeshes[meshID].mHeader.maxX;
	maxY = this->loadedMeshes[meshID].mHeader.maxY;
	maxZ = this->loadedMeshes[meshID].mHeader.maxZ;
}

char * Importer::getMaterialName()
{
	return this->loadedMeshes->material.materialName;
}

char * Importer::getMaterialName(int meshID)
{
	return this->loadedMeshes[meshID].material.materialName;
}

char * Importer::getTextureName()
{
	return this->loadedMeshes->material.textureName;
}

char * Importer::getTextureName(int meshID)
{
	return this->loadedMeshes[meshID].material.textureName;
}

char * Importer::getNormalMapName()
{
	return this->loadedMeshes->material.normalMapName;
}

char * Importer::getNormalMapName(int meshID)
{
	return this->loadedMeshes[meshID].material.normalMapName;
}

char * Importer::getSpecularMapName()
{
	return this->loadedMeshes->material.specularMapName;
}

char * Importer::getSpecularMapName(int meshID)
{
	return this->loadedMeshes[meshID].material.specularMapName;
}

void Importer::getDiffuseColor(float & r, float & g, float & b)
{
	r = this->loadedMeshes->material.diffR;
	g = this->loadedMeshes->material.diffG;
	b = this->loadedMeshes->material.diffB;
}

void Importer::getDiffuseColor(float & r, float & g, float & b, int meshID)
{
	r = this->loadedMeshes[meshID].material.diffR;
	g = this->loadedMeshes[meshID].material.diffG;
	b = this->loadedMeshes[meshID].material.diffB;
}

void Importer::getAmbientColor(float & r, float & g, float & b)
{
	r = this->loadedMeshes->material.ambiR;
	g = this->loadedMeshes->material.ambiG;
	b = this->loadedMeshes->material.ambiB;
}

void Importer::getAmbientColor(float & r, float & g, float & b, int meshID)
{
	r = this->loadedMeshes[meshID].material.ambiR;
	g = this->loadedMeshes[meshID].material.ambiG;
	b = this->loadedMeshes[meshID].material.ambiB;
}

void Importer::getEmissiveColor(float & r, float & g, float & b)
{
	r = this->loadedMeshes->material.emisR;
	g = this->loadedMeshes->material.emisG;
	b = this->loadedMeshes->material.emisB;
}

void Importer::getEmissiveColor(float & r, float & g, float & b, int meshID)
{
	r = this->loadedMeshes[meshID].material.emisR;
	g = this->loadedMeshes[meshID].material.emisG;
	b = this->loadedMeshes[meshID].material.emisB;
}

void Importer::getTransparencyColor(float & r, float & g, float & b)
{
	r = this->loadedMeshes->material.trspR;
	g = this->loadedMeshes->material.trspG;
	b = this->loadedMeshes->material.trspB;
}

void Importer::getTransparencyColor(float & r, float & g, float & b, int meshID)
{
	r = this->loadedMeshes[meshID].material.trspR;
	g = this->loadedMeshes[meshID].material.trspG;
	b = this->loadedMeshes[meshID].material.trspB;
}

void Importer::getSpecularColor(float & r, float & g, float & b)
{
	r = this->loadedMeshes->material.specR;
	g = this->loadedMeshes->material.specG;
	b = this->loadedMeshes->material.specB;
}

void Importer::getSpecularColor(float & r, float & g, float & b, int meshID)
{
	r = this->loadedMeshes[meshID].material.specR;
	g = this->loadedMeshes[meshID].material.specG;
	b = this->loadedMeshes[meshID].material.specB;
}

LoadedMesh Importer::getMesh() const
{
	return LoadedMesh();
}


Vertex * Importer::getVertices() const
{
	return this->loadedMeshes[0].vertices;
}

Vertex * Importer::getVertices(const char* meshName) const
{
	for (int i = 0; i < this->header.meshCount; i++)
	{
		if (!strcmp(this->loadedMeshes[i].mHeader.meshName, meshName))
			return this->loadedMeshes[i].vertices;
	}

	return nullptr;
}

Vertex * Importer::getVertices(int meshID) const
{
	return this->loadedMeshes[meshID].vertices;
}

void Importer::getReflectionColor(float & r, float & g, float & b)
{
	r = this->loadedMeshes->material.reflR;
	g = this->loadedMeshes->material.reflG;
	b = this->loadedMeshes->material.reflB;
}

void Importer::getReflectionColor(float & r, float & g, float & b, int meshID)
{
	r = this->loadedMeshes[meshID].material.reflR;
	g = this->loadedMeshes[meshID].material.reflG;
	b = this->loadedMeshes[meshID].material.reflB;
}

Joint * Importer::getJoints() const
{
	return this->loadedMeshes->skeleton;
}

animatedJoint * Importer::getAnimatedJointsAtKey(int keyFrame)
{
	return this->loadedMeshes->animation.keyFrames[keyFrame].animatedSkeleton;
}

int Importer::getJointCount() const
{
	return this->loadedMeshes->mHeader.jointCount;
}

int Importer::getAnimationLength()
{
	return this->loadedMeshes->animation.animationInfo.animationLength;
}

int Importer::getNrOfAnimations()
{
	return this->loadedMeshes->mHeader.animationCount;
}

int Importer::getBlendShapeCount()
{
	return this->loadedMeshes->mHeader.blendShapeCount;
}

int Importer::getCameraCount()
{
	return this->header.cameraCount;
}

void Importer::getCameraPosition(float & x, float & y, float & z)
{
	x = loadedCameras->x;
	y = loadedCameras->y;
	z = loadedCameras->z;
}

void Importer::getCameraPosition(float & x, float & y, float & z, int cameraID)
{
	x = loadedCameras[cameraID].x;
	y = loadedCameras[cameraID].y;
	z = loadedCameras[cameraID].z;
}

void Importer::getCameraLookAt(float & x, float & y, float & z)
{
	x = loadedCameras->lookAtX;
	y = loadedCameras->lookAtY;
	z = loadedCameras->lookAtZ;
}

void Importer::getCameraLookAt(float & x, float & y, float & z, int cameraID)
{
	x = loadedCameras[cameraID].lookAtX;
	y = loadedCameras[cameraID].lookAtY;
	z = loadedCameras[cameraID].lookAtZ;
}

void Importer::getCameraUP(float & x, float & y, float & z)
{
	x = loadedCameras->upX;
	y = loadedCameras->upY;
	z = loadedCameras->upZ;
}

void Importer::getCameraUP(float & x, float & y, float & z, int cameraID)
{
	x = loadedCameras[cameraID].upX;
	y = loadedCameras[cameraID].upY;
	z = loadedCameras[cameraID].upZ;
}

int Importer::getLightCount()
{
	return this->header.lightCount;
}

char * Importer::getLightType()
{
	return this->loadedLights->type;
}

void Importer::getLightPosition(float & x, float & y, float & z)
{
	x = this->loadedLights->x;
	y = this->loadedLights->y;
	z = this->loadedLights->z;
}

void Importer::getLightPosition(float & x, float & y, float & z, int lightID)
{
	x = this->loadedLights[lightID].x;
	y = this->loadedLights[lightID].y;
	z = this->loadedLights[lightID].z;
}

void Importer::getLightColor(float & x, float & y, float & z)
{
	x = this->loadedLights->r;
	y = this->loadedLights->g;
	z = this->loadedLights->b;
}

void Importer::getLightColor(float & x, float & y, float & z, int lightID)
{
	x = this->loadedLights[lightID].r;
	y = this->loadedLights[lightID].g;
	z = this->loadedLights[lightID].b;
}

float Importer::getLightIntensity()
{
	return this->loadedLights->intensity;
}

float Importer::getLightIntensity(int lightID)
{
	return this->loadedLights[lightID].intensity;
}

void Importer::destroyMesh()
{
	if (this->header.meshCount > 0)
	{
		for (int i = 0; i < this->header.meshCount; i++)
		{
			delete[] this->loadedMeshes[i].vertices;
			if (this->loadedMeshes[i].mHeader.jointCount > 0)
			{
				delete[] this->loadedMeshes[i].skeleton;

				if (this->loadedMeshes[i].animation.animationInfo.animationLength > 0)
				{
					for (int j = 0; j < this->loadedMeshes[i].animation.animationInfo.animationLength; j++)
					{
						delete[] this->loadedMeshes[i].animation.keyFrames[j].animatedSkeleton;
					}

					delete[] this->loadedMeshes[i].animation.keyFrames;
				}
			}
		}


		delete[] this->loadedMeshes;

		this->header.meshCount = 0;
	}

	if (this->header.cameraCount > 0)
	{
		delete[] this->loadedCameras;
		this->header.cameraCount = 0;
	}

	if (this->header.lightCount > 0)
	{
		delete[] this->loadedLights;
		this->header.lightCount = 0;
	}
}