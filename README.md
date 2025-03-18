# SW-Actors: Smith--Waterman with Actors

SW-Actors uses the Actor Model to perform parallelized sequence alignment with [Smith–Waterman algorithm](https://icbi.i-med.ac.at/courses/bioinformatics_ex_2021/7265238.pdf) across DNA, RNA, or protein sequence pairs. This approach enhances scalability and efficiency for sequence alignment, optimizing computational resources for large datasets. SW-Actors is implemented using the [C++ Actor Framework](https://github.com/actor-framework/actor-framework).

## Documentation
This guide will cover the essentials for compiling and running SW-Actors. For further details or to report any issues, feel free to open an issue in this repository.

## Compiling SW-Actors

### Dependencies
SW-Actors has dependency on the [C++ Actor Framework](https://github.com/actor-framework/actor-framework), specifically the [1.0.1 
release](https://github.com/actor-framework/actor-framework/archive/refs/tags/1.0.1.tar.gz).

The following steps can be used to install the C++ Actor Framework on a Linux system:
```bash
wget https://github.com/actor-framework/actor-framework/archive/refs/tags/1.0.1.tar.gz
tar -xzf 1.0.1.tar.gz
cd actor-framework-1.0.1/
./configure --prefix=/path/to/install
cd build
make # [-j] for parallel build
make install # [use root privileges if necessary]
```

### Steps To Compile
To compile SW-Actors:

1. Clone the repository:

 ```
 git clone https://git.cs.usask.ca/numerical_simulations_lab/actors/papers/sw-actors
 cd SW-Actors
 ```

2. Add the paths in the `Makefile` to your CAF installation path if you used --prefix=/path/to/install option.

3. Run:

```
make
```
---

## Running SW-Actors
After compilation, SW-Actors can be executed using the provided job scripts.  
Modify job scripts to match your system’s configurations, such as paths, resources, and any specific environment modules you may need.

- Use `single.sh` to run on a single node.  
- Use `multi.sh` to run the code in distributed mode across multiple nodes.  

**Example Command**:
```sh
./swActor -D -s -p 4444 -Q ./dataset/BRCA1.fasta -A 40 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=40
```

## Command-line Options

| Option                | Flag                        | Description                                                                  | Default      |
|-----------------------|-----------------------------|------------------------------------------------------------------------------|--------------|
| **Query Input**       | `-Q` / `--query`            | Path to input query sequences                                                | *(empty)*    |
| **Subject Input**     | `-S` / `--subject`          | Path to input subject sequences                                              | *(empty)*    |
| **Worker Actors**     | `-A` / `--actorNumber`      | Number of worker actors per node                                             | `1`          |
| **Match Score**       | `-m` / `--match`            | Score for a match                                                            | `2`          |
| **Mismatch Penalty**  | `-M` / `--mismatch`         | Penalty for a mismatch                                                       | `-1`         |
| **Gap Penalty**       | `-g` / `--gap`              | Penalty for a gap                                                            | `-2`         |
| **Row Divider**       | `-R` / `--devideRow`        | Division factor for scoring matrix rows                                       | `1`          |
| **Column Divider**    | `-C` / `--devideCol`        | Division factor for scoring matrix columns                                    | `1`          |
| **Distributed Mode**  | `-D` / `--distributedMode`  | Enable distributed execution across multiple nodes                            | `false`      |
| **Server Mode**       | `-s` / `--serverMode`       | Start the actor system in server mode (listens for incoming connections)      | `false`      |
| **Host**              | `-H` / `--host`             | Host address (used in client mode only)                                      | `localhost`  |
| **Port**              | `-p` / `--port`             | Port number for server/client communication                                  | `0`          |

### Additional CAF options
You can also pass additional CAF runtime options such as:
```sh
--caf.scheduler.max-threads=40
```
to control the thread pool size used by CAF's scheduler.

---
  
## Credits
The implementation of SW-Actors builds on the foundation provided by the [Smith–Waterman algorithm](https://icbi.i-med.ac.at/courses/bioinformatics_ex_2021/7265238.pdf) and the [C++ Actor Framework](https://github.com/actor-framework/actor-framework). We credit the original creators of the C++ Actor Framework which allowed us to implement the actor model into SUMMA-Actors. Links to their research work can be found below.

 * Charousset, D., Schmidt, T. C., Hiesgen, R., Wählisch, M., 2013: Native actors: 
 a scalable software platform for distributed, heterogeneous environments. _AGERE!_, 
 [doi:10.1145/2541329.2541336](http://dx.doi.org/10.1145/2541329.2541336).

 * Charousset, D., Schmidt, T. C., Hiesgen, R., 2016: Revisiting actor programming in 
 C++. _Computer Languages, Systems & Structures_, [doi:10.1016/j.cl.2016.01.002](http://
 dx.doi.org/10.1016/j.cl.2016.01.002)