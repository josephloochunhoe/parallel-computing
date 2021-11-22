# Parallel Computing

## A1: Parallelising Banker's Algorithm using OpenMP
The banker’s algorithm is a resource allocation and deadlock avoidance algorithm that tests for safety by simulating the allocation for predetermined maximum possible amounts of all resources, then makes an “s-state” check to test for possible activities, before deciding whether allocation should be allowed to continue.

I developed a serial version of the banker's algorithm and improved it by referring to the concepts elaborated in the research paper that is listed in the references. Then, I tested the parallel version of the code (utilizing the OpenMP library) which it managed to speed up the performance by 45.2% and 66.2% on a dual-core and quad-core platform respectively

Reference:
Ma, X., & Yan, J. (2011, August). An improved parallel banker's algorithm based on Petri net. In Proceedings of 2011 International Conference on Electronic & Mechanical Engineering and Information Technology (Vol. 3, pp. 1538-1541). IEEE.

## A2: Tsunami Detection in a Distributed Wireless Sensor Network (WSN) using POSIX and OpenMPI libraries 
![image](https://user-images.githubusercontent.com/69203738/142839811-1a9dfeb7-4469-4982-a6d9-40d2420fe0a2.png)

The figure above shows the overview of the WSN to detect a tsunami. The tsunameter sensors are positioned in a 3 × 3 grid configuration. Note that Communication Satellites A1 and A2 are actually the same satellite at different position. For the purpose of illustration, two communication satellites are drawn here to reduce the number of overlapping lines between the tsunameter sensors and the communication satellite. In addition, the satellite altimeter covers or observes the same grid layout where the tsunameter sensors are located. Note that satellite altimeter only communicates with the base station and does not communicate with any of the tsunameter sensors.

A simulation model was designed and implemented using a combination of OpenMPI and POSIX libraries.
