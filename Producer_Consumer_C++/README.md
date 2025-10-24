# Sandwich Simulation System

## Overview
The Sandwich Simulation System is a multithreaded program that models the interaction between an Agent and three Chefs. The Agent produces random combinations of sandwich ingredients, and the Chefs use those ingredients to make sandwiches, depending on the ingredient they already have.

The system uses:
- **Threads**: To simulate the concurrent actions of the Agent and Chefs.
- **Shared Resources**: A `Box` object acts as a shared container for ingredient combinations.
- **Synchronization**: Mutexes and condition variables ensure thread-safe interactions.

## Components

### Classes

1. **Box**:
   - Represents the shared container for ingredient pairs.
   - Handles synchronization using mutexes and condition variables.
   - Attributes:
     - `contents`: The current item in the box.
     - `empty`: Boolean flag to indicate if the box is empty.
     - `finished`: Boolean flag to signal when the Agent is done producing.
   - Methods:
     - `put(Type item)`: Adds an item to the box.
     - `get()`: Retrieves an item from the box.
     - `set_finished()`: Marks the box as finished, signaling no more items will be produced.

2. **Agent**:
   - Produces random ingredient pairs and places them into the box.
   - Attributes:
     - `box`: Reference to the shared `Box`.
     - `ingredients`: List of available ingredients.
   - Methods:
     - `operator()()`: Main function that runs in a separate thread to produce ingredients.

3. **Chef**:
   - Uses ingredients from the box to make sandwiches.
   - Attributes:
     - `has_ingredient`: The ingredient already available to the Chef.
     - `box`: Reference to the shared `Box`.
   - Methods:
     - `operator()(const std::string &name)`: Main function that runs in a separate thread to consume ingredients.

### Multithreading
- The **Agent** runs in its own thread, producing ingredient pairs.
- Each **Chef** runs in its own thread, waiting for ingredients and making sandwiches.
- Synchronization ensures safe access to the shared `Box`.

### UML Class Diagram
The class diagram includes:
- **Box**: Aggregated by both `Agent` and `Chef`.
- **Agent**: Aggregates `Box`.
- **Chef**: Aggregates `Box` and interacts with it to consume ingredients.

## Sequence of Events
1. The **Agent** produces random pairs of ingredients and puts them in the `Box`.
2. Each **Chef** waits for an ingredient pair from the `Box`.
3. A **Chef**:
   - Makes a sandwich if the pair contains ingredients it needs.
   - Puts the pair back in the `Box` if not usable.
4. The **Agent** signals completion when all pairs are produced.
5. Chefs exit once the `Box` signals it is finished.

## How to Run the Program

### Prerequisites
- A C++17 (or later) compiler.
- Standard libraries supporting threading and synchronization.

### Compilation
Compile the program using the following command:
```bash
g++ -std=c++17 -pthread threads.cpp -o threads
```

### Execution
Run the program:
```bash
./threads
```

### Sample Output
The output will show:
- The **Agent** producing ingredient pairs.
- Each **Chef** making sandwiches or skipping items they can't use.
- Threads exiting when no more items are available.

## Key Features
- **Thread-Safe Access**: Ensures no race conditions when accessing the `Box`.
- **Synchronization**: Uses condition variables to handle producer-consumer interactions.
- **Extensibility**: Easy to add more ingredients or modify the behavior of Chefs.

## Limitations
- The number of ingredients and iterations is fixed.
- No dynamic addition of Chefs or Agents during runtime.

## Contact
For questions or feedback, contact Agrim Kasaju at kasajuagrim@gmail.com.
