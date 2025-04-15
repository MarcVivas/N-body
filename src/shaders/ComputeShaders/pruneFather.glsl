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

bool isOccupied(int i) {
    // return mass > 0.f;
    return nodes[i].mass.x > 0.f;
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

void setFirstChild(const float firstChild, const int i){
    nodes[i].mass.y = firstChild;
}

int getNext(inout Node node){
    return int(node.mass.z);
}

void setNext(const float next, const int i){
    nodes[i].mass.z = next; 
}

uniform int left;
uniform int right;

void main(){
    int globalId = int(gl_GlobalInvocationID.x);
    if(globalId >= right-left) return;

    // Get the parent node
    int parentId = left + globalId;
    Node parentNode = nodes[parentId];

    

    int firstChild = -1; 
    int lastChild = -1; 

    for(int i = 0; i < 8; i++){
        const int childIndex = getFirstChild(parentNode) + i;
        if (isOccupied(childIndex)) {
            if (firstChild == -1) {
                firstChild = childIndex;
                lastChild = childIndex;
                continue;
            }
            setNext(childIndex, lastChild);
            lastChild = childIndex;
        }    
    }

    setNext(getNext(parentNode), lastChild);
    setFirstChild(firstChild, parentId);


}