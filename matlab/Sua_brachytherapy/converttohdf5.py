#! /usr/bin/env python

import h5py
import numpy as np

# Open up the mask files
organ_mask_file = open('organ_mesh-SUA', 'r')

# Open the HDF5 file (overwrite if it exists)
hdf5_file = h5py.File("Sua_test_patient.h5", 'w')

# Add a root level attribute for the patient name
patient_name = "Sua test patient"
patient_name_array = np.frombuffer(patient_name, dtype=np.int8)
hdf5_file.attrs['patient_name'] = patient_name_array

# Mask dimensions
x_mesh_dim = 0
y_mesh_dim = 0
z_mesh_dim = 0

# Mesh element dimensions
x_element_dim = 0.0
y_element_dim = 0.0
z_element_dim = 0.0

# Parse the organ mask file header
line = organ_mask_file.readline()
words = line.split()
x_mesh_dim = int(words[1])

line = organ_mask_file.readline()
words = line.split()
x_element_dim = float(words[1])

line = organ_mask_file.readline()
words = line.split()
y_mesh_dim = int(words[1])

line = organ_mask_file.readline()
words = line.split()
y_element_dim = float(words[1])

line = organ_mask_file.readline()
words = line.split()
z_mesh_dim = int(words[1])

line = organ_mask_file.readline()
words = line.split()
z_element_dim = float(words[1])

# Add a root level attribute for the mesh element dimensions
hdf5_file.attrs['mesh_element_dimensions']=(x_element_dim, \
                                            y_element_dim, \
                                            z_element_dim)

# Add a root level attribute for the mesh dimensions
hdf5_file.attrs.create('mesh_dimensions', \
                       (x_mesh_dim, y_mesh_dim, z_mesh_dim), \
                       dtype = np.uint32)

# Create a group for the processed organ masks
organ_mask_group = hdf5_file.create_group("organ_masks")

# Create a dataset in the organ mask group for each organ mask
mask_shape = (z_mesh_dim, y_mesh_dim, x_mesh_dim)
prostate_dset = organ_mask_group.create_dataset('prostate_mask', \
                                                mask_shape, \
                                                dtype = np.int8)
urethra_dset = organ_mask_group.create_dataset('urethra_mask', \
                                               mask_shape, \
                                               dtype = np.int8)
margin_dset = organ_mask_group.create_dataset('margin_mask', \
                                              mask_shape, \
                                              dtype = np.int8)
rectum_dset = organ_mask_group.create_dataset('rectum_mask', \
                                              mask_shape, \
                                              dtype = np.int8)

# Read in the prostate mask
prostate_relative_vol = 0
prostate_volume = 0.0
prostate_mask = np.zeros(mask_shape, np.int8)

line = organ_mask_file.readline()
words = line.split()
prostate_relative_vol = int(words[1])
prostate_volume = \
    prostate_relative_vol*x_element_dim*y_element_dim*z_element_dim

for k in range(z_mesh_dim):
    if k > 0:
        line = organ_mask_file.readline()
    for j in range(y_mesh_dim):
        line = organ_mask_file.readline()
        words = line.split()
        for i in range(x_mesh_dim):
            prostate_mask[k,j,i] = int(words[i])

if prostate_mask.sum() != prostate_relative_vol:
    print "Error reading prostate mask: "
    print "Expected prostate mask relative vol: ", prostate_mask.sum(), 
    print "Actual prostate mask relative vol: ", prostate_relative_vol
# Store the prostate mask data
else:
    prostate_dset[...] = prostate_mask
    prostate_dset.attrs.create('relative_volume', \
                               prostate_relative_vol, \
                               dtype = np.uint32)
    prostate_dset.attrs['volume'] = prostate_volume

# Read in the urethra mask
urethra_relative_vol = 0
urethra_volume = 0.0
urethra_mask = np.zeros(mask_shape, np.int8)

line = organ_mask_file.readline()
line = organ_mask_file.readline()
words = line.split()
urethra_relative_vol = int(words[1])
urethra_volume = \
    urethra_relative_vol*x_element_dim*y_element_dim*z_element_dim

