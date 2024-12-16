#pragma once


#include "Mash.h"
#include "DX.h"
#include "shaders.h"
#include "GEMLoader.h"
#include"texture.h"
#include "Camera.h"
#include "MyWindow.h"

struct Vertex {
	Vec3 position;
	Colour colour;
};


struct STATIC_VERTEX
{
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
};

struct ANIMATED_VERTEX
{
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
	unsigned int bonesIDs[4];
	float boneWeights[4];
};



class Mash {
public:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	int indicesSize;
	UINT strides;
	

	void init(void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, DXcore& dxcore) {
		D3D11_BUFFER_DESC bd;
		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA data;
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;
		dxcore.device->CreateBuffer(&bd, &data, &indexBuffer);
		bd.ByteWidth = vertexSizeInBytes * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		data.pSysMem = vertices;
		dxcore.device->CreateBuffer(&bd, &data, &vertexBuffer);
		indicesSize = numIndices;
		strides = vertexSizeInBytes;
	}
	void init(std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices, DXcore& dxcore)
	{
		init(&vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size(), dxcore);
	}

	void init(std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices, DXcore& dxcore)
	{
		init(&vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size(), dxcore);
	}


	void draw(DXcore& dxcore) {
		UINT offsets = 0;
		dxcore.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dxcore.devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		dxcore.devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dxcore.devicecontext->DrawIndexed(indicesSize, 0, 0);

	}
};

class Plane {
public:
	Mash geometry;             
	Matrix planeWorld;  
	Camera camera;

	
	STATIC_VERTEX addVertex(const Vec3& pos, const Vec3& normal, float tu, float tv) {
		return { pos, normal, Vec3(1, 0, 0), tu, tv };
	}

	
	void init(DXcore& core, TextureManager* textures, const std::string& textureFilename) {
		std::vector<STATIC_VERTEX> vertices = {
			addVertex(Vec3(-500, 0, -500), Vec3(0, 1, 0), 0, 0),  
			addVertex(Vec3(500, 0, -500), Vec3(0, 1, 0), 1, 0),  
			addVertex(Vec3(-500, 0, 500), Vec3(0, 1, 0), 0, 1),   
			addVertex(Vec3(500, 0, 500), Vec3(0, 1, 0), 1, 1),    
		};

		std::vector<unsigned int> indices = {
			0, 1, 2, 
			1, 3, 2   
		};

		
		textures->load(&core, textureFilename);

		
		geometry.init(vertices, indices, core);
	}

	
	void draw(shader* shader, DXcore& core, TextureManager* textures, const std::string& textureFilename, CameraManager camera) {
		
		
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &camera.vp);

		
		shader->updateShaderResourceTexturePS(core, textures->find(textureFilename), "tex");

		
		shader->apply(&core);
		geometry.draw(core);
	}
};

class Water {
public:
	Mash geometry;            
	Matrix waterWorld;       
	

	
	STATIC_VERTEX addVertex(const Vec3& pos, const Vec3& normal, float tu, float tv) {
		return { pos, normal, Vec3(1, 0, 0), tu, tv };
	}

	
	void init(DXcore& core, TextureManager* textures, const std::string& textureFilename, int width = 50, int depth = 50, int spacing = 5) {
		std::vector<STATIC_VERTEX> vertices;
		std::vector<unsigned int> indices;

		
		for (int z = -depth / 2; z <= depth / 2; z += spacing) {
			for (int x = -width / 2; x <= width / 2; x += spacing) {
				float u = (x + width / 2) / (float)width;
				float v = (z + depth / 2) / (float)depth;

				vertices.push_back(addVertex(Vec3(x, 0, z), Vec3(0, 1, 0), u, v));
			}
		}

		
		int vertsPerRow = width / spacing + 1;
		for (int z = 0; z < depth / spacing; ++z) {
			for (int x = 0; x < width / spacing; ++x) {
				int topLeft = z * vertsPerRow + x;
				int topRight = topLeft + 1;
				int bottomLeft = (z + 1) * vertsPerRow + x;
				int bottomRight = bottomLeft + 1;

				
				indices.push_back(topLeft);
				indices.push_back(bottomLeft);
				indices.push_back(topRight);

				
				indices.push_back(topRight);
				indices.push_back(bottomLeft);
				indices.push_back(bottomRight);
			}
		}

	
		textures->load(&core, textureFilename);

		
		geometry.init(vertices, indices, core);
	}


	
	void draw(shader* shader, DXcore& core, TextureManager* textures, const std::string& textureFilename, CameraManager camera, float dt) {
		
		

		
		shader->updateConstantVS("WaterShader", "staticMeshBuffer", "W", &waterWorld);
		shader->updateConstantVS("WaterShader", "staticMeshBuffer", "VP", &camera.vp);
		shader->updateConstantVS("WaterShader", "WaveBuffer", "time", &dt);

		
		shader->updateShaderResourceTexturePS(core, textures->find(textureFilename), "tex");

		
		shader->apply(&core);
		geometry.draw(core);
	}
};



