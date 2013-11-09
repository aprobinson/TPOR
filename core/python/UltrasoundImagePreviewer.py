#! /usr/bin/env python
from matplotlib import pyplot, figure
import numpy as np
import math
import sys

class UltrasoundImagePreviewer(object):
    """
    This class allows one to preview all ultrasound slices at once.
    """
    def __init__(self, \
                 ultrasound_image_file, \
                 image_x_resolution = 640, \
                 image_y_resolution = 480):
        """
        Initialize the UltrasoundImagePreviewer class.

        A string containing the ultrasound image file (with path) must be 
        passed as an argument.
        """
        # Load the ultrasound image slices
        self.image_file_name = ultrasound_image_file
        self.full_image = np.fromfile(ultrasound_image_file, dtype = 'uint8')

        # Determine the number of slices
        self.image_x_resolution = image_x_resolution
        self.image_y_resolution = image_y_resolution
        image_area = self.image_x_resolution*self.image_y_resolution
        self.total_slices = len(self.full_image)/image_area

        # Shape the image data
        self.full_image.shape = (self.total_slices,\
                                 self.image_y_resolution,\
                                 self.image_x_resolution)

        # Create the main figure window
        self.figure = pyplot.figure(figsize = (14,8))
        self.figure.canvas.set_window_title(self.image_file_name)
        pyplot.subplots_adjust(left = 0.01,   \
                               right = 0.99,  \
                               bottom = 0.01, \
                               top = 0.99,    \
                               wspace = 0.05, \
                               hspace = 0.01)

        # Create a subplot for each slice
        self.image_axis_handles = []
        xmax = self.full_image.shape[2]
        ymax = self.full_image.shape[1]
        num_rows = self.total_slices/5
        num_rows += math.ceil((self.total_slices-5*num_rows)/5.0)
        
        for slice in range(self.total_slices):
            subplot = self.figure.add_subplot(num_rows, 5, slice+1, \
                                              xlim = (0,xmax), \
                                              ylim = (0,ymax), \
                                              autoscale_on = False)
            subplot.xaxis.set_visible(False)
            subplot.yaxis.set_visible(False)

            subplot.set_title( "Slice " + str(slice+1) )

            image_slice = self.full_image[slice]
            subplot.imshow(image_slice[::-1], pyplot.get_cmap('gray'))

            self.image_axis_handles.append(subplot)

        # Show the figure
        pyplot.show()

# Test the UltrasoundImagePreviewer
if __name__ == '__main__':
    import argparse as ap

    # Set up the argument parser
    description = "Testing script for the UltrasoundImagePreviewer class"

    parser = ap.ArgumentParser(description=description)

    image_file = "binary ultrasound image file (with path)"
    parser.add_argument('image_file', help=image_file)

    # Parse the user's arguments
    user_args = parser.parse_args()

    # Create the UltrasoundImagePreviewer
    ultrasound_image_previewer = \
        UltrasoundImagePreviewer( user_args.image_file )

    print "Tests successful.\n"

        