for k in range(z_mesh_dim):
    if k > 0:
        line = organ_mask_file.readline()
    for j in range(y_mesh_dim):
        line = organ_mask_file.readline()
        words = line.split()
        for i in range(x_mesh_dim):
            urethra_mask[k,j,i] = int(words[i])

if urethra_mask.sum() != urethra_relative_vol:
    print "Error reading urethra mask: "
    print "Expected urethra mask relative vol: ", urethra_mask.sum(), 
    print "Actual urethra mask relative vol: ", urethra_relative_vol
# Store the urethra mask data
else:
    urethra_dset[...] = urethra_mask
    urethra_dset.attrs.create('relative_volume', \
                               urethra_relative_vol, \
                               dtype = np.uint32)
    urethra_dset.attrs['volume'] = urethra_volume

# Read in the rectum mask
rectum_relative_vol = 0
rectum_volume = 0.0
rectum_mask = np.zeros(mask_shape, np.int8)

line = organ_mask_file.readline()
line = organ_mask_file.readline()
words = line.split()
rectum_relative_vol = int(words[1])
rectum_volume = rectum_relative_vol*x_element_dim*y_element_dim*z_element_dim

for k in range(z_mesh_dim):
    if k > 0:
        line = organ_mask_file.readline()
    for j in range(y_mesh_dim):
        line = organ_mask_file.readline()
        words = line.split()
        for i in range(x_mesh_dim):
            rectum_mask[k,j,i] = int(words[i])

if rectum_mask.sum() != rectum_relative_vol:
    print "Error reading rectum mask: "
    print "Expected rectum mask relative vol: ", rectum_mask.sum(), 
    print "Actual rectum mask relative vol: ", rectum_relative_vol
# Store the rectum mask data
else:
    rectum_dset[...] = rectum_mask
    rectum_dset.attrs.create('relative_volume', \
                               rectum_relative_vol, \
                               dtype = np.uint32)
    rectum_dset.attrs['volume'] = rectum_volume

# Skip the normal mask
line = organ_mask_file.readline()
line = organ_mask_file.readline()

for k in range(z_mesh_dim):
    if k > 0:
        line = organ_mask_file.readline()
    for j in range(y_mesh_dim):
        line = organ_mask_file.readline()

# Read in the margin mask
margin_relative_vol = 0
margin_volume = 0.0
margin_mask = np.zeros(mask_shape, np.int8)

line = organ_mask_file.readline()
line = organ_mask_file.readline()
words = line.split()
margin_relative_vol = int(words[1])
margin_volume = margin_relative_vol*x_element_dim*y_element_dim*z_element_dim

for k in range(z_mesh_dim):
    if k > 0:
        line = organ_mask_file.readline()
    for j in range(y_mesh_dim):
        line = organ_mask_file.readline()
        words = line.split()
        for i in range(x_mesh_dim):
            margin_mask[k,j,i] = int(words[i])

if margin_mask.sum() != margin_relative_vol:
    print "Error reading margin mask: "
    print "Expected margin mask relative vol: ", margin_mask.sum(), 
    print "Actual margin mask relative vol: ", margin_relative_vol
# Store the margin mask data
else:
    margin_dset[...] = margin_mask
    margin_dset.attrs.create('relative_volume', \
                               margin_relative_vol, \
                               dtype = np.uint32)
    margin_dset.attrs['volume'] = margin_volume

# Create Sua's needle template
needle_template_shape = (y_mesh_dim, x_mesh_dim)
needle_template = np.zeros(needle_template_shape, np.int8)

for j in range(4, y_mesh_dim, 5):
    for i in range(4, x_mesh_dim, 5):
        needle_template[j,i] = 1

# Create a dataset for the needle template
needle_template_dset = hdf5_file.create_dataset('needle_template', \
                                                needle_template.shape, \
                                                dtype = np.int8)
needle_template_dset[...] = needle_template

##---------------------------------------------------------------------------##
# # Open up the adjoint file
# adjoint_file = open('adjoint_data-SUA', 'r')

# line = adjoint_file.readline()
# line = adjoint_file.readline()
# line = adjoint_file.readline()
# line = adjoint_file.readline()
# line = adjoint_file.readline()
# line = adjoint_file.readline()

