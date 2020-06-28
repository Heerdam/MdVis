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
./mdvis [path]
````
path is either a valid path to a .traj file or nothing to show the demo.traj file.

### Windows
Run the Cmake gui to creat the .sln file. In Visual Studio set MdVis as startup project and build/run it.

## Using MdVis and Features
All flags to configurating MdVis are located in the Defines.h header file.

### General configuration
#### Window size 
Change the size of the window to something fitting. Default: 1920x1080
#### Widget size 
Change the size of the camera widget. Default: 400x400
#### Interpolation type 
MdVis comes with 3 interpolations: no interpolation, linear interpoltation, cubic spline interpolation.
Default: cubic spline interpolation.
#### Cyclic boundary conditions
Toggle this if the cyclic boundary conditions should be enforced.
#### Logging 
If enabled it will print out an overview every frame.

### Performance
#### Icosahedron
Defines how many times the icosahedron gets subdivided. More subdivison means smoother surface but more vertices to draw. High impact on performance.
#### SSAO
Enables/ Disables SSAO (Screen Space Ambient Occlusion). Disabling it will increase performance.
#### Computing spline 
Allows ultra fast concurrent computing of the cubic splines on the gpu. Set this to 0 if your computer doesnt manage to link the shader. (-> if MdVis gets stuck for no reason)
  
### Key bindings
Rotate the camera with left mouse button pressed.
Move the camera with the arrow keys.
Pause/ Unpause the animation with 'space'.
Reset the animation with 'r'.
While pause step through the animation with 'x' and 'y'.
Increase and decrease the speed of the stepping speed with 'page up' and 'page down'.

## Trajectory Specifications
MdVis can parse binary or ascii file format. The flags can be set in the Define.h file to switch between the modes.
### Binary file format (recommended!)
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
Important: Output must be set up in the input file of mdatom. Set TrajectoryOutputFormat to 0 for binary and 1 für ascii.
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

## FAQ and know bugs
### Very low frame rate or strange drawing issues (deformed spheres)
This is a known issue that can happen every so often. I'm not sure why this happens but it is most likely a problem with the libraries or the opengl driver.
Solution: clean and recompile until it works.
