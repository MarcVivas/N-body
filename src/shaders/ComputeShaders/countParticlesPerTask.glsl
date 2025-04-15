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

layout(std430, binding=5) buffer nodesBuffer
{
    Node nodes[];
};

layout(std430, binding=9) buffer tasksBuffer
{
    Task tasks[];
};

layout(std430, binding=0) buffer positionsBuffer
{
    vec4 positions[];
};

layout(std430, binding=10) buffer particleOffsetsBuffer
{
    ivec2 particleOffsets[];
};




vec4 getMaxBoundary(inout Node node){
    return vec4(node.mass.w, node.centerOfMass.w, node.minBoundary.w, 0);
}

vec4 getMinBoundary(inout Node node){
    return vec4(node.minBoundary.x, node.minBoundary.y, node.minBoundary.z, 0);
}


vec4 getQuadrantCenter(inout Node node) {
    const vec4 maxBoundary = getMaxBoundary(node);
    const vec4 minBoundary = getMinBoundary(node);
    const float width = maxBoundary.x - minBoundary.x;
    const float height = maxBoundary.y - minBoundary.y;
    const float depth = maxBoundary.z - minBoundary.z;
    return vec4(minBoundary.x + (width / 2.f), minBoundary.y + (height / 2.f), minBoundary.z + (depth / 2.f), 0.f);
}

int getFirstChild(inout Node node){
    return int(node.mass.y);
}

/**
     * Get the next node based on the quadrant the particle is in
     * @param pos
     * @param i
     * @return
*/
int getNextNode(vec4 pos, int i){
  Node node = nodes[i];
  const vec4 quadCenter = getQuadrantCenter(node);
  const int quadrantIndex = int(pos.x < quadCenter.x) | (int(pos.y < quadCenter.y) << 1) | (int(pos.z < quadCenter.z) << 2);
  return getFirstChild(node) + quadrantIndex;
}

uniform int totalParticles;
uniform int maxDepth;
uniform int totalTasks;

void main() {
    uint index = gl_GlobalInvocationID.x;

    if(index >= totalParticles){
        return;
    }

    int i = 0;
    int depth = 0;
    const vec4 pos = positions[index];

    while(depth < maxDepth){
        // Go down until you are at max depth
        // Traverse the tree
        i = getNextNode(pos, i);
        depth++;
    }

    particleOffsets[index].x = atomicAdd(tasks[i % totalTasks].totalParticles, 1);  // Store offset
    particleOffsets[index].y = i;   // Store node index
    tasks[i % totalTasks].root = i; // No race condition here, it's the same value for all threads

}