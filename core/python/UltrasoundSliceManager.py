#! /usr/bin/env python
from matplotlib import patches, path, pyplot
from matplotlib.widgets import Lasso, Button
import numpy as np

class UltrasoundSliceManager(object):
    """
    This class allows one to select the prostate, urethra, margin and rectum
    contours on an ultrasound image. 

    After a contour is selected a mask for the contour (and corresponding 
    organ) is created. All of the organ masks are stored for later used. 
    Contours can be redone by connecting a button to the undo_contour member
    function. Once all of the contours have been created, the slice image locks
    so that no more contours can be created. Calling the undo_contour member
    function will release the slice image lock so that a new contour can be 
    created. The save_mask member function should be called after all contours
    have been selected to save the organ masks for later processing.
    """
    def __init__(self, image_axis_handle, mask_axis_handle, slice_image_data ):
        """
        Initialize the UltrasoundSliceManager class.

        Two matplotlib.axes objects and an array containing the slice image 
        data must be passed as arguments. The first matplotlib.axes object must
        corresponding to the image figure and the second matplotlib.axes object
        mask figure and the mask figure. The array of slice image data must 
        have dimensions of 480x640 (480 columns x 640 rows).
        """
        # Store the two axis handles
        self.image_axis_handle = image_axis_handle
        self.mask_axis_handle = mask_axis_handle

        # Store the slice image data
        self.image = slice_image_data

        # Create the organ contour list
        self.organ_contours = []

        # Create the organ mask array (4x480x640)
        self.organ_masks =np.zeros((4,self.image.shape[0],self.image.shape[1]))

        # No organs have been contoured yet
        self.organ_index = 0

        # Bind the slice image to the button press event and the 
        # on_slice_image_press function
        self.canvas = self.image_axis_handle.figure.canvas
        self.cid = self.canvas.mpl_connect('button_press_event', \
                                           self.on_slice_image_press)

        # Initialize the pixels array so that masks are created correctly
        self.pixels = []
        for j in range(self.image.shape[0]):
            for i in range(self.image.shape[1]):
                self.pixels.append([i,j])

        # All images will be displayed in gray scale
        self.cmap = pyplot.get_cmap('gray')

        # Show the slice image and an empty mask
        self.show_images()

    def show_images(self):
        """
        Show the slice image and most recent organ mask with appropriate 
        titles.

        The title of the slice image will be a command to contour a specific
        organ. Once all organs have been contoured the command will be to
        save the organ slices.
        """
        # No organs have been contoured
        if self.organ_index == 0:
            self.image_axis_handle.set_title( "Select Prostate Contour" )
            self.mask_axis_handle.set_title( "" )
            self.clear_mask(self.organ_masks[0])
        
        # The prostate has been contoured
        elif self.organ_index == 1:
            self.image_axis_handle.set_title( "Select Urethra Contour" )
            self.mask_axis_handle.set_title( "Prostate Mask" )
            
        # The prostate and urethra have been contoured
        elif self.organ_index == 2:
            self.image_axis_handle.set_title( "Select Margin Contour" )
            self.mask_axis_handle.set_title( "Urethra Mask" )
            
        # The prostate, urethra and margin have been contoured
        elif self.organ_index == 3:
            self.image_axis_handle.set_title( "Select Rectum Contour" )
            self.mask_axis_handle.set_title( "Margin Mask" )

        # All organs have been contoured
        else:
            self.image_axis_handle.set_title( "Contouring Complete!" )
            self.mask_axis_handle.set_title( "Rectum Mask" )

        # Show the slice image and the most recent organ mask
        self.image_axis_handle.imshow(self.image, self.cmap)
        
        if self.organ_index == 0:
            self.mask_axis_handle.imshow(self.organ_masks[0], self.cmap)
        else:
            self.mask_axis_handle.imshow(self.organ_masks[self.organ_index-1],
                                         self.cmap)

    def clear_mask(self, mask):
        """
        Clear the desired mask.

        The mask must be an array with dimensions 480x640 (columns x rows).
        """
        for j in range(self.image.shape[0]):
            for i in range(self.image.shape[1]):
                mask[j][i] = False

    def get_contours(self):
        """
        Return the organ contours that have been created.
        
        The organ contours will only be returned if they have all been created.
        """
        if self.organ_index == 4:
            return self.organ_contours

    def get_prostate_mask(self):
        """
        Return the prostate mask that has been created.
        
        The prostate mask will only be returned if all masks have been created
        """
        if self.organ_index == 4:
            return self.organ_masks[0]
        
    def get_urethra_mask(self):
        """
        Return the urethra mask that has been created.
        
        The urethra mask will only be returned if all masks have been created
        """
        if self.organ_index == 4:
            return self.organ_masks[1]

    def get_margin_mask(self):
        """
        Return the margin mask that has been created.

        The margin mask will only be returned if all masks have been created
        """
        if self.organ_index == 4:
            return self.organ_masks[2]

    def get_rectum_mask(self):
        """
        Return the rectum mask that has been created:
        
        The rectum mask will only be created if all masks have been created
        """
        if self.organ_index == 4:
            return self.organ_masks[3]

    def undo_contour(self, event):
        """
        When the desired event occurs, the most recent organ contour and mask
        are deleted.

        This function is meant to be bound to a matplotlib.widgets.Button
        object. When the on_clicked event occurs with the button, this function
        should be called. When properly bound to a button (named "Undo" for
        instance) this function allows a user to redo a contour.
        """
        if self.organ_index > 0:
            # Decrement the organ index so that the plot windows can be reset
            self.organ_index -= 1
            
            # Delete the most recent organ mask that was created
            self.clear_mask(self.organ_masks[self.organ_index])

            # Delete the most recent organ contour
            self.organ_contours.pop()
            self.image_axis_handle.patches.pop()

            # Replot the images
            self.show_images()
        
    def on_slice_image_press(self, event):
        """
        When the slice image is pressed, initiate a lasso event to contour
        the image.

        After the user releases the lasso the after_slice_image_press function
        is called to process the lasso that was created. Note that the lasso
        does not need to be closed. It will be closed automatically (by adding
        a copy of the initial point to the end of the lasso point list). If
        all of the organs have been contoured, a lasso event will not be
        initialized.
        """
        if self.organ_index < 4:
            self.lasso = Lasso(event.inaxes, 
                               (event.xdata, event.ydata),
                               self.after_slice_image_press)

    def after_slice_image_press(self, lasso_vertices):
        """
        After the slice image has been pressed, the lasso that was created will
        be processed.

        The lasso is converted to an organ contour and from the organ contour
        a mask is created. Once the lasso has been processed it is deleted.
        Note that during the lasso processing, the slice image will be locked.
        Only after processing has been successfully completed will the image
        be unlocked.
        """
        # Store this organ contour
        contour = path.Path(lasso_vertices)
        self.organ_contours.append( contour )

        # Add this organ contour to the slice image
        organ_path_patch = \
            patches.PathPatch(self.organ_contours[self.organ_index], 
                              facecolor = 'none', 
                              ec = 'yellow', 
                              lw = 2, 
                              zorder = 50)
        
        self.image_axis_handle.add_patch(organ_path_patch)
        
        # Create the organ mask
        mask_shape = self.organ_masks[self.organ_index].shape
        self.organ_masks[self.organ_index] = \
            contour.contains_points(self.pixels).reshape(mask_shape)

        # Another organ has be contoured, increment the organ index
        self.organ_index += 1

        # Replot the images
        self.show_images()

        # Postprocess the lasso object
        self.canvas.draw_idle()
        self.canvas.widgetlock.release(self.lasso)
        del self.lasso

        
