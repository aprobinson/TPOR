#! /usr/bin/env python
import numpy as np
import math
import argparse as ap
import h5py
from matplotlib import pyplot, figure
from UltrasoundImageMetadataManager import UltrasoundImageMetadataManager
from UltrasoundImagePreviewer import UltrasoundImagePreviewer
from UltrasoundImageManager import UltrasoundImageManager
from OrganMaskProcessor import OrganMaskProcessor

# Set up the argument parser
description = "This script allows one to process a patients ultrasound images"\
    " for use in a treatment planning algorithm. All processed data is stored"\
    " in a HDF5 file."

parser = ap.ArgumentParser(description=description)

image_file = "binary ultrasound image file (with path)"
parser.add_argument('image_file', help=image_file)

image_header_file = "ultrasound image file header (with path)"
parser.add_argument('image_header_file', help=image_header_file)

preview = "do not preview the ultrasound slices before processing them"
parser.add_argument('--no-preview', action='store_true', help=preview)

view_processed_data = "do not view the processed organ slices"
parser.add_argument('--no-view-processed-data', action='store_true', \
                    help=view_processed_data)

# Parse the user's arguments
user_args = parser.parse_args()

# Create the ultrasound image metadata manager
image_metadata = UltrasoundImageMetadataManager(user_args.image_header_file)

# Preview the ultrasound slices unless the no-preview option was requested
if not user_args.no_preview:
    preview = UltrasoundImagePreviewer(user_args.image_file)

# Create the ultrasound image manager
image_manager = UltrasoundImageManager(user_args.image_file,\
                                      image_metadata.get_image_x_resolution(),\
                                      image_metadata.get_image_y_resolution())


# Retrieve the organ masks that were created by the user
prostate_masks = image_manager.get_prostate_masks()
urethra_masks = image_manager.get_urethra_masks()
margin_masks = image_manager.get_margin_masks()
rectum_masks = image_manager.get_rectum_masks()

# Release the image_manager
del image_manager

# Create the organ mask processor
organ_mask_processor = OrganMaskProcessor( \
                                       prostate_masks, \
                                       urethra_masks, \
                                       margin_masks, \
                                       rectum_masks, \
                                       image_metadata.get_pixel_size_x(), \
                                       image_metadata.get_pixel_size_y(), \
                                       image_metadata.get_slice_offset())

# Display the processed organ masks unless the no-view-processed_data option 
# was requested
if not user_args.no_view_processed_data:
    
    # Retrieve the fov masks
    prostate_masks = organ_mask_processor.get_fov_prostate_masks()
    urethra_masks = organ_mask_processor.get_fov_urethra_masks()
    margin_masks = organ_mask_processor.get_fov_margin_masks()
    rectum_masks = organ_mask_processor.get_fov_rectum_masks()

    # Retrieve the coarsened masks
    coarse_prostate_masks = \
        organ_mask_processor.get_coarsened_prostate_masks()
    coarse_urethra_masks = \
        organ_mask_processor.get_coarsened_urethra_masks()
    coarse_margin_masks = \
        organ_mask_processor.get_coarsened_margin_masks()
    coarse_rectum_masks = \
        organ_mask_processor.get_coarsened_rectum_masks()

    # Retrieve the needle template
    needle_template = organ_mask_processor.get_needle_template()

    # Create a fov mesh for the fov masks
    x = np.arange(0.0, coarse_prostate_masks.shape[2], 0.1)
    y = np.arange(0.0, coarse_prostate_masks.shape[1], 0.1)
    X, Y = np.meshgrid(x, y)
    Z = urethra_masks + prostate_masks*2 + margin_masks*3 + \
        rectum_masks*4

    # Plot the coarsened masks for each slice
    figure = pyplot.figure(figsize = (10,8))
    pyplot.subplots_adjust(left = 0.01,   \
                           right = 0.99,  \
                           bottom = 0.01, \
                           top = 0.99,    \
                           wspace = 0.05, \
                           hspace = 0.01)
    
    image_axis_handles = []
    xmax = coarse_prostate_masks.shape[2]
    ymax = coarse_prostate_masks.shape[1]
    total_slices = coarse_prostate_masks.shape[0]
    num_rows = (total_slices+1)/5
    num_rows += math.ceil(((total_slices+1)-5*num_rows)/5.0)
    
    for slice in range(total_slices):
        subplot = figure.add_subplot(num_rows, 5, slice+1, \
                                     xlim = (0,xmax), \
                                     ylim = (0,ymax), \
                                     autoscale_on = False)
                                     
        subplot.xaxis.set_visible(False)
        subplot.yaxis.set_visible(False)

        subplot.set_title( "Slice " + str(slice+1) )

        image_slice = coarse_urethra_masks + coarse_prostate_masks*2 + \
            coarse_margin_masks*3 + coarse_rectum_masks*4
        
        subplot.imshow(image_slice[slice])
        
        # Add the raw organ countours
        pyplot.contour(X, Y, Z[slice], colors = 'black', linewidths = 1, \
                       linestyles = 'dashed')
                       
        image_axis_handles.append(subplot)

    # Add the needle template
    needle_template_plot = figure.add_subplot(num_rows, 5, total_slices+1, \
                                              xlim = (0,xmax), \
                                              ylim = (0,ymax), \
                                              autoscale_on = False)
    
    needle_template_plot.xaxis.set_visible(False)
    needle_template_plot.yaxis.set_visible(False)

    needle_template_plot.set_title( "Needle Template" )

    needle_template_plot.imshow(needle_template)
                       
    # Show the figure
    pyplot.show()

