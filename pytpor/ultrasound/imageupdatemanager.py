"""@package imageupdatemanager
The module for high-level management of ultrasound image updates
"""
#! /usr/bin/env python

from matplotlib import patches, path, pyplot, figure
from matplotlib.widgets import Lasso, Button
import numpy as np
import sys
from pytpor.ultrasound.imagesliceupdatemanager import ImageSliceUpdateManager

class ImageUpdateManager(object):
    """
    This class allows one to update the binary, 8-bit image files. This can
    be useful when personal patient data needs to be eliminated from the slices
    of an image file.
    """
    def __init__(self, \
                 ultrasound_image_file,\
                 image_x_resolution = 640,
                 image_y_resolution = 480,):
        """
        Initialize the ImageUpdateManager class.

        A string containing the ultrasound image file (with path) must be
        passed as an argument.
        """
        # Load the ultrasound image slices
        self.image_file_name = ultrasound_image_file
        self.full_image = np.fromfile(ultrasound_image_file, dtype='uint8')

        # Determine the number of slices
        self.image_x_resolution = image_x_resolution
        self.image_y_resolution = image_y_resolution
        image_area = self.image_x_resolution*self.image_y_resolution
        self.total_slices = len(self.full_image)/image_area

        # Shape the image data
        self.full_image.shape = (self.total_slices,\
                                 self.image_y_resolution,\
                                 self.image_x_resolution)

        # No slices have been updated yet
        self.slice = 0

        # Create the main figure window
        self.figure = pyplot.figure(figsize = (14,5.5))
        pyplot.subplots_adjust(left = 0.05, right = 0.95)
        figure_title = self.image_file_name+": Slice "+str(self.slice+1)+\
                       " of "+str(self.total_slices)
        self.figure.canvas.set_window_title(figure_title)

        # Create the subplots
        xmax = self.full_image.shape[2]
        ymax = self.full_image.shape[1]
        self.image_axis_handle = self.figure.add_subplot(121, \
                                                         xlim = (0,xmax), \
                                                         ylim = (0,ymax), \
                                                         autoscale_on = False)
        self.image_axis_handle.xaxis.set_visible(False)
        self.image_axis_handle.yaxis.set_visible(False)

        self.new_image_preview_axis_handle = \
            self.figure.add_subplot(122, \
                                    xlim = (0,xmax), \
                                    ylim = (0,ymax), \
                                    autoscale_on = False)
        self.new_image_preview_axis_handle.xaxis.set_visible(False)
        self.new_image_preview_axis_handle.yaxis.set_visible(False)

        # Create the slice update manager for this slice
        slice_image = self.full_image[self.slice]
        self.slice_update_manager = ImageSliceUpdateManager(self.image_axis_handle, \
                                                            self.new_image_preview_axis_handle,
                                                            slice_image[::-1])
        # Create the Undo button        
        self.undo_button_axes = pyplot.axes([0.15, 0.025, 0.1, 0.075])
        self.undo_button = Button(self.undo_button_axes, 'Undo')
        self.undo_button_id = \
            self.undo_button.on_clicked(self.undo_and_update_image)

        # Create the Next Slice button
        self.slice_button_axes = pyplot.axes([0.25, 0.025, 0.125, 0.075])
        self.slice_button = Button(self.slice_button_axes, 'Next Slice')
        self.slice_button.on_clicked(self.initialize_next_slice)

        # Show the figure
        pyplot.show()

    def initialize_next_slice(self, event):
        """
        When the desired event occurs, the next ultrasound image slice will
        be initialized.
        
        Before initializing the next slice, the current slice will be 
        processed by setting all bytes of the image slice that are covered by
        the sensitive data mask to 0 (black).
        """
        self.full_image[self.slice] = \
                        self.slice_update_manager.get_updated_image_slice()

        # Increment ultrasound slice
        self.slice += 1

        # Load the next slice if there is one
        if self.slice < self.total_slices:

            # Rename the figure window
            figure_title = self.image_file_name+": Slice "+ \
                           str(self.slice+1)+" of "+str(self.total_slices)
            self.figure.canvas.set_window_title(figure_title)
                
            # Create the slice manager for this slice
            self.slice_update_manager.clear_rectangle_data()
            del self.slice_update_manager
            
            slice_image = self.full_image[self.slice]
            self.slice_update_manager = \
                    ImageSliceUpdateManager(self.image_axis_handle,
                                            self.new_image_preview_axis_handle,
                                            slice_image[::-1])
                
            pyplot.show()
                
        # Close the plot window
        else:
            self.undo_button.disconnect(self.undo_button_id)
            pyplot.close()

    def undo_and_update_image(self, event):
        """
        Undo the last rectangle and update the image
        """
        self.slice_update_manager.undo_rectangle(event)
        self.figure.canvas.draw()

    def get_updated_image_data(self):
        """
        Return the updated image data.
        
        The updated image data will only be returned if all slices have been 
        processed.
        """
        if self.slice == self.total_slices:
            return self.full_image

# Test the ImageUpdateManager:
if __name__ == '__main__':
    import argparse as ap

    # Set up the argument parser
    description = "Testing script for the ImageUpdateManager class."
    parser = ap.ArgumentParser(description=description)

    image_file = "binary ultrasound image file (with path)"
    parser.add_argument('image_file', help=image_file)

    # Parse the user's arguments
    user_args = parser.parse_args()

    # Create the ImageUpdateManager
    ultrasound_image_update_manager = ImageUpdateManager( user_args.image_file )

    # Retrieve the updated image
    updated_image = ultrasound_image_update_manager.get_updated_image_data(self)
    
    image_update_success = True

    # Test that the output data has been filled by the manager
    slices = ultrasound_image_update_manager.total_slices
    try:
        if updated_image.shape != (slices,480,640):
            image_update_success = False
            print "The updated image returned is not valid\n."

        test_success = image_updated_success

    except AttributeError:
        sys.exit("Tests failed")
    else:
        if not test_success:
            sys.exit("Tests failed")
        else:
            print "Tests successful.\n"

