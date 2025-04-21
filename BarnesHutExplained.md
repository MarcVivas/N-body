## Algorithm Breakdown

Each step involves building the octree and calculating forces. The parallel octree construction and force calculation phases are outlined below:

### 1. Bounding Box Computation

*   **Goal:** Determine the minimum spatial extent containing all particles.
*   **Method:** A parallel reduction operation is performed on the GPU across all particle positions.
*   **Input:** Array of particle positions.
*   **Output:** Minimum and maximum corner coordinates (e.g., `min_x`, `min_y`, `min_z`, `max_x`, `max_y`, `max_z`).
*   **Parallelism:** Highly parallel reduction algorithm executed on the GPU.

### 2. Octree Structure Generation (Expansion Phase)

*   **Goal:** Pre-build the upper levels of the octree structure down to a certain depth, creating "subtrees". This defines the spatial partitioning *before* particles are inserted.
*   **Method:** The octree structure is expanded level by level in parallel. Starting from the root node (defined by the bounding box), each level generates the child nodes for the nodes created in the previous level. Nodes are allocated in batches of 8. `firstChild` pointers are set, and siblings are linked using `nextSibling`.
*   **Important:** No particles are inserted into the tree during this phase. The structure represents potential spatial divisions.
*   **Parallelism:** Each level's node generation is performed in parallel by GPU threads, operating on the nodes created in the prior level.

### 3. Particle Assignment to Subtrees

*   **Goal:** Determine which pre-generated subtree (from Step 2) each particle belongs to and prepare for parallel insertion.
*   **Method:** This involves three sub-steps implemented with efficient parallel primitives:
    1.  **Count:** A parallel kernel counts how many particles fall into the spatial region defined by each subtree root.
    2.  **Prefix Sum (Scan):** A parallel prefix sum is performed on the counts. This calculates the starting offset for each subtree's particle list within a global temporary array.
    3.  **Scatter/Store:** A parallel kernel iterates through the particles again. Based on which subtree a particle belongs to and the offsets calculated by the prefix sum, the particle's index is written into the appropriate segment of the global temporary array.
*   **Output:** An array where particle indices are grouped by the subtree they belong to. Also, the counts per subtree.
*   **Parallelism:** Count, Scan, and Scatter operations are executed in parallel on the GPU.

### 4. Parallel Particle Insertion (Task Execution)

*   **Goal:** Insert the particles into their designated subtrees, creating deeper nodes within those subtrees as needed.
*   **Method:** Based on the particle assignments and counts from Step 3, parallel tasks are launched on the GPU. Each task is responsible for inserting a list of particles (belonging to one subtree) into its corresponding subtree root node. Since particles within one task only affect nodes within that specific subtree (below the pre-generated level), these insertions happen concurrently without race conditions between tasks. The `firstChild`/`nextSibling` structure is used and updated within each task. Nodes are allocated in batches of 8 as required during insertion. Leaf nodes containing a single particle store its mass and position (as its center of mass).
*   **Parallelism:** Independent GPU thread blocks (or work groups) execute the insertion tasks for different subtrees concurrently.

### 5. Mass Properties Propagation (Bottom-Up)

*   **Goal:** Calculate the total mass and center of mass for all internal nodes based on their children.
*   **Method:** A bottom-up traversal approach is used, implemented with parallel kernels operating level by level, starting from the deepest level towards the root.
    1.  **Leaf Initialization:** Mass properties for leaf nodes are set during particle insertion (Step 4).
    2.  **Parallel Aggregation:** For each internal node at a given level, a kernel calculates its aggregate mass and weighted center of mass by summing the contributions from its direct children. The `firstChild` and `nextSibling` pointers are used to locate all children of a node. This process repeats level by level, moving upwards. An array with the parent nodes is used in this step for the bottom up traversal. 
*   **Parallelism:** Mass calculation for all nodes at a given level is performed in parallel. Synchronization is required between levels to ensure child properties are computed before their parent's.

### 6. Octree Pruning (Top-Down)

*   **Goal:** Identify and mark or logically remove empty branches in the octree that were created during expansion but received no particles during insertion. This optimizes the subsequent force calculation step.
*   **Method:** A top-down traversal approach is used, implemented with parallel kernels processing the tree level by level, starting from the root.
    1.  **Parallel Check:** For each internal node at a given level, a kernel checks its children (using `firstChild` and `nextSibling`).
    2.  **Empty Branch Identification:** If an internal node has zero total mass (as computed in Step 5) or all its children have been marked as empty/pruned in a previous level's pass, this node itself is marked as empty, and its `firstChild` pointer is set to an invalid index (e.g., -1). This effectively removes the empty branch from subsequent traversals.
    3.  **Sibling Chain Update:** Pruning also involves updating `nextSibling` pointers to bypass pruned nodes, maintaining a compact list of valid children.
*   **Parallelism:** Node checks and marking at each level are performed in parallel. Synchronization between levels ensures parent nodes are processed after their potential children's status is known from the level below.

### 7. Force Calculation

*   **Goal:** Compute the net force acting on each particle.
*   **Method:** A parallel kernel is launched, with one GPU thread assigned to each particle. Each thread traverses the *pruned* octree starting from the root **stacklessly**, using the `firstChild` and `nextSibling` pointers to navigate. For each node encountered during traversal:
    *   If the node is marked as empty/pruned, it's skipped.
    *   The Barnes-Hut opening criterion (based on node size `s`, distance `d`, and threshold parameter `θ`: `s/d < θ`) is applied.
    *   If the criterion is met (node is far enough away) or the node is a leaf (containing a single particle), the node's aggregate mass and center of mass (computed in Step 5) are used to calculate the force contribution on the particle.
    *   If the criterion is not met and the node is a non-empty internal node, the thread traverses deeper into the node's children using the `firstChild` pointer. Siblings are visited using the `nextSibling` pointer.
*   **Parallelism:** Force calculation for each particle is independent and runs in parallel on the GPU.