class SimplePond {
public:
	Mash geometry;            
	Matrix planeWorld;  
	Camera camera;
	float width = 25;
	float depth = 25;
	float height = 25;

	
	STATIC_VERTEX addVertex(const Vec3& pos, const Vec3& normal, float tu, float tv) {
		return { pos, normal, Vec3(1, 0, 0), tu, tv };
	}

	
	void init(DXcore& core, TextureManager* textures, const std::string& textureFilename) {
		std::vector<STATIC_VERTEX> vertices = {
			
			addVertex(Vec3(-width, -height, -depth), Vec3(0, 1, 0), 0, 0),  
			addVertex(Vec3(width, -height, -depth), Vec3(0, 1, 0), 1, 0),   
			addVertex(Vec3(-width, -height, depth), Vec3(0, 1, 0), 0, 1),   
			addVertex(Vec3(width, -height, depth), Vec3(0, 1, 0), 1, 1),   

			
			addVertex(Vec3(-width, -height, -depth), Vec3(0, 0, 1), 0, 0),
			addVertex(Vec3(width, -height, -depth), Vec3(0, 0, 1), 1, 0),
			addVertex(Vec3(-width, 0, -depth), Vec3(0, 0, 1), 0, 1),
			addVertex(Vec3(width, 0, -depth), Vec3(0, 0, 1), 1, 1),

		
			addVertex(Vec3(-width, -height, depth), Vec3(0, 0, -1), 0, 0),
			addVertex(Vec3(width, -height, depth), Vec3(0, 0, -1), 1, 0),
			addVertex(Vec3(-width, 0, depth), Vec3(0, 0, -1), 0, 1),
			addVertex(Vec3(width, 0, depth), Vec3(0, 0, -1), 1, 1),

		
			addVertex(Vec3(-width, -height, -depth), Vec3(1, 0, 0), 0, 0),
			addVertex(Vec3(-width, -height, depth), Vec3(1, 0, 0), 1, 0),
			addVertex(Vec3(-width, 0, -depth), Vec3(1, 0, 0), 0, 1),
			addVertex(Vec3(-width, 0, depth), Vec3(1, 0, 0), 1, 1),

			
			addVertex(Vec3(width, -height, -depth), Vec3(-1, 0, 0), 0, 0),
			addVertex(Vec3(width, -height, depth), Vec3(-1, 0, 0), 1, 0),
			addVertex(Vec3(width, 0, -depth), Vec3(-1, 0, 0), 0, 1),
			addVertex(Vec3(width, 0, depth), Vec3(-1, 0, 0), 1, 1),
		};

		std::vector<unsigned int> indices = {
			
		   0, 1, 2,  1, 3, 2,

		  
		   4, 5, 6,  5, 7, 6,

		  
		   8, 9, 10,  9, 11, 10,

		  
		   12, 13, 14,  13, 15, 14,

		 
		   16, 17, 18,  17, 19, 18,
		};

		
		textures->load(&core, textureFilename);

		
		geometry.init(vertices, indices, core);
	}

