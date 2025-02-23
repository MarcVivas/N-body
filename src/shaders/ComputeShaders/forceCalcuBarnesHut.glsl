#version 430 core

#define BLOCK_SIZE 320

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;

uniform float squaredSoftening;
uniform float G;
uniform int numParticles;
uniform float theta;

layout(std430, binding=0) buffer positionsBuffer
{
    vec4 positions[];
};


layout(std430, binding=4) buffer forcesBuffer
{
    vec4 forces[];
};

layout(std430, binding=5) buffer nodeMassBuffer
{
    vec4 masses[];  // mass = mass.x; firstChild=mass.y; next=mass.z; maxBoundary.x=mass.w
};

layout(std430, binding=6) buffer centerOfMassBuffer
{
    vec4 centerOfMasses[];  // (x1*m1 + x2*m2) / (m1 + m2); maxBoundary.y = centerOfMass.w
};

layout(std430, binding=7) buffer minBoundaryBuffer
{
    vec4 minBoundaries[];  // maxBoundary.z = minBoundary.w
};


bool isLeaf(float firstChild){
    return firstChild < 0.f;
}

bool isOccupied(float mass) {
    return mass > 0.f;
}

void main() {
    uint index = gl_GlobalInvocationID.x;

    if (index < numParticles) {


        vec3 force = vec3(0.f);

        int i = 0; // Root of the tree


        while (i >= 0){

            // Get node i
            vec4 mass = masses[i];
            vec4 centerOfMass = centerOfMasses[i];
            vec4 minBoundary = minBoundaries[i];
            vec3 maxBoundary = vec3(mass.w, centerOfMass.w, minBoundary.w);

            // We need to compute the size and the distance between the particle and the node
            vec3 size = maxBoundary - minBoundary.xyz;
            float s = max(max(size.x, size.y), size.z);
            float s_squared = s*s;

            const vec3 vector_i_j = centerOfMass.xyz - positions[index].xyz;
            const float dist_squared = dot(vector_i_j, vector_i_j);  // Squared distance

            float firstChild = mass.y;
            float next = mass.z;

            // If the node is an occupied leaf OR satisfies the criteria
            if (isLeaf(firstChild) || s_squared < theta * dist_squared) {
                // Compute the force

                if (isOccupied(mass.x) && dist_squared > 0.f) {
                    const float effective_dist_squared = dist_squared + squaredSoftening; // Apply softening to avoid 0 division
                    const float inv_dist = 1.0f / pow(effective_dist_squared, 1.5f);
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


