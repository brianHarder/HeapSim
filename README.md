# HeapSim

## Project Overview

HeapSim is a learning tool for memory allocation algorithms and the memory heap that runs as a command-line interface. Users have the ability to allocate and free blocks based on their selected heuristic, and at any point they can obtain a visualization of the heap in its current state. The goal of the project is to strengthen people's understanding of key allocation algorithms through visual learning.

## Features

- Interactive command line interface for heap simulation with allocate & free
- Support for first fit, best fit, worst fit, next fit, and buddy system
- Visual representation of heap state including allocated and free blocks

## Built With

- C
- Unix command line

## Getting Started

### Download the repository

```
git clone https://github.com/brianHarder/HeapSim.git
cd HeapSim
```

### Quick Start (recommended)
Run HeapSim with default heap size of 8192 bytes

```
make run
```

### Manual Build and Run

#### Build the Project
```
make
```

#### Run HeapSim

**Option 1: Default heap size (8192 bytes)**
```
./heapsim
```

**Option 2: Custom heap size**
```
./heapsim <heap_size_in_bytes>
```