	void setPosition() {
		
		planeWorld =  Matrix::translation(Vec3(0,2,0));
		
	}
	
	void draw(shader* shader, DXcore& core, TextureManager* textures, const std::string& textureFilename, CameraManager camera) {


		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &camera.vp);

		
		shader->updateShaderResourceTexturePS(core, textures->find(textureFilename), "tex");

		
		shader->apply(&core);
		geometry.draw(core);
	}
};

class cube {
public:
	Mash geometry;
	Matrix cubeWorld;
	Matrix vp1;
	Vec3 from = Vec3(11 * cos(t), 5, 11 * sin(t));
	Vec3 to = Vec3(0.0f, 1.0f, 0.0f);
	Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
	float t = 0.0f;
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		//Frame frame;
		//frame.fromVector(n);
		//v.tangent = frame.u;
		v.tangent = Vec3(0, 0, 0);
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	void init(DXcore& core) {
		std::vector<STATIC_VERTEX> vertices;

		
		Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
		Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
		Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
		Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
		Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
		Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);

		
		vertices.push_back(addVertex(p0, Vec3(0, 0, -1), 0, 0));
		vertices.push_back(addVertex(p1, Vec3(0, 0, -1), 1, 0));
		vertices.push_back(addVertex(p2, Vec3(0, 0, -1), 1, 1));
		vertices.push_back(addVertex(p3, Vec3(0, 0, -1), 0, 1));

		
		vertices.push_back(addVertex(p4, Vec3(0, 0, 1), 0, 0));
		vertices.push_back(addVertex(p5, Vec3(0, 0, 1), 1, 0));
		vertices.push_back(addVertex(p6, Vec3(0, 0, 1), 1, 1));
		vertices.push_back(addVertex(p7, Vec3(0, 0, 1), 0, 1));

		
		vertices.push_back(addVertex(p3, Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(p2, Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(p6, Vec3(0, 1, 0), 1, 1));
		vertices.push_back(addVertex(p7, Vec3(0, 1, 0), 0, 1));

	
		vertices.push_back(addVertex(p0, Vec3(0, -1, 0), 0, 0));
		vertices.push_back(addVertex(p1, Vec3(0, -1, 0), 1, 0));
		vertices.push_back(addVertex(p5, Vec3(0, -1, 0), 1, 1));
		vertices.push_back(addVertex(p4, Vec3(0, -1, 0), 0, 1));

		
		vertices.push_back(addVertex(p0, Vec3(-1, 0, 0), 0, 0));
		vertices.push_back(addVertex(p3, Vec3(-1, 0, 0), 1, 0));
		vertices.push_back(addVertex(p7, Vec3(-1, 0, 0), 1, 1));
		vertices.push_back(addVertex(p4, Vec3(-1, 0, 0), 0, 1));

	
		vertices.push_back(addVertex(p1, Vec3(1, 0, 0), 0, 0));
		vertices.push_back(addVertex(p2, Vec3(1, 0, 0), 1, 0));
		vertices.push_back(addVertex(p6, Vec3(1, 0, 0), 1, 1));
		vertices.push_back(addVertex(p5, Vec3(1, 0, 0), 0, 1));

		std::vector<unsigned int> indices;
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(0); indices.push_back(2); indices.push_back(3);
		indices.push_back(4); indices.push_back(5); indices.push_back(6);
		indices.push_back(4); indices.push_back(6); indices.push_back(7);
		indices.push_back(8); indices.push_back(9); indices.push_back(10);
		indices.push_back(8); indices.push_back(10); indices.push_back(11);
		indices.push_back(12); indices.push_back(13); indices.push_back(14);
		indices.push_back(12); indices.push_back(14); indices.push_back(15);
		indices.push_back(16); indices.push_back(17); indices.push_back(18);
		indices.push_back(16); indices.push_back(18); indices.push_back(19);
		indices.push_back(20); indices.push_back(21); indices.push_back(22);
		indices.push_back(20); indices.push_back(22); indices.push_back(23);

		geometry.init(vertices, indices, core);

	}

	void draw(shader* shader, DXcore& core) {
		Vec3 from = Vec3(11 * cos(t), 5, 11 * sin(t));
		Vec3 to = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
		Matrix v = vp1.lookat(from, Vec3(0, 1, 0), Vec3(0, 1, 0));
		//Matrix p = vp1.projection(1.f, 90.f, 0.1f, 100.f);
		//vp1 = v * p;
		shader->updateConstantVS("CubeModel", "staticMeshBuffer", "W", &cubeWorld);
		shader->updateConstantVS("CubeModel", "staticMeshBuffer", "VP", &vp1);
		geometry.draw(core);


	}
	void updateWorld(const Matrix& cubeworld) {
		cubeWorld = cubeworld; 
	}


};

class Sphere {
public:
	Mash geometry;
	float pi = 3.141592653589793f;
	Matrix planeWorld;
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0, 0, 0);
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	void init(DXcore& core, int rings, int segments, float radius, TextureManager* textures, std::string textureFilenames) {
		std::vector<STATIC_VERTEX> vertices;
		for (int lat = 0; lat <= rings; lat++) {
			float theta = lat * pi / rings;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);
			for (int lon = 0; lon <= segments; lon++) {
				float phi = lon * 2.0f * pi / segments;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);
				Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta, radius * sinTheta * sinPhi);
				Vec3 normal = position.normalize();
				float tu = 1.0f - (float)lon / segments;
				float tv = 1.0f - (float)lat / rings;
				vertices.push_back(addVertex(position, normal, tu, tv));
			}
		}
		std::vector<unsigned int> indices;
		for (int lat = 0; lat < rings; lat++)
		{
			for (int lon = 0; lon < segments; lon++)
			{
				int current = lat * (segments + 1) + lon;
				int next = current + segments + 1;
				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);
				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}
		textures->load(&core, textureFilenames);
		geometry.init(vertices, indices, core);
	}

	void draw(shader* shader, DXcore& core, CameraManager camera, TextureManager* textures, std::string textureFilenames) {
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &camera.vp);
		shader->updateShaderResourceTexturePS(core, textures->find(textureFilenames), "tex");
		shader->apply(&core);
		geometry.draw(core);
	}
};