# # Create the adjoint data group
# adjoint_group = hdf5_file.create_group("adjoint_data")
# amersham6711seed_adjoint_group = adjoint_group.create_group("Amersham6711Seed")

# # Read in the prostate adjoint
# prostate_adjoint = np.zeros(mask_shape, np.float64)

# line = adjoint_file.readline()

# for k in range(z_mesh_dim):
#     if k > 0:
#         line = adjoint_file.readline()
#     for j in range(y_mesh_dim):
#         line = adjoint_file.readline()
#         words = line.split()
#         for i in range(x_mesh_dim):
#             prostate_adjoint[k,j,i] = float(words[i])

# # Store the prostate adjoint data
# prostate_adjoint_dset = amersham6711seed_adjoint_group.create_dataset( \
#     'prostate_adjoint_data', \
#     prostate_adjoint.shape, \
#     dtype = np.float64)
# prostate_adjoint_dset[...] = prostate_adjoint

# # Read in the urethra adjoint
# urethra_adjoint = np.zeros(mask_shape, np.float64)

# line = adjoint_file.readline()
# line = adjoint_file.readline()

# for k in range(z_mesh_dim):
#     if k > 0:
#         line = adjoint_file.readline()
#     for j in range(y_mesh_dim):
#         line = adjoint_file.readline()
#         words = line.split()
#         for i in range(x_mesh_dim):
#             urethra_adjoint[k,j,i] = float(words[i])

# # Store the urethra adjoint data
# urethra_adjoint_dset = amersham6711seed_adjoint_group.create_dataset( \
#     'urethra_adjoint_data', \
#     urethra_adjoint.shape, \
#     dtype = np.float64)
# urethra_adjoint_dset[...] = urethra_adjoint

# # Read in the rectum adjoint
# rectum_adjoint = np.zeros(mask_shape, np.float64)

# line = adjoint_file.readline()
# line = adjoint_file.readline()

# for k in range(z_mesh_dim):
#     if k > 0:
#         line = adjoint_file.readline()
#     for j in range(y_mesh_dim):
#         line = adjoint_file.readline()
#         words = line.split()
#         for i in range(x_mesh_dim):
#             rectum_adjoint[k,j,i] = float(words[i])

# # Store the rectum adjoint data
# rectum_adjoint_dset = amersham6711seed_adjoint_group.create_dataset( \
#     'rectum_adjoint_data', \
#     rectum_adjoint.shape, \
#     dtype = np.float64)
# rectum_adjoint_dset[...] = rectum_adjoint

# # Skip the normal adjoint
# line = adjoint_file.readline()
# line = adjoint_file.readline()

# for k in range(z_mesh_dim):
#     if k > 0:
#         line = adjoint_file.readline()
#     for j in range(y_mesh_dim):
#         line = adjoint_file.readline()

# # Read in the margin adjoint
# margin_adjoint = np.zeros(mask_shape, np.float64)

# line = adjoint_file.readline()
# line = adjoint_file.readline()

# for k in range(z_mesh_dim):
#     if k > 0:
#         line = adjoint_file.readline()
#     for j in range(y_mesh_dim):
#         line = adjoint_file.readline()
#         words = line.split()
#         for i in range(x_mesh_dim):
#             margin_adjoint[k,j,i] = float(words[i])

# # Store the margin adjoint data
# margin_adjoint_dset = amersham6711seed_adjoint_group.create_dataset( \
#     'margin_adjoint_data', \
#     margin_adjoint.shape, \
#     dtype = np.float64)
# margin_adjoint_dset[...] = margin_adjoint

hdf5_file.close()

##---------------------------------------------------------------------------##
# Open up the Amersham6711Seed file
seed_file = open('I6711-SUA', 'r')

# Open the HDF5 file (overwrite if it exists)
hdf5_file = h5py.File("BrachytherapySeeds.h5", 'w')

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
            dose_distribution[k,j,i] = float(words[i])*100

# Store the dose distributions
dose_dset = hdf5_file.create_dataset('Amersham6711Seed', \
                                     (z_mesh_dim, y_mesh_dim, x_mesh_dim), \
                                     dtype = np.float64)
dose_dset[...] = dose_distribution