# Free the organ mask processor
del organ_mask_processor

# Store the processed data in an HDF5 file (overwrite if it exists)
hdf5_file = h5py.File(image_metadata.get_patient_name()+".h5", 'w')

# Add a root level attribute for the patient name
patient_name = image_metadata.get_patient_name()
patient_name_array = np.frombuffer(patient_name, dtype=np.int8)
hdf5_file.attrs['patient_name'] = patient_name_array

# Add a root level attribute for the mesh element dimensions [cm]
element_x_dim = 0.1
element_y_dim = 0.1
element_z_dim = image_metadata.get_slice_offset()
hdf5_file.attrs['mesh_element_dimensions']=(element_x_dim, \
                                            element_y_dim, \
                                            element_z_dim)

# Add a root level attribute for the mesh dimensions [num elements]
mesh_x_dim = coarse_prostate_masks.shape[2]
mesh_y_dim = coarse_prostate_masks.shape[1]
mesh_z_dim = coarse_prostate_masks.shape[0]
hdf5_file.attrs.create('mesh_dimensions', \
                       (mesh_x_dim, mesh_y_dim, mesh_z_dim), \
                       dtype = np.uint32)

# Create a group for the processed organ masks
organ_mask_group = hdf5_file.create_group("organ_masks")

# Create a dataset in the organ mask group for each organ mask
prostate_dset = organ_mask_group.create_dataset('prostate_mask', \
                                                coarse_prostate_masks.shape, \
                                                dtype=np.bool)
prostate_dset[...] = coarse_prostate_masks

urethra_dset = organ_mask_group.create_dataset('urethra_mask', \
                                               coarse_urethra_masks.shape, \
                                               dtype=np.bool)
urethra_dset[...] = coarse_urethra_masks

margin_dset = organ_mask_group.create_dataset('margin_mask', \
                                              coarse_margin_masks.shape, \
                                              dtype=np.bool)
margin_dset[...] = coarse_margin_masks

rectum_dset = organ_mask_group.create_dataset('rectum_mask', \
                                              coarse_rectum_masks.shape, \
                                              dtype=np.bool)
rectum_dset[...] = coarse_rectum_masks

# Create a volume attribute for each organ mask data set
prostate_mesh_elements = coarse_prostate_masks.sum()
prostate_volume = prostate_mesh_elements*0.1*0.1*\
    image_metadata.get_slice_offset()
prostate_dset.attrs.create('relative_volume', \
                           prostate_mesh_elements, \
                           dtype=np.uint32)
prostate_dset.attrs['volume'] = prostate_volume

urethra_mesh_elements = coarse_urethra_masks.sum()
urethra_volume = urethra_mesh_elements*0.1*0.1*\
    image_metadata.get_slice_offset()
urethra_dset.attrs.create('relative_volume', \
                          urethra_mesh_elements, \
                          dtype=np.uint32)
urethra_dset.attrs['volume'] = urethra_volume

margin_mesh_elements = coarse_margin_masks.sum()
margin_volume = margin_mesh_elements*0.1*0.1*\
    image_metadata.get_slice_offset()
margin_dset.attrs.create('relative_volume', \
                         margin_mesh_elements, \
                         dtype=np.uint32)
margin_dset.attrs['volume'] = margin_volume

rectum_mesh_elements = coarse_rectum_masks.sum()
rectum_volume = rectum_mesh_elements*0.1*0.1*\
    image_metadata.get_slice_offset()
rectum_dset.attrs.create('relative_volume', \
                         rectum_mesh_elements, \
                         dtype=np.uint32)
rectum_dset.attrs['volume'] = rectum_volume

# Create a dataset for the needle template
needle_template_dset = hdf5_file.create_dataset('needle_template', \
                                                needle_template.shape, \
                                                dtype=np.bool)

needle_template_dset[...] = needle_template

hdf5_file.close()