struct AABB {
	Vec3 min;  
	Vec3 max;  

	
	void update(const Vec3& center, const Vec3& size) {
		min = center - size;
		max = center + size;
	}

	
	void logToFile(const std::string& message) const {
		std::ofstream logFile("log.txt", std::ios::app);  
		if (logFile.is_open()) {
			logFile << message << std::endl;
			logFile.close();
		}
		else {
			
		}
	}

	
	bool isColliding(const AABB& other) const {
		bool collided = (min.x <= other.max.x && max.x >= other.min.x) &&
			
			(min.z <= other.max.z && max.z >= other.min.z);


		return collided;
	}
	bool isCollidingXZ(const AABB& other) const {

		return (min.x <= other.max.x && max.x >= other.min.x) &&
			(min.z <= other.max.z && max.z >= other.min.z);

	}
};
class GEMModel {
public:
	std::vector<Mash> meshes;
	Matrix vp;
	Matrix planeWorld;
	float t = 0.f;
	std::vector<std::string> textureFilenames;
	std::vector<std::string> mormalFilenames;
	Vec3 currentScale = Vec3(1.0f, 1.0f, 1.0f);
	Vec3 currentPosition;
	AABB collider;

	void init(DXcore& core, std::string filename, TextureManager& textures) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(filename, gemmeshes);
		for (int i = 0; i < gemmeshes.size(); i++) {
			Mash mesh;
			std::vector<STATIC_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
			}
			mesh.init(vertices, gemmeshes[i].indices, core);
			textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
			textures.load(&core, gemmeshes[i].material.find("diffuse").getValue());
			mormalFilenames.push_back(gemmeshes[i].material.find("normals").getValue());
			textures.load(&core, gemmeshes[i].material.find("normals").getValue());

