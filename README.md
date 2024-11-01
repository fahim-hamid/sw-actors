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
wget https://github.com/actor-framework/actor-framework/archive/refs/tags/0.18.6.tar.gz
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

2. Modify the paths in the `Makefile` to match your CAF installation path.

3. Run:

```
make
```


## Running SUMMA-Actors
After compilation, SW-Actors can be executed using the job.sh script. Modify job.sh to match your system’s configurations, such as paths, resources, and any specific environment modules you may need.

For basic use, run:
```
bash job.sh
```
  
## Credits
The implementation of SW-Actors builds on the foundation provided by the [Smith–Waterman algorithm](https://icbi.i-med.ac.at/courses/bioinformatics_ex_2021/7265238.pdf) and the [C++ Actor Framework](https://github.com/actor-framework/actor-framework). We credit the original creators of the C++ Actor Framework which allowed us to implement the actor model into SUMMA-Actors. Links to their research work can be found below.

 * Charousset, D., Schmidt, T. C., Hiesgen, R., Wählisch, M., 2013: Native actors: 
 a scalable software platform for distributed, heterogeneous environments. _AGERE!_, 
 [doi:10.1145/2541329.2541336](http://dx.doi.org/10.1145/2541329.2541336).

 * Charousset, D., Schmidt, T. C., Hiesgen, R., 2016: Revisiting actor programming in 
 C++. _Computer Languages, Systems & Structures_, [doi:10.1016/j.cl.2016.01.002](http://
 dx.doi.org/10.1016/j.cl.2016.01.002)