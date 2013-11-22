#! /usr/bin/env python
from matplotlib import patches, path, pyplot, figure
from matplotlib.widgets import Lasso, Button
import numpy as np
import sys
from pytpor.ultrasound.imageslicemanager import ImageSliceManager

class ImageManager(object):
    """
    This class allows one to create digital representations of organs by
    contouring a series of ultrasound slices.
    """
    def __init__(self, \
                 ultrasound_image_file,\
                 image_x_resolution = 640,\
                 image_y_resolution = 480):
        """
        Initialize the ImageManager class.

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
        
        # Create the organ countours list
        self.previous_slice_organ_contours = []

        # Create the prostate masks array
        self.prostate_masks = np.zeros(self.full_image.shape, np.bool)

        # Create the urethra masks array
        self.urethra_masks = np.zeros(self.full_image.shape, np.bool)

        # Create the margin masks array
        self.margin_masks = np.zeros(self.full_image.shape, np.bool)

        # Create the rectum masks array
        self.rectum_masks = np.zeros(self.full_image.shape, np.bool)

        # No slices have been contoured yet
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
        
        self.mask_axis_handle = self.figure.add_subplot(122, \
                                                        xlim = (0,xmax), \
                                                        ylim = (0,ymax), \
                                                        autoscale_on = False)
        self.mask_axis_handle.xaxis.set_visible(False)
        self.mask_axis_handle.yaxis.set_visible(False)

        # Create the slice manager for this slice
        slice_image = self.full_image[self.slice]
        self.slice_manager = ImageSliceManager(self.image_axis_handle, \
                                               self.mask_axis_handle,  \
                                               slice_image[::-1])

        # Create the Undo button
        self.undo_button_axes = pyplot.axes([0.15, 0.025, 0.1, 0.075])
        self.undo_button = Button(self.undo_button_axes, 'Undo')
        self.undo_button_id = \
            self.undo_button.on_clicked(self.slice_manager.undo_contour)
        
        # Create the Next Slice button
        self.slice_button_axes = pyplot.axes([0.25, 0.025, 0.125, 0.075])
        self.slice_button = Button(self.slice_button_axes, 'Next Slice')
        self.slice_button.on_clicked(self.initialize_next_slice)

        # Create the Exit button
        self.exit_button_axes = pyplot.axes([0.7, 0.025, 0.1, 0.075])
        self.exit_button = Button(self.exit_button_axes, 'Exit')
        self.exit_button.on_clicked(self.force_exit)

        # Show the figure
        pyplot.show()

    def initialize_next_slice(self, event):
        """
        When the desired event occurs, the next ultrasound image slice will
        be initialized.

        This function only executes when the ImageSliceManager instance 
        has all of the desired contour and mask data.
        """
        if self.slice_manager.organ_index == 4:
            
            # Get the contours from the slice manager
            self.previous_slice_organ_contours = \
                self.slice_manager.get_contours()
            
            # Get the organ masks from the slice manager
            self.prostate_masks[self.slice] = \
                self.slice_manager.get_prostate_mask()
            self.urethra_masks[self.slice] = \
                self.slice_manager.get_urethra_mask()
            self.margin_masks[self.slice] = \
                self.slice_manager.get_margin_mask()
            self.rectum_masks[self.slice] = \
                self.slice_manager.get_rectum_mask()

            # Increment ultrasound slice
            self.slice += 1

            # Load the next slice if there is one
            if self.slice < self.total_slices:

                # Rename the figure window
                figure_title = self.image_file_name+": Slice "+ \
                    str(self.slice+1)+" of "+str(self.total_slices)
                self.figure.canvas.set_window_title(figure_title)
                
                # Remove all contours from the slice image
                for i in range(len(self.image_axis_handle.patches)):
                    self.image_axis_handle.patches.pop()
                
                # Add all contours back in a different color
                for patch in self.previous_slice_organ_contours:
                    organ_path_patch = patches.PathPatch(patch, 
                                                         facecolor = 'none',
                                                         ec = 'white',
                                                         lw = 1,
                                                         linestyle = 'dashed',
                                                         zorder = 50)
                    self.image_axis_handle.add_patch(organ_path_patch)
                    
                # Disconnect all buttons from the current slice image
                self.slice_manager.canvas.mpl_disconnect(self.slice_manager.cid)
                self.undo_button.disconnect(self.undo_button_id)

                # Create the slice manager for this slice
                del self.slice_manager
                slice_image = self.full_image[self.slice]
                self.slice_manager = ImageSliceManager(self.image_axis_handle,\
                                                       self.mask_axis_handle, \
                                                       slice_image[::-1])
                
                # Rebind the Undo button
                self.undo_button_id = \
                   self.undo_button.on_clicked(self.slice_manager.undo_contour)
                
                pyplot.show()
                
            # Close the plot window
            else:
                self.slice_manager.canvas.mpl_disconnect(self.slice_manager.cid)
                self.undo_button.disconnect(self.undo_button_id)
                pyplot.close()

    def force_exit(self, event):
        """
        When the desired event occurs, the ImageManager class 
        instance will close the open window and exit.
        """
        sys.exit()

    def get_image_data(self):
        """
        Return the image data that was used.

        The image data will only be returned if all slices have been processed.
        """
        if self.slice == self.total_slices:
            return self.full_image
        
        
    def get_prostate_masks(self):
        """
        Return the prostate masks that were created.

        The prostate masks will only be returned if all slices have been 
        processed.
        """
        if self.slice == self.total_slices:
            return self.prostate_masks

    def get_urethra_masks(self):
        """
        Return the urethra masks that were created.
        
        The urethra masks will only be returned if all slices have been
        processed.
        """
        if self.slice == self.total_slices:
            return self.urethra_masks

    def get_margin_masks(self):
        """
        Return the margin masks that were created.

        The margin mask will only be returned if all slices have been 
        processed.
        """
        if self.slice == self.total_slices:
            return self.margin_masks

    def get_rectum_masks(self):
        """
        Return the rectum masks that were created.

        The rectum mask will only be returned if all slices have been
        processed.
        """
        if self.slice == self.total_slices:
            return self.rectum_masks

# Test the ImageManager
if __name__ == '__main__':
    import argparse as ap

    # Set up the argument parser
    description = "Testing script for the ImageManger class. "
    
    parser = ap.ArgumentParser(description=description)

    image_file = "binary ultrasound image file (with path)"
    parser.add_argument('image_file', help=image_file)

    # Parse the user's arguments
    user_args = parser.parse_args()
    
    # Create the ImageManager
    ultrasound_image_manager = ImageManager( user_args.image_file )

    # Retrieve the masks
    prostate_masks = ultrasound_image_manager.get_prostate_masks()
    urethra_masks = ultrasound_image_manager.get_urethra_masks()
    margin_masks = ultrasound_image_manager.get_margin_masks()
    rectum_masks = ultrasound_image_manager.get_rectum_masks()

    prostate_masks_success = True
    urethra_masks_success = True
    margin_masks_success = True
    rectum_masks_success = True

    # Test that the output arrays have been filled by the manager
    slices = ultrasound_image_manager.total_slices
    try:
        if prostate_masks.shape != (slices,480,640):
            prostate_masks_success = False
            print "The prostate masks returned are not valid\n."
            
        if urethra_masks.shape != (slices,480,640):
            urethra_masks_success = False
            print "The urethra masks returned are not valid\n."
                
        if margin_masks.shape != (slices,480,640):
            margin_masks_success = False
            print "The margin masks returned are not valid\n."
            
        if rectum_masks.shape != (slices,480,640):
            rectum_masks_success = False
            print "The rectum masks returned are not valid\n."

        test_success = prostate_masks_success and urethra_masks_success and \
            margin_masks_success and rectum_masks_success

    except AttributeError:
        sys.exit("Tests failed")
    else:
        if not test_success:
            sys.exit("Tests failed")
        else:
            print "Tests successful.\n"