			meshes.push_back(mesh);
		}

	}
	void scaleModel(const Vec3& newScale) {
		currentScale = newScale;
		planeWorld = Matrix().scaling(currentScale) * Matrix::translation(currentPosition);
		updateCollider();
	}
	void setPosition(const Vec3& newPosition) {
		currentPosition.x = newPosition.x;
		currentPosition.y = newPosition.y;
		currentPosition.z = newPosition.z;

		
		planeWorld = Matrix().scaling(currentScale) * Matrix::translation(currentPosition);
		updateCollider();
	}

	void updateCollider() {
		collider.update(currentPosition, currentScale); 

	}
	

	void draw(shader* shader, DXcore& core, TextureManager& textures, CameraManager camera) {
		
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &camera.vp);
		shader->apply(&core);
		for (int i = 0; i < meshes.size(); i++)
		{
			shader->updateTexturePS(core, "normalMap", textures.find(mormalFilenames[i]));
			shader->updateTexturePS(core, "tex", textures.find(textureFilenames[i]));
			
			
			meshes[i].draw(core);
		}

	}
	void generateForestFile(const std::string& filename) {
		std::ofstream file(filename);
		if (!file.is_open()) {
			std::cerr << "无法打开文件 " << filename << std::endl;
			return;
		}

		
		int gridSize = 100;   
		int exclusionRange = 25;  
		int spacing = 5;    
		int treeCount = 0;

		file << "TreePositions\n";

		
		for (int x = -gridSize / 2; x <= gridSize / 2; x += spacing) {
			for (int z = -gridSize / 2; z <= gridSize / 2; z += spacing) {
				
				if (abs(x) <= exclusionRange && abs(z) <= exclusionRange) {
					continue;
				}

				
				file << x << " " << 0 << " " << z << "\n";
				treeCount++;
				if (treeCount >= 100) break;
			}
			if (treeCount >= 100) break;
		}

		file.close();

	}
};

struct Bone
{
	std::string name;
	Matrix offset;
	int parentIndex;
};

struct Skeleton
{
	std::vector<Bone> bones;
	Matrix globalInverse;
};

struct AnimationFrame
{
	std::vector<Vec3> positions;
	std::vector<Quaternion> rotations;
	std::vector<Vec3> scales;
};

class AnimationSequence
{
	// stores animation data for one animation
public:
	std::vector<AnimationFrame> frames; // transformation data for bones
	float ticksPerSecond; // speed of animation

	Vec3 interpolate(Vec3 p1, Vec3 p2, float t) {
		return ((p1 * (1.0f - t)) + (p2 * t)); // linear interpolation
	}
	Quaternion interpolate(Quaternion q1, Quaternion q2, float t) {
		return Quaternion::Slerp(q1, q2, t); // ensure smooth rotation
	}
	float duration() {
		return ((float)frames.size() / ticksPerSecond);// total duration of animation sequence
	}

	void calcFrame(float t, int& frame, float& interpolationFact)
	{
		// find frame given time
		interpolationFact = t * ticksPerSecond;
		frame = (int)floorf(interpolationFact);
		interpolationFact = interpolationFact - (float)frame; // fractional part
		frame = min(frame, frames.size() - 1);
	}

	int nextFrame(int frame)
	{
		// find next frame, returns index for next frame
		return min(frame + 1, frames.size() - 1);
	}

	Matrix interpolateBoneToGlobal(Matrix* matrices, int baseFrame, float interpolationFact, Skeleton* skeleton, int boneIndex) {
		//Interpolates transformations for a specific bone and calculates its global transformation.
		int nextFrameIndex = nextFrame(baseFrame);
		Matrix temp;
		// interpolate scale, rotation and translation
		Matrix scale = temp.scaling(interpolate(frames[baseFrame].scales[boneIndex], frames[nextFrameIndex].scales[boneIndex], interpolationFact));
		Matrix rotation = interpolate(frames[baseFrame].rotations[boneIndex], frames[nextFrameIndex].rotations[boneIndex], interpolationFact).toMatrix();
		Matrix translation = Matrix::translation1(interpolate(frames[baseFrame].positions[boneIndex], frames[nextFrameIndex].positions[boneIndex], interpolationFact), temp);
		Matrix local = scale * rotation * translation;
		if (skeleton->bones[boneIndex].parentIndex > -1) {
			Matrix global = local * matrices[skeleton->bones[boneIndex].parentIndex];
			return global;
		}
		return local;
	}


};

