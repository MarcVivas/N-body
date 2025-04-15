#version 430 core

#define BLOCK_SIZE 256

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;


struct Node{
	vec4 mass; // mass = mass.x; firstChild=mass.y; next=mass.z; maxBoundary.x=mass.w
	vec4 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2); maxBoundary.y = centerOfMass.w
	vec4 minBoundary;  // maxBoundary.z = minBoundary.w
};

layout(std430, binding=5) buffer nodesBuffer
{
	Node nodes[];
};

int getFirstChild(inout Node node){
    return int(node.mass.y);
}

bool isOccupied(inout Node node) {
    // return mass > 0.f;
    return node.mass.x > 0.f;
}

vec4 getCenterOfMass(inout Node node){
    return vec4(node.centerOfMass.xyz, 0.f);
}

void setCenterOfMass(const vec4 centerOfMass, const uint i){
    nodes[i].centerOfMass.xyz = centerOfMass.xyz;
}

void setMass(const float mass, const uint i){
    nodes[i].mass.x = mass;
}

bool isLeaf(inout Node node){
    // firstChild < 0.f;
    return node.mass.y < 0.f; 
}

uniform int left; 
uniform int right;

void main(){
	uint globalId = gl_GlobalInvocationID.x;

	if(globalId >= right-left) return;

	// Get the parent node
	uint parentId = left + globalId;

    Node parentNode = nodes[parentId];

    if(isLeaf(parentNode)) return; // The parent node is a leaf

	// Compute the center of mass and mass of the current parent node        
    vec4 centerOfMass = vec4(0.f);
    vec4 mass = vec4(0.f);

    for (int j = 0; j < 8; ++j) {
        const int childIndex = getFirstChild(parentNode) + j;
        Node childNode = nodes[childIndex];
        if (!isOccupied(childNode))  continue; // The leaf is empty
        centerOfMass += getCenterOfMass(childNode) * childNode.mass.x;
        mass.x += childNode.mass.x;
    }

    setCenterOfMass(centerOfMass / mass.x, parentId);
    setMass(mass.x, parentId);

}