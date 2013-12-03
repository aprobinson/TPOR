#! /usr/bin/env python

import h5py
import numpy as np
import argparse as ap

# Set up the argument parser
description = "This script converts the seed dose distribution used by the" \
    " original greedy algorithm rewrites to the hdf5 format required by the" \
    " new treatment planning code."

parser = ap.ArgumentParser(description=description)

seed_file_name = "seed dose distribution file (with path)"
parser.add_argument('-s', action='append', help=seed_file_name)

seed_type_name = "seed type name"
seed_type_choices = ['Amersham6711Seed', 'Best2301Seed']
parser.add_argument('-t', action='append', choices=seed_type_choices, \
                    help=seed_type_name)

new_seed_file_name = "new seed dose distribution hdf5 file"
parser.add_argument('--new_seed_file', default="BrachytherapySeeds.h5", \
                    help=new_seed_file_name)

# Parse the user's arguments
user_args = parser.parse_args()

# Open the HDF5 file (overwrite if it exists)
hdf5_file = h5py.File(user_args.new_seed_file, 'w')

for s in range(len(user_args.s)):
    # Open up the seed file
    seed_file = open(user_args.s[s], 'r')
    
    # Seed mesh dimensions
    x_mesh_dim = 0
    y_mesh_dim = 0
    z_mesh_dim = 0
    
    # Seed position indices
    seed_x_pos = 0
    seed_y_pos = 0
    seed_z_pos = 0
    
    # Mesh element dimensions
    x_element_dim = 0.0
    y_element_dim = 0.0
    z_element_dim = 0.0
    
    # Parse the organ mask file header
    line = seed_file.readline()
    words = line.split()
    x_mesh_dim = int(words[1])
    
    line = seed_file.readline()
    words = line.split()
    seed_x_pos = int(words[1])
    
    line = seed_file.readline()
    words = line.split()
    x_element_dim = float(words[1])
    
    line = seed_file.readline()
    words = line.split()
    y_mesh_dim = int(words[1])
    
    line = seed_file.readline()
    words = line.split()
    seed_y_pos = int(words[1])
    
    line = seed_file.readline()
    words = line.split()
    y_element_dim = float(words[1])
    
    line = seed_file.readline()
    words = line.split()
    z_mesh_dim = int(words[1])
    
    line = seed_file.readline()
    words = line.split()
    seed_z_pos = int(words[1])
    
    line = seed_file.readline()
    words = line.split()
    z_element_dim = float(words[1])
    
    if s == 0:
        # Add a root level attribute for the mesh element dimensions
        hdf5_file.attrs['mesh_element_dimensions']=(x_element_dim, \
                                                        y_element_dim, \
                                                        z_element_dim)
        
        # Add a root level attribute for the mesh dimensions
        hdf5_file.attrs.create('mesh_dimensions', \
                                   (x_mesh_dim, y_mesh_dim, z_mesh_dim), \
                                   dtype = np.uint32)
        
        # Add a root level attribute for the seed position
        hdf5_file.attrs.create('seed_position', \
                                   (seed_x_pos, seed_y_pos, seed_z_pos), \
                                   dtype = np.uint32)

        
    # Read in the margin mask
    dose_distribution = np.zeros((z_mesh_dim, y_mesh_dim, x_mesh_dim), np.float64)
    
    for k in range(z_mesh_dim):
        if k > 0:
            line = seed_file.readline()
        for j in range(y_mesh_dim):
            line = seed_file.readline()
            words = line.split()
            for i in range(x_mesh_dim):
                dose_distribution[k,j,i] = float(words[i])*100/1.270
                
    # Store the dose distributions
    dose_dset = hdf5_file.create_dataset(user_args.t[s], \
                                     (z_mesh_dim, y_mesh_dim, x_mesh_dim), \
                                     dtype = np.float64)
    dose_dset[...] = dose_distribution

# Close the hdf5 file
hdf5_file.close()