class Animation
{
	// store skeleton and animations
public:
	std::map<std::string, AnimationSequence> animations; // map of animation sequences
	Skeleton skeleton; // The skeleton structure defining bone hierarchy and transformations.

	int bonesSize() {
		int bonesSize = skeleton.bones.size();
		return bonesSize;
	}

	// Calculation of the current frame and interpolation factor
	void calcFrame(std::string name, float t, int& frame, float& interpolationFact) {
		animations[name].calcFrame(t, frame, interpolationFact);
	}
	// bone transformation interpolation to the corresponding AnimationSequence.
	Matrix interpolateBoneToGlobal(std::string name, Matrix* matrices, int baseFrame, float interpolationFact, int boneIndex) {
		return animations[name].interpolateBoneToGlobal(matrices, baseFrame, interpolationFact, &skeleton, boneIndex);
	}

	void calcFinalTransforms(Matrix* matrices)
	{
		for (int i = 0; i < bonesSize(); i++)
		{
			matrices[i] = skeleton.bones[i].offset * matrices[i] * skeleton.globalInverse;
		}
	}
};

class AnimationInstance
{
public:
	Animation animation;
	std::string currentAnimation;
	Matrix matrices[1000];
	float t;

	void resetAnimationTime()
	{
		t = 0;
	}
	bool animationFinished()
	{
		if (t > animation.animations[currentAnimation].duration())
		{
			return true;
		}
		return false;
	}

	void update(std::string name, float dt) {
		if (name == currentAnimation) {
			t += dt;
		}
		else {
			currentAnimation = name;  t = 0;
		}
		if (animationFinished() == true) { resetAnimationTime(); }
		int frame = 0;
		float interpolationFact = 0;
		animation.calcFrame(name, t, frame, interpolationFact);
		int boneCount = animation.skeleton.bones.size();
		for (int i = 0; i < boneCount; i++)
		{
			matrices[i] = animation.interpolateBoneToGlobal(name, matrices, frame, interpolationFact, i);
		}
		animation.calcFinalTransforms(matrices);

	}

};



class GEMModelanimation {
public:
	float t = 0.0f;            
	Matrix vp;               
	Matrix planeWorld;          
	std::vector<Mash> meshes; 
	std::vector<std::string> textureFilenames;
	std::vector<std::string> normalFilenames;
	Animation animation;
	AnimationInstance instance;
	bool animationTriggered = false;
	float moveSpeed = 2.0f;
	Vec3 currentPosition;
	Vec3 movementDirection;
	float currentRotationy = 0.0f;   
	float currentRotationx = 0.0f;   
	float rotationSpeed = 3.0f;
	Vec3 currentScale = Vec3(1.0f, 1.0f, 1.0f);  
	Vec3 currentRotationvec = Vec3(0.0f,0.0f,0.0f);
	AABB collider;
	Vec3 defaultcollider = Vec3(1.0f, 1.0f, 1.0f);


	
	void init(DXcore& core, const std::string& filename, TextureManager& textures) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		loader.load(filename, gemmeshes, gemanimation);
		currentPosition = Vec3(0.0f, 0.0f, 0.0f);
		movementDirection = Vec3(0.0f, 0.0f, 0.0f);


		for (int i = 0; i < gemmeshes.size(); i++) {
			Mash mesh;
			std::vector<ANIMATED_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++) {
				ANIMATED_VERTEX  v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
				vertices.push_back(v);

			}
			mesh.init(vertices, gemmeshes[i].indices, core);
			textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
			textures.load(&core, gemmeshes[i].material.find("diffuse").getValue());

			
			normalFilenames.push_back(gemmeshes[i].material.find("normal").getValue());
			textures.load(&core, gemmeshes[i].material.find("normal").getValue(), true);

