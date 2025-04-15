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

layout(std430, binding=12) buffer taskParticlesIndexesBuffer
{
    int taskParticlesIndexes[];
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


layout(std430, binding=3) buffer massesBuffer
{
    vec4 masses[];
};



layout(std430, binding=10) buffer particleOffsetsBuffer
{
    ivec2 particleOffsets[];
};

void setMinBoundary(const vec4 b, const int i){
    nodes[i].minBoundary.xyz = b.xyz;
}

void setMaxBoundary(const vec4 b, const int i){
    nodes[i].mass.w = b.x;
    nodes[i].centerOfMass.w = b.y;
    nodes[i].minBoundary.w = b.z; 
}

void setBoundingBox(const vec4 minB, const vec4 maxB, const int i){
	setMinBoundary(minB, i);
	setMaxBoundary(maxB, i);
}

void setFirstChild(const float firstChild, const int i){
    nodes[i].mass.y = firstChild;
}

void setMass(const float mass, const int i){
    nodes[i].mass.x = mass;
}

void setNext(const float next, const int i){
    nodes[i].mass.z = next; 
}

void setCenterOfMass(const vec4 centerOfMass, const int i){
    nodes[i].centerOfMass.xyz = centerOfMass.xyz;
}

vec4 getMinBoundary(const int i){
    return vec4(nodes[i].minBoundary.xyz, 0);
}

vec4 getMaxBoundary(const int i){
    return vec4(nodes[i].mass.w, nodes[i].centerOfMass.w, nodes[i].minBoundary.w, 0.f);
}
vec4 getMinBoundary(inout Node node){
    return vec4(node.minBoundary.xyz, 0);
}

vec4 getMaxBoundary(inout Node node){
    return vec4(node.mass.w, node.centerOfMass.w, node.minBoundary.w, 0.f);
}

int getFirstChild(const int i){
    return int(nodes[i].mass.y);
}

int getFirstChild(inout Node node){
    return int(node.mass.y);
}

int getNext(const int i){
    return int(nodes[i].mass.z);
}

int getNext(inout Node node){
    return int(node.mass.z);
}

vec4 getCenterOfMass(inout Node node){
    return vec4(node.centerOfMass.xyz, 0.f);
}








bool isLeaf(inout Node node){
    // firstChild < 0.f;
    return node.mass.y < 0.f; 
}

bool isOccupied(inout Node node) {
    // return mass > 0.f;
    return node.mass.x > 0.f;
}


uniform int totalTasks;
uniform int fatherTreeTotalNodes;

#define EXTRA_NODES 20

int getTotalParticles(int taskId) {
    return (taskId >= 0 && taskId < totalTasks) ? tasks[taskId].totalParticles: 0;
}

int getPrefixSumValue(int taskId){
    return (taskId >= 0 && taskId < totalTasks) ? tasks[taskId].totalParticles: 0;
}

int getFirstChildFromTask(int taskId, inout Task task){
    return taskId == 0 ? fatherTreeTotalNodes : fatherTreeTotalNodes + getPrefixSumValue(taskId-1) * EXTRA_NODES; // 20 is for extra space

}



// Get the number of nodes in the subtree for the taskId
int getTaskMaxTotalNodes(int taskId){
    const int currentTotalParticles = getTotalParticles(taskId);
    const int numParticles = abs(getTotalParticles(taskId+1) - currentTotalParticles);
    return (numParticles <= 300) ? currentTotalParticles * 38 : currentTotalParticles * 8;
}

// Get the root of the subtree for the taskId
int getSubtree(int treeId){
    return fatherTreeTotalNodes + getTaskMaxTotalNodes(treeId-1);	// treeId = taskId 
}


// Insert particle into the node
void insertParticle(const vec4 centerOfMass, const float mass, int nodeId){
    setCenterOfMass(centerOfMass, nodeId);
    setMass(mass, nodeId);
}

void createEmptyNode(const int i, const vec4 minB, const vec4 maxB){
    setMinBoundary(minB, i);
    setMaxBoundary(maxB, i);
    setFirstChild(-1.0f, i);
    setMass(-1.0f, i);
}




void subdivide(int i, int firstChild, int subTreeId){
    // Set the first child to the current node count
    setFirstChild(float(firstChild), i);

    // Mark this node as a parent
    subTreeParents[getPrefixSumValue(subTreeId-1) * EXTRA_NODES + subTreeParentCounts[subTreeId]] = i;
    subTreeParentCounts[subTreeId]++;

    // Create the next child nodes
    subTreeNodeCounts[subTreeId] += 8;

    const vec4 maxB = getMaxBoundary(i);
    const vec4 minB = getMinBoundary(i);

    const float width = maxB.x - minB.x;
    const float height = maxB.y - minB.y;
    const float depth = maxB.z - minB.z;

    int currentChild = firstChild;

    for (int j = 0; j < 8; ++j) {
        // Create the boundaries for the new node
        vec4 minBoundary = vec4(0.f);
        vec4 maxBoundary = vec4(0.f);

        // Check the 3rd bit
        if ((j & 4) != 0) {
            minBoundary.z = minB.z;
            maxBoundary.z = minB.z + (depth / 2.f);
        }
        else {
            minBoundary.z = minB.z + (depth / 2.f);
            maxBoundary.z = maxB.z;
        }

        // Check the second bit
        if ((j & 2) != 0) {
            minBoundary.y = minB.y;
            maxBoundary.y = minB.y + (height / 2.f);
        }
        else {
            minBoundary.y = minB.y + (height / 2.f);
            maxBoundary.y = maxB.y;
        }

        // Check the first bit
        if ((j & 1) != 0) {
            minBoundary.x = minB.x;
            maxBoundary.x = minB.x + (width / 2.f);
        }
        else {
            minBoundary.x = minB.x + (width / 2.f);
            maxBoundary.x = maxB.x;
        }


        createEmptyNode(currentChild, minBoundary, maxBoundary);

        // Set the next node
        if (j < 7) {
            // The last child is different
            setNext(firstChild + j + 1, currentChild);
        }

        // Next child node
        currentChild = firstChild + j + 1;
    }

    // The last child node points to the parent's next
    setNext(getNext(i), firstChild+7);
    
}

vec4 getQuadrantCenter(inout Node node) {
    const vec4 maxBoundary = getMaxBoundary(node);
    const vec4 minBoundary = getMinBoundary(node);
    const float width = maxBoundary.x - minBoundary.x;
    const float height = maxBoundary.y - minBoundary.y;
    const float depth = maxBoundary.z - minBoundary.z;
    return vec4(minBoundary.x + (width / 2.f), minBoundary.y + (height / 2.f), minBoundary.z + (depth / 2.f), 0.f);
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
  return int(getFirstChild(node)) + quadrantIndex;
}

float distSquared( vec4 A, vec4 B ){
    const vec4 C = A - B;
    return dot( C, C );
}

void insert(int particleId, int taskId, inout Task task){
    
    // Get the particle's position and mass
    const vec4 pos = positions[particleId];
    const vec4 mass = masses[particleId];

    // Start at the root of the octree
    int i = task.root;

    // Keep the root node
    const int root = i;

   
    const int firstChildFromTask = getFirstChildFromTask(taskId, task);

    // Traverse the tree
    while (true) {
        Node node = nodes[i];
        bool isLeaf = isLeaf(node);
        bool isOccupied = isOccupied(node);


        // Case 1: The node is an empty leaf
        if (isLeaf && !isOccupied) {
            // Proceed to insert the particle
            insertParticle(pos, mass.x, i);
            return;
        }

        // Case 2: There's a particle in this node, therefore we need to subdivide
        if (isLeaf && isOccupied) {
            // Get the current particle's position and mass
            const vec4 pos2 = getCenterOfMass(node);
            const vec4 mass2 = vec4(node.mass.x, 0.f, 0.f, 0.f);

            // Subdivide the current node
            subdivide(i, subTreeNodeCounts[taskId] + firstChildFromTask, taskId);

            // Get child indexes
            int childIndex1 = getNextNode(pos, i);
            int childIndex2 = getNextNode(pos2, i);
            const float dist = distSquared(pos2, pos);

            int count = 0;
            // If both particles go to the same child node we need to keep subdividing
            while (childIndex1 == childIndex2) {

                // Edge case: Both particles are in the same position or very close
                if (dist < 1e-6 || ++count == 5) {
                    insertParticle(vec4((pos.xyz * mass.x + pos2.xyz * mass2.x)/(mass2.x+mass.x), 0), mass2.x + mass.x, childIndex1);  // In this case, insert both particles anyway
                    return;
                }

                // Keep subdividing
                subdivide(childIndex1, subTreeNodeCounts[taskId] + firstChildFromTask, taskId);


                // Recalculate child indexes after further subdivisions
                childIndex1 = getNextNode(pos, childIndex1);
                childIndex2 = getNextNode(pos2, childIndex2);
            }

            // Once particles are in different children, insert them
            insertParticle(pos, mass.x, childIndex1);
            insertParticle(pos2, mass2.x, childIndex2);

            return;
         
        }
        
        
        // Case 3: The node is internal
        // Traverse the tree
        i = getNextNode(pos, i);

    }

}


void propagate(int subTreeId){
    const int totalParentNodes = subTreeParentCounts[subTreeId];


    for (int i = totalParentNodes - 1; i >= 0; i--) {

        // Compute the center of mass and mass of the current parent node
        int parentIndex = subTreeParents[i + getPrefixSumValue(subTreeId-1)*EXTRA_NODES];
        vec4 centerOfMass = vec4(0.f);
        vec4 mass = vec4(0.f);

        Node parent = nodes[parentIndex];


        for (int j = 0; j < 8; ++j) {
            const int childIndex = int(getFirstChild(parent)) + j;
            Node child = nodes[childIndex];
            if (!isOccupied(child))  continue; // The leaf is empty
            centerOfMass += getCenterOfMass(child) * child.mass.x;
            mass.x += child.mass.x;
        }
        setCenterOfMass(centerOfMass / mass.x, parentIndex);
        setMass(mass.x, parentIndex);
    }
}



void main(){

    // each thread will be processing a task
	int taskId = int(gl_GlobalInvocationID.x);
	if (taskId >= totalTasks) return;

	Task task = tasks[taskId];

	if(task.root < 0 || task.root < fatherTreeTotalNodes - totalTasks) return; // Task with no root node


	// Reset the sub tree
	setFirstChild(-1.0f, task.root);    
	setMass(-1.0f, task.root);
    setNext(-1.0f, task.root);
	
	subTreeNodeCounts[taskId] = 0;  // No nodes in the subtree 
	subTreeParentCounts[taskId] = 0; // No parents in the subtree
	
    // The subtree is now empty

	if(task.totalParticles <= 0) return;    // No particles to insert

	    
    const int prevPrefixSum = getPrefixSumValue(taskId-1);
    const int currentPrefixSum = task.totalParticles;
    const int numParticlesInTask = currentPrefixSum - prevPrefixSum;

    // Insert the particles of the task
	for (int i = 0; i < numParticlesInTask; i++){
		const int particleId = taskParticlesIndexes[prevPrefixSum+i];
		insert(particleId, taskId, task);
	}
	
    propagate(taskId);

}