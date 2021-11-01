#include "ModelLoader.h"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../Deps/tiny_obj_loader.h"

using byte = unsigned char;

bool ObjLoader::loadPrimitive(const char* filename, std::vector<Primitive*>& outPrimitives)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);
	if (!ret)
		return false;

	for (size_t i = 0; i < shapes.size(); i++)
	{
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++)
		{
			PrimitiveTriangle* primTriangle = new PrimitiveTriangle();

			size_t fnum = shapes[i].mesh.num_face_vertices[f];
			for (size_t v = 0; v < fnum; v++)
			{
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				primTriangle->vertex[v] = vec3<f32>(
					attrib.vertices[3 * idx.vertex_index],
					attrib.vertices[3 * idx.vertex_index + 1],
					attrib.vertices[3 * idx.vertex_index + 2]);
			}
			index_offset += fnum;

			primTriangle->updateAabb();
			outPrimitives.push_back(primTriangle);
		}
	}

	return true;
}

bool VoxLoader::loadPrimitive(const char* filename, std::vector<Primitive*>& outPrimitives)
{
	std::vector<VoxelChunk> voxelChunks;
	bool succ = loadInternal(filename, voxelChunks);

	if (!succ)	return false;

	for (auto& voxelChunk : voxelChunks)
	{
		vec3<f32> positon(voxelChunk.x, voxelChunk.y, voxelChunk.z);
		unsigned color = palette[voxelChunk.index - 1];
		float a = ((color >> 24) & 0xff) / 255.0f;
		float b = ((color >> 16) & 0xff) / 255.0f;
		float g = ((color >> 8) & 0xff) / 255.0f;
		float r = ((color) & 0xff) / 255.0f;

		vec3<f32> offset(dimX * 0.5f - 0.5f, 0.0f, dimZ * -0.5f + 0.5f);
		vec3<f32> pmin = positon - vec3<f32>(0.5f) + offset; //x轴翻转
		vec3<f32> pmax = positon + vec3<f32>(0.5f) + offset;

		PrimitiveAabox* primAabox = new PrimitiveAabox();
		primAabox->aabb.min = pmin;
		primAabox->aabb.max = pmax;
		primAabox->material.color = vec3<f32>(r, g, b);
		outPrimitives.push_back(primAabox);
	}

	return voxelChunks.size() > 0;
}

bool VoxLoader::loadInternal(const char* filename, std::vector<VoxelChunk>& voxelChunks)
{
	FILE* file = nullptr;
	fopen_s(&file, filename, "rb");
	if (!file)	return false;

	//文件大小
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	//https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
	//"VOX "
	char fileType[5];
	fileType[4] = '\0';
	fread(fileType, sizeof(char), 4, file);
	if (strcmp(fileType, "VOX ") != 0)
	{
		std::cerr << "error file format " << std::endl;
		fclose(file);
		return false;
	}

	//version
	int version;
	fread(&version, sizeof(int), 1, file);

	while (ftell(file) < fileSize)
	{
		//chunk
		char chunkID[5];
		chunkID[4] = '\0';
		int num_bytes;
		int num_child_bytes;

		fread(chunkID, sizeof(char), 4, file);
		fread(&num_bytes, sizeof(int), 1, file);
		fread(&num_child_bytes, sizeof(int), 1, file);

		if (strcmp(chunkID, "PACK") == 0)
		{
			int num_models;
			fread(&num_models, sizeof(int), 1, file);
			//std::cout << "num_models:" << num_models << std::endl;
		}
		else if (strcmp(chunkID, "SIZE") == 0)
		{
			int dims[3];
			fread(dims, sizeof(int), 3, file);
			dimX = dims[0];
			dimY = dims[2];
			dimZ = dims[1];
			//std::cout << "dims:" << dimX << ", " << dimY << ", " << dimZ << std::endl;
		}
		else if (strcmp(chunkID, "XYZI") == 0)
		{
			int numVoxels;
			fread(&numVoxels, sizeof(int), 1, file);
			voxelChunks.reserve(numVoxels);
			for (int i = 0; i < numVoxels; i++)
			{
				byte voxel[4];
				fread(voxel, sizeof(byte), 4, file);

				if (voxel[3] < 0 || voxel[3] > 255)
				{
					//std::cerr << "error file format " << std::endl;
					fclose(file);
					return false;
				}

				VoxelChunk voxelChunk;
				voxelChunk.x = -voxel[0];
				voxelChunk.y = voxel[2];
				voxelChunk.z = voxel[1];
				voxelChunk.index = voxel[3];
				voxelChunks.push_back(voxelChunk);
			}
		}
		else if (strcmp(chunkID, "RGBA") == 0)
		{
			fread(palette, sizeof(int), 256, file);
		}
		else
		{
			fseek(file, num_bytes, SEEK_CUR);
			//std::cout << "chunkID:" << chunkID << std::endl;
		}
	}

	fclose(file);
	return true;
}