			meshes.push_back(mesh);

		}




		for (int i = 0; i < gemanimation.bones.size(); i++)
		{
			Bone bone;
			bone.name = gemanimation.bones[i].name;
			memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
			bone.parentIndex = gemanimation.bones[i].parentIndex;
			animation.skeleton.bones.push_back(bone);
		}

		for (int i = 0; i < gemanimation.animations.size(); i++)
		{
			std::string name = gemanimation.animations[i].name;
			AnimationSequence aseq;
			aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;
			for (int n = 0; n < gemanimation.animations[i].frames.size(); n++)
			{
				AnimationFrame frame;
				for (int index = 0; index < gemanimation.animations[i].frames[n].positions.size(); index++)
				{
					Vec3 p;
					Quaternion q;
					Vec3 s;
					memcpy(&p, &gemanimation.animations[i].frames[n].positions[index], sizeof(Vec3));
					frame.positions.push_back(p);
					memcpy(&q, &gemanimation.animations[i].frames[n].rotations[index], sizeof(Quaternion));
					frame.rotations.push_back(q);
					memcpy(&s, &gemanimation.animations[i].frames[n].scales[index], sizeof(Vec3));
					frame.scales.push_back(s);
				}
				aseq.frames.push_back(frame);
			}
			animation.animations.insert({ name, aseq });
		}
		instance.animation = animation;


	}
	void LogToFile(const std::string& message) {
		std::ofstream logFile("log.txt", std::ios::app);  
		logFile << message << std::endl;
		logFile.close();
	}

	void triggerAnimation() {
		animationTriggered = true;
	}

	
	void resetAnimation() {
		animationTriggered = false;
		instance.resetAnimationTime();
	}
	void scaleModel(const Vec3& newScale) {
		currentScale = newScale;
		planeWorld = Matrix().scaling(currentScale) * Matrix().rotationX(currentRotationx) * Matrix().rotationY(currentRotationy) * Matrix::translation(currentPosition);
		updateCollider();
	}
	void moveModel(Vec3 a) {
		planeWorld = planeWorld.translation(a);
	}
	void rotationX(float degree) {
		currentRotationx = degree;
		planeWorld = Matrix().scaling(currentScale) * Matrix().rotationX(currentRotationx) * Matrix().rotationY(currentRotationy) * Matrix::translation(currentPosition);
		updateCollider();
	}
	

	void ani(const std::string& animationName, float dt) {
		if (!animationName.empty()) {
			instance.update(animationName, dt);
		}
	}
	void updateCollider() {
		collider.update(currentPosition, defaultcollider);
	}

	


	



	
	void setPosition(const Vec3& newPosition) {
		currentPosition.x = newPosition.x;
		currentPosition.y = newPosition.y;
		currentPosition.z = newPosition.z;

		planeWorld = Matrix().scaling(currentScale) * Matrix().rotationX(currentRotationx) * Matrix().rotationY(currentRotationy) * Matrix::translation(currentPosition);
		updateCollider();
	}
	void setcollider(const Vec3& newcollider) {
		defaultcollider = newcollider;
		updateCollider();
	}

	void looktarget(const Vec3& target) {
		
		Vec3 direction = (target - currentPosition).normalize();

		
		currentRotationy = atan2(-direction.x, direction.z) * 180.0f / M_PI + 180.0f;
		currentRotationx = atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z)) * 180.0f / M_PI+90.0f;


		
		planeWorld = Matrix().scaling(currentScale) * Matrix().rotationX(currentRotationx) * Matrix().rotationY(currentRotationy) * Matrix::translation(currentPosition);
		updateCollider();
	}

	


	void chaseCamera(CameraManager& camera, float dt, float chaseSpeed) {
		Vec3 cameraPosition = camera.camera.getPosition();
		Vec3 direction = Vec3(cameraPosition.x - currentPosition.x, 0.0f, cameraPosition.z - currentPosition.z).normalize();

		float distance = sqrt(
			pow(cameraPosition.x - currentPosition.x, 2) +
			pow(cameraPosition.y - currentPosition.y, 2) +
			pow(cameraPosition.z - currentPosition.z, 2)
		);
		if (distance > 30.0f) {  
			currentPosition += direction * chaseSpeed * dt*7;
			currentRotationy = atan2(-direction.x, direction.z) * 180.0f / M_PI;
			currentRotationx = atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z)) * 180.0f / M_PI ;


			
			planeWorld = Matrix().scaling(currentScale) * Matrix().rotationX(currentRotationx) * Matrix().rotationY(currentRotationy) * Matrix::translation(currentPosition);
			updateCollider();
		}


		
		updateCollider();
	}



	

	void draw(shader* shader, DXcore& core, TextureManager& textures, float dt, CameraManager camera) {
		
		//ani(animationname, dt);
		shader->updateConstantVS("Animated", "animatedMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("Animated", "animatedMeshBuffer", "VP", &camera.vp);
		shader->updateConstantVS("Animated", "animatedMeshBuffer", "bones", instance.matrices);
		

		shader->apply(&core);
		
		for (int i = 0; i < meshes.size(); i++) {
			shader->updateShaderResourceTexturePS(core, textures.find(textureFilenames[i]),"tex" );
			meshes[i].draw(core);
		}
	}



};

