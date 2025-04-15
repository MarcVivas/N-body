#version 430 core

#define BLOCK_SIZE 256

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;


struct Node{
	vec4 mass; // mass = mass.x; firstChild=mass.y; next=mass.z; maxBoundary.x=mass.w
	vec4 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2); maxBoundary.y = centerOfMass.w
	vec4 minBoundary;  // maxBoundary.z = minBoundary.w
};

struct Task {
	int root; 
	int totalParticles;
};

layout(std430, binding=9) buffer tasksBuffer
{
	Task tasks[];
};

layout(std430, binding=5) buffer nodesBuffer
{
	Node nodes[];
};

layout(std430, binding=13) buffer subTreeParentsBuffer
{
    int subTreeParents[];
};

layout(std430, binding=14) buffer subTreeNodeCountsBuffer
{
    int subTreeNodeCounts[];
};

layout(std430, binding=15) buffer subTreeParentCountsBuffer
{
    int subTreeParentCounts[];
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

uniform int totalTasks; 

int getPrefixSumValue(int taskId){
    return (taskId >= 0 && taskId < totalTasks) ? tasks[taskId].totalParticles: 0;
}

#define EXTRA_NODES 20

void main(){
    // Get the task ID
    int taskId = int(gl_GlobalInvocationID.x);

    if(taskId >= totalTasks) return;

    Task task = tasks[taskId];
    
    if(task.totalParticles <= 0 || task.root < 0) return;

    
    const int totalParentNodes = subTreeParentCounts[taskId];

    const int prevPrefixSum = getPrefixSumValue(taskId - 1);

    for (int i = 0; i < totalParentNodes; i++) {
        const int parentIndex = subTreeParents[i + prevPrefixSum * EXTRA_NODES];
        Node parent = nodes[parentIndex];

        if(getFirstChild(parent) < parentIndex) continue; 
    
        int firstChild = -1;
        int lastChild = -1;


        for (int j = 0; j < 8; j++) {
            const int childIndex = getFirstChild(parent) + j;
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
        setNext(getNext(parent), lastChild);
        setFirstChild(firstChild, parentIndex);
    }
}