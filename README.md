# HeapSim

## Projet Overview

HeapSim is a learning tool for memory allocation.

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