"""@package imagesliceupdatemanager
The module for lower-level management of ultrasound image updates.
"""
#! /usr/bin/env python
from matplotlib import patches, path, pyplot
from matplotlib.widgets import RectangleSelector, Button
import numpy as np

class ImageSliceUpdateManager(object):
    """
    This class allows one to select regions of sensitive data in an ultrasound
    image.
    
    After a sensitive region is selected a mask of the region is created and
    a preview of the updated image will be shown. Sensitive regions can be
    redone by connecting a button to the undo_sensitive_region member function.
    Once all of the sensitive regions have been created, the slice image
    locks so that no more sensitive regions can be created. Calling the
    undo_sensitive_region member function will release the slice image lock so
    that new sensitive_regions can be selected. The save_updated_slice member
    function should be called after all senstive regions have been selected to
    save the updated image slice for later processing.
    """
    def __init__(self, image_axis_handle, updated_image_preview_axis_handle, slice_image_data):
        """
        Initialize the ImageSliceUpdateManager class.
        """
        # Store the two axis handles
        self.image_axis_handle = image_axis_handle
        self.updated_image_preview_axis_handle = updated_image_preview_axis_handle

        # Store the slice image data
        self.image = slice_image_data

        # Bind the slice image to the button press event and the
        # on_slice_image_press function
        # Create the rectangle selector and bind it to the on_rectangle_select
        # method
        self.rectangle_selector = RectangleSelector(self.image_axis_handle,
                                                    self.on_rectangle_select )

        # All images will be displayed in gray scale
        self.cmap = pyplot.get_cmap('gray')

        # Initialize the countours and rectangle images lists
        self.contours = []
        self.rectangle_images = []

        # Show the slice image and an empty mask
        self.original_image_shown = False
        self.show_images()

    def show_images(self):
        """
        Show the slice image and the sensitive region masks with appropriate
        titles.
        """
        # No sensitive regions have been selected - set the titles
        if self.original_image_shown == False:
            self.image_axis_handle.set_title("Select Sensitive Region",
                                             color="red")
            self.updated_image_preview_axis_handle.set_title( "Updated Image Preview",
                                                          color="black" )
            
            self.image_axis_handle.imshow(self.image, self.cmap)
            self.updated_image_preview_axis_handle.imshow(self.image, self.cmap)
            self.original_image_shown = True
        
        
    def get_updated_image_slice(self):
        """
        Update the image slice using the sensitive region rectangles.
        """

        # Initialize the pixels array so that masks are created correctly
        if len(self.contours) > 0:
            pixels = []
            for j in range(self.image.shape[0]):
                for i in range(self.image.shape[1]):
                    pixels.append([i,j])

            sensitive_region_masks = []
            for sensitive_region_contour in self.contours:
                sensitive_region_masks.append(sensitive_region_contour.contains_points(pixels))

            sensitive_region_mask = sensitive_region_masks[0]

            for i in range(1,len(sensitive_region_masks)):
                for j in range(0,len(sensitive_region_mask)):
                    if sensitive_region_masks[i][j] == True:
                        sensitive_region_mask[j] = True

            updated_image = np.zeros((self.image.shape[0], self.image.shape[1]), dtype='uint8')

            for j in range(self.image.shape[0]):
                for i in range(self.image.shape[1]):
                    if sensitive_region_mask[j*self.image.shape[1]+i] == True:
                        updated_image[j][i] = 0
                    else:
                        updated_image[j][i] = self.image[j][i]

            return updated_image[::-1]
        else:
            return self.image[::-1]

    def on_rectangle_select(self, click_event, release_event):
        """
        Callback used by the RectangleSelector
        """
        # Construct the rectangle vertices
        rectangle_vertices = [(click_event.xdata, click_event.ydata),
                              (release_event.xdata, click_event.ydata),
                              (release_event.xdata, release_event.ydata),
                              (click_event.xdata, release_event.ydata)]

        # Store the sensitive region contour
        self.contours.append( path.Path(rectangle_vertices) )

        # Render the new image preview
        #for stored_path in self.contours:
        path_patch = patches.PathPatch( self.contours[-1],
                                        facecolor = 'black',
                                        ec = 'white',
                                        lw = 0.5,
                                        ls = "--",
                                        zorder = 50 )
            
        self.rectangle_images.append(self.updated_image_preview_axis_handle.add_patch( path_patch ))

        #self.show_images()
                                                    
    def undo_rectangle(self, event):
        """
        When the desired event occurs, the most recent rectangle selection is
        deleted.
        
        This function is meant to be bound to a matplotlib.widgets.Button
        object. When the on_clicked event occurs with the button, this function
        should be called. When properly bound to a button (named "Undo" for 
        instance) this function allows a user to redo a rectangle.
        """

        if len(self.contours) > 0:
            self.contours.pop()
            self.rectangle_images[-1].remove()
            self.rectangle_images.pop()
            
    def clear_rectangle_data(self):
        """
        This method clears all of the rectangle data. It must be called before
        an instance of this class is deleted.
        """

        del self.contours[:]

        for rect_image in self.rectangle_images:
            rect_image.set_zorder(0)
            rect_image.remove()

        del self.rectangle_images[:]

        
