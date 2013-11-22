#! /usr/bin/env python
import numpy as np
import math
import sys

class ArrayError(Exception):
    """
    Exception raised for errors in the mask arrays.
    """
    pass

class MaskProcessor(object):
    """
    This class allows one to process organ masks that are created from
    medical image files.
    """
    def __init__(self, \
                 raw_prostate_masks, \
                 raw_urethra_masks, \
                 raw_margin_masks, \
                 raw_rectum_masks, 
                 mesh_element_x_dim, \
                 mesh_element_y_dim, \
                 mesh_element_z_dim ):
        """
        Initialize the MaskProcessor class.
        
        An array containing the prostate masks, urethra masks, margin_masks and
        rectum masks must be passed as arguments. The dimensions of the (numpy)
        arrays must be (num_slices,y_resolution,x_resolution). All arrays must 
        also be the same size and type (bool). The mesh element dimensions are
        assumed to be in units of cm.
        """
        # Store the raw organ masks
        self.raw_prostate_masks = raw_prostate_masks
        self.raw_urethra_masks = raw_urethra_masks
        self.raw_margin_masks = raw_margin_masks
        self.raw_rectum_masks = raw_rectum_masks

        # Check that the arrays are valid
        self.__check_input_arrays__()

        # Store the mesh element dimensions
        self.mesh_element_dimensions = (mesh_element_z_dim, \
                                        mesh_element_y_dim, \
                                        mesh_element_x_dim)

        # Create an array with the organ mask mesh element boundaries
        self.raw_mask_mesh_x_boundaries = \
            np.arange(0.0, \
                      mesh_element_x_dim*(self.raw_prostate_masks.shape[2]+1),\
                      mesh_element_x_dim)

        self.raw_mask_mesh_y_boundaries = \
            np.arange(0.0, \
                      mesh_element_y_dim*(self.raw_prostate_masks.shape[1]+1),\
                      mesh_element_y_dim)

        # Save the number of slices
        self.slices = self.raw_prostate_masks.shape[0]
       
        # Clean the raw organ masks
        self.__clean_raw_masks__()

        # Refine the masks
        self.__refine_masks__()
        
        # Coarsen the cleaned, refined organ masks
        self.__coarsen_masks__()

        # Create the needle template
        self.__create_needle_template__()

    def __check_input_arrays__(self):
        """
        Check that the raw mask arrays have the correct shape and that all of
        the shapes are the same. 

        When these tests fail, an exception is thrown causing a program exit.
        """
        prostate_slices = 0
        urethra_slices = 0
        margin_slices = 0
        rectum_slices = 0       
        
        # Check that the raw prostate masks array is a numpy array
        try:
            prostate_slices = self.raw_prostate_masks.shape[0]
        except AttributeError as err:
            print "Error: raw_prostate_masks is not a numpy array."
            print err

        # Check that the raw urethra masks array is a numpy array
        try:
            urethra_slices = self.raw_prostate_masks.shape[0]
        except AttributeError as err:
            print "Error: raw_urethra_masks is not a numpy array."
            print err

        # Check that the raw margin masks array is a numpy array
        try:
            margin_slices = self.raw_margin_masks.shape[0]
        except AttributeError as err:
            print "Error: raw_margin_masks is not a numpy array."
            print err
            
        # Check that the raw rectum masks array is a numpy array
        try:
            rectum_slices = self.raw_rectum_masks.shape[0]
        except AttributeError as err:
            print "Error: raw_rectum_masks is not a numpy array."
            print err

        # Get the dimensions of each array
        prostate_dimensions = self.raw_prostate_masks.ndim
        urethra_dimensions = self.raw_urethra_masks.ndim
        margin_dimensions = self.raw_margin_masks.ndim
        rectum_dimensions = self.raw_rectum_masks.ndim

        # Test every array is 3 dimensional
        if prostate_dimensions != 3:
            print "Error: raw_prostate_masks array is not 3 dimensional."
            raise ArrayError
        if urethra_dimensions != 3:
            print "Error: raw_urethra_masks array is not 3 dimensional."
            raise ArrayError
        if margin_dimensions != 3:
            print "Error: raw_margin_masks array is not 3 dimensional."
            raise ArrayError
        if rectum_dimensions != 3:
            print "Error: raw_rectum_masks array is not 3 dimensional."
            raise ArrayError

        # Test that every array has the same number of slices
        slice_number_test = prostate_slices == urethra_slices and \
            prostate_slices == margin_slices and \
            prostate_slices == rectum_slices

        if not slice_number_test:
            print \
                "Error: the mask arrays do not have the same number of slices."
            raise ArrayError
        
        # Get the shape of each array
        prostate_array_shape = self.raw_prostate_masks.shape
        urethra_array_shape = self.raw_urethra_masks.shape
        margin_array_shape = self.raw_margin_masks.shape
        rectum_array_shape = self.raw_rectum_masks.shape

        # Test that every array has the same shape
        array_shape_test = prostate_array_shape == urethra_array_shape and \
            prostate_array_shape == margin_array_shape and \
            prostate_array_shape == rectum_array_shape

        if not array_shape_test:
            print \
                "Error: the mask arrays do not have the same shape."
            raise ArrayError

        # Get the type contained in each array
        prostate_array_type = self.raw_prostate_masks.dtype
        urethra_array_type = self.raw_urethra_masks.dtype
        margin_array_type = self.raw_margin_masks.dtype
        rectum_array_type = self.raw_rectum_masks.dtype

        # Test that every array contains booleans
        if prostate_array_type != np.bool:
            print \
               "Error: the raw_prostate_masks array does not contain booleans."
            raise ArrayError
        if urethra_array_type != np.bool:
            print \
                "Error: the raw_urethra_masks array does not contain booleans."
            raise ArrayError
        if margin_array_type != np.bool:
            print \
                "Error: the raw_margin_masks array does not contain booleans."
            raise ArrayError
        if rectum_array_type != np.bool:
            print \
                "Error: the raw_rectum_masks array does not contain booleans."
            raise ArrayError

    def __clean_raw_masks__(self):
        """
        Clean up overlapping regions of the masks so that each pixel is
        assigned to exactly one mask.
        """
        # Clean the margin and prostate masks
        for k in range(self.slices):
            for j in range(self.raw_prostate_masks.shape[1]):
                for i in range(self.raw_prostate_masks.shape[2]):
                    
                    # Clean margin mask = margin mask - prostate mask - 
                    # rectum mask
                    self.raw_margin_masks[k,j,i] = \
                        self.raw_margin_masks[k,j,i] and not \
                        self.raw_prostate_masks[k,j,i] and not \
                        self.raw_rectum_masks[k,j,i]

                    # Clean prostate mask = prostate mask - urethra mask -
                    # rectum mask
                    self.raw_prostate_masks[k,j,i] = \
                        self.raw_prostate_masks[k,j,i] and not \
                        self.raw_urethra_masks[k,j,i] and not \
                        self.raw_rectum_masks[k,j,i]

    def __refine_masks__(self):
        """
        Convert the masks to ones that have smaller, standard mesh element
        size.
        """
        new_mesh_element_x_dim = 0.01
        new_mesh_element_y_dim = 0.01

        if self.mesh_element_dimensions[2] < 0.01 and \
           self.mesh_element_dimensions[1] < 0.01:
            print "Warning: pixel dimensions are very fine. Organ mask " \
                "refinement will result in distortion."
            
        refined_mesh_x_dim = int(math.ceil(self.raw_prostate_masks.shape[2]* \
                                           self.mesh_element_dimensions[2]/ \
                                           new_mesh_element_x_dim))
        refined_mesh_y_dim = int(math.ceil(self.raw_prostate_masks.shape[1]* \
                                           self.mesh_element_dimensions[1]/ \
                                           new_mesh_element_y_dim))

        refined_mask_shape = (self.slices, \
                              refined_mesh_y_dim, \
                              refined_mesh_x_dim)
        self.refined_prostate_masks = np.zeros(refined_mask_shape, np.bool)
        self.refined_urethra_masks = np.zeros(refined_mask_shape, np.bool)
        self.refined_margin_masks = np.zeros(refined_mask_shape, np.bool)
        self.refined_rectum_masks = np.zeros(refined_mask_shape, np.bool)

        for slice in range(self.slices):
            for j in range(refined_mesh_y_dim):
                mesh_y_center = j*new_mesh_element_y_dim + \
                    new_mesh_element_y_dim/2.0

                raw_mask_j = np.searchsorted(self.raw_mask_mesh_y_boundaries, \
                                             mesh_y_center)
                if raw_mask_j >= self.raw_prostate_masks.shape[1]:
                    raw_mask_j = self.raw_prostate_masks.shape[1]-1
                
                for i in range(refined_mesh_x_dim):
                    mesh_x_center = i*new_mesh_element_x_dim + \
                        new_mesh_element_x_dim/2.0
                    
                    raw_mask_i = \
                        np.searchsorted(self.raw_mask_mesh_x_boundaries, \
                                        mesh_x_center)

                    if raw_mask_i >= self.raw_prostate_masks.shape[2]:
                        raw_mask_i = self.raw_prostate_masks.shape[2]-1

                    self.refined_prostate_masks[slice,j,i] = \
                        self.raw_prostate_masks[slice,raw_mask_j,raw_mask_i]
                    self.refined_urethra_masks[slice,j,i] = \
                        self.raw_urethra_masks[slice,raw_mask_j,raw_mask_i]
                    self.refined_margin_masks[slice,j,i] = \
                        self.raw_margin_masks[slice,raw_mask_j,raw_mask_i]
                    self.refined_rectum_masks[slice,j,i] = \
                        self.raw_rectum_masks[slice,raw_mask_j,raw_mask_i]
                    
    def __calculate_field_of_view__(self):
         """
         Find the field of view for the organ slices
         """
         i_max = 0
         i_min = self.refined_prostate_masks.shape[2]

         j_max = 0
         j_min = self.refined_prostate_masks.shape[1]

         for k in range(self.slices):
             for j in range(self.refined_prostate_masks.shape[1]):
                 for i in range(self.refined_prostate_masks.shape[2]):

                     if self.refined_prostate_masks[k,j,i] or \
                             self.refined_urethra_masks[k,j,i] or \
                             self.refined_margin_masks[k,j,i] or \
                             self.refined_rectum_masks[k,j,i]:
                         if j > j_max:
                             j_max = j
                         elif j < j_min:
                             j_min = j

                         if i > i_max:
                             i_max = i
                         elif i < i_min:
                             i_min = i

         # The FOV dimension needs to be a multiple of 10
         fov_x_dim = i_max - i_min + 1
         x_remainder = fov_x_dim % 10
         fov_x_extension = 0
         
         if x_remainder != 0:
             fov_x_extension = 10 - x_remainder
             
         fov_y_dim = j_max - j_min + 1
         y_remainder = fov_y_dim % 10
         fov_y_extension = 0
         
         if y_remainder != 0:
             fov_y_extension = 10 - fov_y_dim % 10

         self.fov_x = (i_min, i_max + fov_x_extension+1)
         self.fov_y = (j_min, j_max + fov_y_extension+1)

    def __coarsen_masks__(self):
        """
        The masks will be coarsened so that every array element corresponds to
        one potential brachytherapy seed location (instead of a pixel). Each
        refined pixel is assumed to be 0.01cm x 0.01cm. With these 
        dimensions, a 10 pixel by 10 pixel area has dimensions of 
        0.1cm x 0.1cm, which is roughly the diameter of most seeds. These
        dimensions will also allow for a standard needle template to be 
        overlaid on the organ masks.
        """
        # Calculate the field-of-view (FOV) for the refined organ masks 
        self.__calculate_field_of_view__()

        # Initialize the coarsened organ masks
        self.coarse_x_dim = (self.fov_x[1] - self.fov_x[0])/10;
        self.coarse_y_dim = (self.fov_y[1] - self.fov_y[0])/10;
        self.coarse_z_dim = self.slices
        coarse_mask_shape = (self.coarse_z_dim, \
                             self.coarse_y_dim, \
                             self.coarse_x_dim)
        
        self.coarse_prostate_masks = np.zeros(coarse_mask_shape, np.bool)
        self.coarse_urethra_masks = np.zeros(coarse_mask_shape, np.bool)
        self.coarse_margin_masks = np.zeros(coarse_mask_shape, np.bool)
        self.coarse_rectum_masks = np.zeros(coarse_mask_shape, np.bool)

        # Chop the refined masks to fit in the desired FOV
        self.refined_prostate_masks = \
            self.refined_prostate_masks[:,self.fov_y[0]:self.fov_y[1],\
                                        self.fov_x[0]:self.fov_x[1]]
        self.refined_urethra_masks = \
            self.refined_urethra_masks[:,self.fov_y[0]:self.fov_y[1],\
                                       self.fov_x[0]:self.fov_x[1]]
        self.refined_margin_masks = \
            self.refined_margin_masks[:,self.fov_y[0]:self.fov_y[1],\
                                      self.fov_x[0]:self.fov_x[1]]
        self.refined_rectum_masks = \
            self.refined_rectum_masks[:,self.fov_y[0]:self.fov_y[1],\
                                      self.fov_x[0]:self.fov_x[1]]

        # Coarsen the subarrays
        for k in range(self.coarse_z_dim):
            for j in range(self.coarse_y_dim):
                for i in range(self.coarse_x_dim):
                
                    # Tally the num of organ pixels in the selected 100x100 
                    # region
                    prostate_pixels = 0
                    urethra_pixels = 0
                    rectum_pixels = 0
                    margin_pixels = 0

                    pixel_x_start = i*10
                    pixel_y_start = j*10

                    for pixel_x_shift in range(10):
                        pixel_x = pixel_x_start + pixel_x_shift
                        
                        for pixel_y_shift in range(10):
                            pixel_y = pixel_y_start + pixel_y_shift

                            if self.refined_prostate_masks[k,pixel_y,pixel_x]:
                                prostate_pixels += 1
                            elif self.refined_urethra_masks[k,pixel_y,pixel_x]:
                                urethra_pixels += 1
                            elif self.refined_margin_masks[k,pixel_y,pixel_x]:
                                margin_pixels += 1
                            elif self.refined_rectum_masks[k,pixel_y,pixel_x]:
                                rectum_pixels += 1
                    
                    # Determine which organ to classify the coarsened region as
                    if urethra_pixels > 28:
                        self.coarse_urethra_masks[k,j,i] = True
                    elif rectum_pixels > 40:
                        self.coarse_rectum_masks[k,j,i] = True
                    elif margin_pixels > 40:
                        self.coarse_margin_masks[k,j,i] = True
                    elif prostate_pixels > 40:
                        self.coarse_prostate_masks[k,j,i] = True

    def __create_needle_template__(self):
        """
        Create the needle template. 

        The needle template has a spacing of 0.5 cm x 0.5 cm. This will simply
        be overlaid on the coarsened organ masks
        """
        needle_template_shape = (self.coarse_prostate_masks.shape[1],
                                 self.coarse_prostate_masks.shape[2])
        self.needle_template = np.zeros(needle_template_shape, np.bool)

        for j in range(2, self.needle_template.shape[0], 5):
            for i in range(2, self.needle_template.shape[1], 5):
                self.needle_template[j,i] = True
        

    def get_fov_prostate_masks(self):
        """
        Return the cleaned prostate masks in the field-of-view.
        """
        return self.refined_prostate_masks

    def get_fov_urethra_masks(self):
        """
        Return the urethra masks in the field-of-view.
        """
        return self.refined_urethra_masks
    
    def get_fov_margin_masks(self):
        """
        Return the cleaned margin masks in the field-of-view.
        """
        return self.refined_margin_masks

    def get_fov_rectum_masks(self):
        """
        Return the rectum masks in the field-of-view.
        """
        return self.refined_rectum_masks
    
    def get_coarsened_prostate_masks(self):
        """
        Return the coarsened prostate masks
        """
        return self.coarse_prostate_masks

    def get_coarsened_urethra_masks(self):
        """
        Return the coarsened urethra masks
        """
        return self.coarse_urethra_masks

    def get_coarsened_margin_masks(self):
        """
        Return the coarsened margin masks
        """
        return self.coarse_margin_masks

    def get_coarsened_rectum_masks(self):
        """
        Return the coarsened rectum masks
        """
        return self.coarse_rectum_masks

    def get_needle_template(self):
        """
        Return the needle template
        """
        return self.needle_template

# Test the MaskProcessor
if __name__ == '__main__':
    import argparse as ap
    import math
    from matplotlib import pyplot, figure
    from pytpor.ultrasound.imagemanager import ImageManager

    # Set up the argument parser
    description = "Testing script for the MaskProcessor class."

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
    
    # Create the MaskProcessor
    organ_mask_processor = MaskProcessor(prostate_masks, \
                                         urethra_masks, \
                                         margin_masks, \
                                         rectum_masks,
                                         0.018150,
                                         0.018150,
                                         0.500000)

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
    num_rows = total_slices/5
    num_rows += math.ceil((total_slices-5*num_rows)/5.0)
    
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

    print "Tests successful.\n"
