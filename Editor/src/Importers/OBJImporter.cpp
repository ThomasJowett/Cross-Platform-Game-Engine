#include "OBJImporter.h"

#include "Engine.h"

void OBJImporter::ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	PROFILE_FUNCTION();

	std::ifstream inFile;
	inFile.open(filepath);

	if (!inFile.good())
	{
		CLIENT_ERROR("Could not Open {0}", filepath.string());
		return;
	}

	std::vector<Vector3f> verts;
	std::vector<Vector3f> normals;
	std::vector<Vector2f> texCoords;

	std::vector<uint32_t> vertIndices;
	std::vector<uint32_t> normalIndices;
	std::vector<uint32_t> textureIndices;

	std::string input;

	std::string currentGroupName;

	Vector3f vert;
	Vector2f texCoord;
	Vector3f normal;
	uint32_t vInd[3];
	uint32_t tInd[3];
	uint32_t nInd[3];
	std::string beforeFirstSlash;
	std::string afterFirstSlash;
	std::string afterSecondSlash;

	while (!inFile.eof())
	{
		inFile >> input;

		if (input[0] == '#')
		{
			std::getline(inFile, input);
		}
		else if (input == "v")
		{
			inFile >> vert.x;
			inFile >> vert.y;
			inFile >> vert.z;

			verts.push_back(vert);
		}
		else if (input == "vt")
		{
			inFile >> texCoord.x;
			inFile >> texCoord.y;

			texCoords.push_back(texCoord);
		}
		else if (input == "vn")
		{
			inFile >> normal.x;
			inFile >> normal.y;
			inFile >> normal.z;

			normals.push_back(normal);
		}
		else if (input == "f")
		{
			for (int i = 0; i < 3; ++i)
			{
				inFile >> input;
				size_t slash = input.find("/");
				size_t secondSlash = input.find("/", slash + 1);

				beforeFirstSlash = input.substr(0, slash);
				afterFirstSlash = input.substr(slash + 1, secondSlash - slash - 1);
				afterSecondSlash = input.substr(secondSlash + 1);

				vInd[i] = (uint32_t)atoi(beforeFirstSlash.c_str());
				tInd[i] = !afterFirstSlash.empty() ? (uint32_t)atoi(afterFirstSlash.c_str()) : 1;
				nInd[i] = !afterSecondSlash.empty() ? (uint32_t)atoi(afterSecondSlash.c_str()) : 1;
			}

			for (int i = 0; i < 3; ++i)
			{
				vertIndices.push_back(vInd[i] - 1);
				textureIndices.push_back(tInd[i] - 1);
				normalIndices.push_back(nInd[i] - 1);
			}
		}
		else if (input == "mtllib" || input == "usemtl")
		{
			std::getline(inFile, input);
		}
		else if (input == "g")
		{
			std::getline(inFile, input);
			if (input != currentGroupName)
			{
				//TODO: Create a new material slot
				currentGroupName = input;
			}
		}
		else if (input == "s")
		{
			std::getline(inFile, input);
		}
	}
	inFile.close();

	std::vector<Vector3f> expandedVertices;
	std::vector<Vector3f> expandedNormals;
	std::vector<Vector2f> expandedTexCoord;
	uint32_t numIndices = (uint32_t)vertIndices.size();
	for (uint32_t i = 0; i < numIndices; i++)
	{
		expandedVertices.push_back(verts[vertIndices[i]]);
		expandedNormals.push_back(normals[normalIndices[i]]);
		expandedTexCoord.push_back(texCoords[textureIndices[i]]);
	}

	std::vector<uint32_t> meshIndices;
	std::vector<Vector3f> meshVertices;
	meshVertices.reserve(expandedVertices.size());
	std::vector<Vector3f> meshNormals;
	meshNormals.reserve(expandedNormals.size());
	std::vector<Vector2f> meshTexCoords;
	meshTexCoords.reserve(expandedTexCoord.size());


}

void OBJImporter::InsertTangentsIntoArray()
{
}

Vector3f OBJImporter::CalculateTangent(Vector3f pos0, Vector3f pos1, Vector3f pos2, Vector2f tex0, Vector2f tex1, Vector2f tex2)
{
	//using Eric Lengyel's approach with a few modifications
	//from Mathematics for 3D Game Programmming and Computer Graphics
	// want to be able to trasform a vector in Object Space to Tangent Space
	// such that the x-axis cooresponds to the 's' direction and the
	// y-axis corresponds to the 't' direction, and the z-axis corresponds
	// to <0,0,1>, straight up out of the texture map

	Vector3f P = pos1 - pos0;
	Vector3f Q = pos2 - pos0;

	float s1 = tex1.x - tex0.x;
	float t1 = tex1.y - tex0.y;
	float s2 = tex2.x - tex0.x;
	float t2 = tex2.y - tex0.y;

	//we need to solve the equation
	// P = s1*T + t1*B
	// Q = s2*T + t2*B
	// for T and B

	//this is a linear system with six unknowns and six equatinos, for TxTyTz BxByBz
	//[px,py,pz] = [s1,t1] * [Tx,Ty,Tz]
	// qx,qy,qz     s2,t2     Bx,By,Bz

	//multiplying both sides by the inverse of the s,t matrix gives
	//[Tx,Ty,Tz] = 1/(s1t2-s2t1) *  [t2,-t1] * [px,py,pz]
	// Bx,By,Bz                      -s2,s1	    qx,qy,qz  

	//solve this for the unormalized T and B to get from tangent to object space

	float tmp = 0.0f;
	if (fabsf(s1 * t2 - s2 * t1) <= 0.0001f)
	{
		tmp = 1.0f;
	}
	else
	{
		tmp = 1.0f / (s1 * t2 - s2 * t1);
	}

	Vector3f tangent((t2 * P.x - t1 * Q.x) * tmp,
		(t2 * P.y - t1 * Q.y) * tmp,
		(t2 * P.z - t1 * Q.z) * tmp);

	tangent.Normalize();
	return tangent;
}
