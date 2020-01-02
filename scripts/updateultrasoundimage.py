#! /usr/bin/env python
import numpy as np
import math
import argparse as ap
import h5py
from matplotlib import pyplot, figure
import pytpor
from pytpor.ultrasound.imagemetadata import ImageMetadata
from pytpor.ultrasound.imageupdatemanager import ImageUpdateManager

# Set up the argument parser
description = "Placeholder"

parser = ap.ArgumentParser(description=description)

image_file = "binary ultrasound image file (with path)"
parser.add_argument('image_file', help=image_file)

image_header_file = "ultrasound image file header (with path)"
parser.add_argument('image_header_file', help=image_header_file)

output_file = "the output binary ultrasound image file (with path)"
parser.add_argument("--output_file", help=output_file, default="")

# Parse the user's arguments
user_args = parser.parse_args()

output_file_name = user_args.output_file

if output_file_name == "":
    output_file_name = "updated-"+user_args.image_file

# Create the ultrasound image metadata manager
image_metadata = ImageMetadata(user_args.image_header_file)

# Create the ultrasound image update manager
image_update_manager = ImageUpdateManager(user_args.image_file,
                                          image_metadata.get_image_x_resolution(),
                                          image_metadata.get_image_y_resolution())

# Save the updated ultrasound images
updated_image = image_update_manager.get_updated_image_data()

if not updated_image is None:
    updated_image.tofile(output_file_name)
else:
    print "Oops, something went wrong..."