# Test the UltrasoundSliceManager class with a single slice
if __name__ == '__main__':
    import sys
    import argparse as ap

    # Set up the argument parser
    description = "Testing script for the UltrasoundSliceManager class. "
    "Close the figure window when contouring is completed to finish the tests."
    
    parser = ap.ArgumentParser(description=description)

    image_file = "binary ultrasound image file (with path)"
    parser.add_argument('image_file', help=image_file)

    slice_number ="slice of the ultrasound image that will be processed (1-12)"
    valid_slices = list(range(1,13))
    parser.add_argument('--slice-number', 
                        type = int, 
                        choices = valid_slices,
                        help = slice_number, 
                        default = 1)
    
    # Parse the users arguments
    user_args = parser.parse_args()
    
    # Load the ultrasound image slices
    ultrasound_slices = np.fromfile(user_args.image_file, dtype = 'uint8')
    
    # Select the desired slice
    slice_start_index = 640*480*(user_args.slice_number-1)
    slice_end_index = 640*480*user_args.slice_number
    slice_image = ultrasound_slices[slice_start_index:slice_end_index]
    slice_image.shape = (480, 640)
    
    # Create the main figure window
    main_figure = pyplot.figure()
    figure_title = user_args.image_file+": Slice "+str(user_args.slice_number)
    main_figure.canvas.set_window_title(figure_title)

    # Create the slice image subplot
    image_axis_handle = main_figure.add_subplot(121,  \
                                                xlim = (0,640), \
                                                ylim = (0,480), \
                                                autoscale_on = False)

    # Create the mask subplot
    mask_axis_handle = main_figure.add_subplot(122,  \
                                               xlim = (0,640), \
                                               ylim = (0,480), \
                                               autoscale_on = False)

    # Create the slice manager
    slice_manager = UltrasoundSliceManager(image_axis_handle, \
                                           mask_axis_handle,  \
                                           slice_image[::-1])

    # Create an Undo button
    undo_button_axes = pyplot.axes([0.2, 0.05, 0.1, 0.075])
    undo_button = Button(undo_button_axes, 'Undo')
    undo_button.on_clicked(slice_manager.undo_contour)

    # Show the figure
    pyplot.show()

    # Retrieve the contours and the masks
    organ_contours = slice_manager.get_contours()
    prostate_mask = slice_manager.get_prostate_mask()
    urethra_mask = slice_manager.get_urethra_mask()
    margin_mask = slice_manager.get_margin_mask()
    rectum_mask = slice_manager.get_rectum_mask()

    organ_contours_success = True
    prostate_mask_success = True
    urethra_mask_success = True
    margin_mask_success = True
    rectum_mask_success = True

    # Test that the output arrays have been filled by the manager
    if len(organ_contours) != 4:
        organ_contours_success = False
        print "The number of organ contours retrieved is not correct\n."
        
    if prostate_mask.shape != (480,640):
        prostate_mask_success = False
        print "The prostate mask returned is not valid\n."

    if urethra_mask.shape != (480,640):
        urethra_mask_success = False
        print "The urethra mask returned is not valid\n."

    if margin_mask.shape != (480,640):
        margin_mask_success = False
        print "The margin mask returned is not valid\n."

    if rectum_mask.shape != (480,640):
        rectum_mask_success = Fals
        print "The rectum mask returned is not valid\n."

    test_success = organ_contours_success and prostate_mask_success and \
    urethra_mask_success and margin_mask_success and rectum_mask_success

    if not test_success:
        sys.exit("Tests failed")
    else:
        print "Tests successful.\n"