struct Particle {
	Vec3 position;     
	Vec3 velocity;     
	float lifetime;   
	bool active;      
};

class ParticleMesh {
public:
	Mash geometry;


	void initSphere(DXcore& core, float radius, int slices, int stacks) {
		std::vector<STATIC_VERTEX> vertices;
		std::vector<unsigned int> indices;

		for (int i = 0; i <= stacks; i++) {
			float phi = M_PI * i / stacks;  

			for (int j = 0; j <= slices; j++) {
				float theta = 2.0f * M_PI * j / slices;  

				Vec3 pos(
					radius * sin(phi) * cos(theta),  
					radius * cos(phi),               
					radius * sin(phi) * sin(theta)  
				);

				Vec3 normal = pos.normalize();
				float u = (float)j / slices;
				float v = (float)i / stacks;

				vertices.push_back({ pos, normal, Vec3(1, 0, 0), u, v });
			}
		}

		
		for (int i = 0; i < stacks; i++) {
			for (int j = 0; j < slices; j++) {
				int first = i * (slices + 1) + j;
				int second = first + slices + 1;

				
				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);

				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}

		geometry.init(vertices, indices, core);
	}


	void draw(DXcore& core) {
		geometry.draw(core);
	}
};

class ParticleSystem {
public:
	std::vector<Particle> particles;
	ParticleMesh mesh; 
	int maxParticles = 10000;
	Vec3 emitterPosition;
	float particleLifetime = 5.0f;

	void init(DXcore& core, const Vec3& emitter, int maxCount, CameraManager camera) {
		emitterPosition = emitter;
		maxParticles = maxCount;
		particles.resize(maxParticles);

		
		mesh.initSphere(core, 0.1f, 16, 16);

		for (auto& p : particles) {
			resetParticle(p,camera);
		}
	}

	void resetParticle(Particle& p,  CameraManager camera) {
		
		float spawnRangeX = 100.0f;  
		float spawnRangeZ = 100.0f;  

		
		p.position = camera.camera.getWorldPosition() + Vec3(randRange(-spawnRangeX, spawnRangeX), 100.0f, randRange(-spawnRangeZ, spawnRangeZ));
		p.velocity = Vec3(0, randRange(-20, -30), 0); 
		p.lifetime = particleLifetime;
		p.active = true;
	}


	void update(float dt, CameraManager camera) {
		for (auto& p : particles) {
			if (!p.active) continue;

			p.position += p.velocity * dt;
			

			if (p.position.y <= 0.0f) {
				resetParticle(p,camera);
			}
		}
	}

	void draw(shader* shader, DXcore& core, CameraManager camera) {
		for (auto& p : particles) {
			if (!p.active) continue;

			Matrix world = Matrix::translation(p.position);
			shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &world);
			shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &camera.vp);
			shader->apply(&core);
			mesh.draw(core); 
		}
	}

private:
	float randRange(float min, float max) {
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}
};







