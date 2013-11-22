#! /usr/bin/env python
import numpy as np
import argparse as ap

# Set up the argument parser
description = "Script for isolating slices from ultrasound image files"
parser = ap.ArgumentParser(description=description)

image_file = "binary ultrasound image file (with path)"
parser.add_argument('image_file', help=image_file)

output_file = "processed ultrasound image file name (with path)"
parser.add_argument('output_file', help=output_file)

# Parse the users arguments
user_args = parser.parse_args()

# Load the desired ultrasound image file
full_image = np.fromfile(user_args.image_file, dtype = 'uint8')

# Determine the number of slices
total_slices = len(full_image)/(480*640)

# output the middle two slices
start_slice = total_slices/2
end_slice = start_slice + 1
start_element = start_slice*480*640
end_element = (end_slice+1)*480*640

partial_image = full_image[start_element:end_element]
partial_image.tofile(user_args.output_file)
