![pic](https://github.com/Heerdam/MdVis/blob/master/preview.jpg)

# MdVis

MdVis is a high performance visualizer written in c++ and opengl. It is used namely for visualizing output from MdAtom.

## Building
Clone the repository with 
````
git clone git@github.com:Heerdam/MdVis.git.
````
Important:
MdVis needs c++17. Make sure that at least gcc 9 is installed. Furthermore, **your gpu must support opengl 4.3 or it will not run!**

### GNU/ Linux
Build and run MdVis as following:
````
mkdir bin
cd bin
cmake ..
make -j4
./mdatom [path]
````
path is either a valid path to a .traj file or nothing to show the demo.traj file.

### Windows
Run the Cmake gui to creat the .sln file. In Visual Studio set MdVis as startup project and build/run it.

## Using MdVis and Features
All flags to configurating MdVis are located in the Defines.h header file.

### General configuration
#### Window size 
#### Widget size 
#### Interpolation type 
#### Cyclic boundary conditions
#### Logging 

### Performance
#### Icosahedron
#### SSAO
#### Computing spline 

## Trajectory Specifications
MdVis can parse binary or ascii file format. The flags can be set in the Define.h file to switch between the modes.
### Binary file format ###
The binary file is similar to the ascii file format that it takes the exact same layout. The easiest way to achieve it is to push it into a vector and write the vector to a file (e.g. std::ofstream).
```
{ number_of_atoms, box_size_x box_size_y box_size_z, atom_1_step_0_x, atom_1_step_0_y .... }
```
### Ascii file format
```
number_of_Atoms
box_size_x box_size_y box_size_z
atom_1_step_0_x atom_1_step_0_y atom_1_step_0_z
  ...
atom_n_step_0_x atom_n_step_0_y atom_n_step_0_z
atom_1_step_1_x atom_1_step_1_y atom_1_step_1_z
  ...
atom_n_step_1_x atom_n_step_1_y atom_n_step_1_z
atom_1_step_n_x atom_1_step_n_y atom_1_step_n_z
  ...
atom_n_step_n_x atom_n_step_n_y atom_n_step_n_z
```

### Setting up MdAtom
#### Binary
Replace the function writeBeforeRun() in the file TrajectoryFileWriter.cpp with the following function:
````
void TrajectoryFileWriter::writeBeforeRun() {
    ofstream fout1; // trajectory output
    if (par.trajectoryOutput) {
        fout1.open(trajectoryCoordinatesFilename, ios::out);
        if (fout1.bad()) {
            throw std::runtime_error("can't open " + trajectoryCoordinatesFilename);
        }
        //fout1 << par.title << endl;
        std::vector<double> tmp(4);
        tmp[0] = static_cast<double>(par.numberAtoms);
        tmp[1] = par.boxSize[0];
        tmp[2] = par.boxSize[1];
        tmp[3] = par.boxSize[2];
        writeOutTrajectoryStepInBinaryForm(tmp);     
    }
}
````

#### Ascii
Implement a writer that outputs the trajectories in the proper format. Don't print the initial positions. 
