#version 430 core

#define BLOCK_SIZE 1024

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;

uniform float squaredSoftening;
uniform float G;
uniform int numParticles;
uniform float theta;
uniform int fatherTreeNodes;

struct Node{
    vec4 mass; // mass = mass.x; firstChild=mass.y; next=mass.z; maxBoundary.x=mass.w
    vec4 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2); maxBoundary.y = centerOfMass.w
    vec4 minBoundary;  // maxBoundary.z = minBoundary.w
};

layout(std430, binding=0) buffer positionsBuffer
{
    vec4 positions[];
};


layout(std430, binding=4) buffer forcesBuffer
{
    vec4 forces[];
};

layout(std430, binding=5) buffer nodesBuffer
{
    Node nodes[];
};


bool isLeaf(float firstChild){
    return firstChild < 0.f;
}

bool isOccupied(float mass) {
    return mass > 0.f;
}
shared Node sharedNodes[BLOCK_SIZE];

void loadSharedNodes(uint threadId){
    if(threadId < BLOCK_SIZE && threadId < fatherTreeNodes){
        sharedNodes[threadId] = nodes[threadId];
    }
    groupMemoryBarrier(); // Ensure all threads have loaded their nodes before proceeding
    barrier(); // Ensure all threads have completed their work before proceeding
}


void main() {
    uint index = gl_GlobalInvocationID.x;
    
    loadSharedNodes(gl_LocalInvocationID.x);

    if (index < numParticles) {



        vec3 force = vec3(0.f);

        int i = 0; // Root of the tree

        Node node; 
        while (i >= 0){
        
            // Get node i
            node = i < BLOCK_SIZE && i < fatherTreeNodes  ? sharedNodes[i] : nodes[i];
            

            const vec4 mass = node.mass;
            const vec4 centerOfMass = node.centerOfMass;
            const vec4 minBoundary = node.minBoundary;
            const vec3 maxBoundary = vec3(mass.w, centerOfMass.w, minBoundary.w);

            // We need to compute the size and the distance between the particle and the node
            const vec3 size = maxBoundary - minBoundary.xyz;
            const float s = max(max(size.x, size.y), size.z);
            const float s_squared = s*s;

            const vec3 vector_i_j = centerOfMass.xyz - positions[index].xyz;
            const float dist_squared = dot(vector_i_j, vector_i_j);  // Squared distance

            const float firstChild = mass.y;
            const float next = mass.z;

            // If the node is an occupied leaf OR satisfies the criteria
            if (isLeaf(firstChild) || s_squared < theta * dist_squared) {
                // Compute the force

                if (isOccupied(mass.x) && dist_squared > 0.f) {
                    const float effective_dist_squared = dist_squared + squaredSoftening;
                    const float inv_sqrt_val = inversesqrt(effective_dist_squared); 
                    const float inv_dist = inv_sqrt_val * inv_sqrt_val * inv_sqrt_val;

                    force += ((vector_i_j * (G * mass.x)) * inv_dist);
                }


                // Go to the next sibling or parent
                i = int(next);
            }
            else {
                // Go down the tree
                i = int(firstChild);
            }
        }



        forces[index] = vec4(force, 0.f);

    }

}


