#ifndef MESH_FORMAT
#define MESH_FORMAT

struct MyFormat {
	int meshCount = 0;
	int cameraCount = 0;
	int lightCount = 0;
	int groupCount = 0;
};

struct FormatCamera {
	float x, y, z;
	float upX, upY, upZ;
	float lookAtX, lookAtY, lookAtZ;
};

struct Material {
	char materialName[100];
	char textureName[100];
	char normalMapName[100];
	char specularMapName[100];

	//Basic values
	float diffR, diffG, diffB;			//Diffuse
	float ambiR, ambiG, ambiB;			//Ambient
	float emisR, emisG, emisB;			//Emissive
	float trspR, trspG, trspB;			//Transparency

										//Phong values
	float specR, specG, specB, specCos; //Specular, SpecCos = shininess
	float reflR, reflG, reflB;			//Reflected color
};

struct Light {
	char type[100];
	float x, y, z;
	float r, g, b;
	float intensity;
};

struct FormatMesh {
	char meshName[100];
	int vertexCount;
	char materialName[100];
	float minX, maxX, minY, maxY, minZ, maxZ;
	int jointCount = 0;
	int animationCount = 0;
	int blendShapeCount = 0;
};

struct Weight
{
	float value = -1.0f;
	int jointIndex = -1;
};

struct AnimationHeader {
	int animationLength = 0;
};

struct Vertex {
	float x, y, z;
	float nx, ny, nz;

	float r, g, b;
	float u, v;

	float weight1;
	float weight2;
	float weight3;
	float weight4;
	int joint1;
	int joint2;
	int joint3;
	int joint4;

	float tangentX, tangentY, tangentZ;
	float binormalX, binormalY, binormalZ;
};

struct Joint {
	char name[100];
	int index;
	int parentIndex;
	float globalBindposeInverse[4][4];
};

struct animatedJoint {
	char name[100];
	int keyFrame;
	int index;
	int parentIndex;
	float keyFrameTransform[4][4];
};

struct BlendShapeHeader {
	float weight;
};

struct GroupHeader {
	char groupName[100];
	float transform[4][4];
	int nrOfChildren;
};

struct GroupChildren
{
	char name[100];
};

#endif // !MESH_FORMAT
