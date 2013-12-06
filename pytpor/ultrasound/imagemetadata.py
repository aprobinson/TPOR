"""@package imagemetadata
The module for managing ultrasound image metadata
"""
#! /usr/bin/env python

class ImageMetadata(object):
    """
    This class stores all image metadata.
    """
    def __init__(self, ultrasound_image_header_file):
        """
        Initialize the ImageMetadata class.

        A string containing the ultrasound image header file (with path) must
        be passed as an argument.
        """
        # Open the ultrasound image header file for reading only
        header_file = open(ultrasound_image_header_file, 'r')

        # Store all of the header metadata
        for line in header_file:
            line = line.translate(None, ':=;')
            words = line.split()

            if words[0] == 'data_type':
                if words[1] == 'char':
                    self.image_data_type = 'unit8'
                else:
                    print "unsupported binary image file data type: "+\
                        words[1]
                    raise TypeError                    
                
            elif words[0] == 'byte_order':
                self.image_byte_order = int(words[1])
                if self.image_byte_order != 1:
                    print "unsupported binary image file byte order: "+\
                        str(self.image_byte_order)
                    raise ValueError

            elif words[0] == 'bytes_pix':
                self.image_bytes_per_pixel = int(words[1])
                if self.image_bytes_per_pixel != 1:
                    print "unsupported binary image file bytes-per-pixel: "+\
                        srt(self.image_bytes_per_pixel)
                    raise ValueError

            elif words[0] == 'vol_min':
                self.min_volume = int(words[1])

            elif words[0] == 'vol_max':
                self.max_volume = int(words[1])

            elif words[0] == 'x_dim':
                self.num_x_pixels = int(words[1])

            elif words[0] == 'y_dim':
                self.num_y_pixels = int(words[1])

            elif words[0] == 'z_dim':
                self.slices = int(words[1])

            elif words[0] == 'dim_units':
                self.pixel_dimension_units = words[1]

            elif words[0] == 'x_pixdim':
                self.pixel_x_extent = float(words[1])
                
            elif words[0] == 'y_pixdim':
                self.pixel_y_extent = float(words[1])
                
            elif words[0] == 'z_pixdim':
                self.slice_extent = float(words[1])

            elif words[0] == 'x_start':
                self.starting_pixel_x_index = int(float(words[1]))

            elif words[0] == 'y_start':
                self.starting_pixel_y_index = int(float(words[1]))

            elif words[0] == 'z_start':
                self.starting_slice_index = int(float(words[1]))

            elif words[0] == 'db_name':
                self.patient_name = words[1]
                
                if len(words) == 3:
                    self.patient_name += "_" + words[2]
                
    def get_image_file_data_type(self):
        """
        Return the binary image file data type. This type is useable as the 
        dtype in the numpy.fromfile function.
        """
        return self.image_data_type

    def get_image_file_byte_order(self):
        """
        Return the binary image file byte order. 
        """
        return self.image_byte_order

    def get_image_file_bytes_per_pixel(self):
        """
        Return the binary image file bytes-per-pixel.
        """
        return self.image_bytes_per_pixel

    def get_image_min_volume(self):
        """
        Return the minimum image volume (square cm)
        """
        return self.min_volume

    def get_image_max_volume(self):
        """
        Return the maximum image volume (square cm)
        """
        return self.max_volume

    def get_image_x_resolution(self):
        """
        Return the number of pixels in the x dimension
        """
        return self.num_x_pixels

    def get_image_y_resolution(self):
        """
        Return the number of pixels in the y dimension
        """
        return self.num_y_pixels

    def get_number_image_slices(self):
        """
        Return the number of image slices
        """
        return self.slices

    def get_pixel_units(self):
        """
        Return the units associated with the pixel size
        """
        return self.pixel_dimension_units

    def get_pixel_size_x(self):
        """
        Return the extent of a pixel in the x dimension
        """
        return self.pixel_x_extent

    def get_pixel_size_y(self):
        """
        Return the extent of a pixel in the y dimension
        """
        return self.pixel_y_extent

    def get_slice_offset(self):
        """
        Return the offset between two consecutive slices
        """
        return self.slice_extent

    def get_start_pixel_x_index(self):
        """
        Return the x index of the starting pixel
        """
        return self.starting_pixel_x_index

    def get_start_pixel_y_index(self):
        """
        Return the y index of the starting pixel
        """
        return self.starting_pixel_y_index

    def get_start_slice_index(self):
        """
        Return the index of the starting slice
        """
        return self.starting_slice_index

    def get_patient_name(self):
        """
        Return the name of the patient
        """
        return self.patient_name

# Test the ImageMetadata
if __name__ == '__main__':
    import argparse as ap

    # Set up the argument parser
    description = "Testing script for the ImageMetadata class"

    parser = ap.ArgumentParser(description=description)

    image_header_file = "ultrasound image header file (with path)"
    parser.add_argument('image_header_file', help=image_header_file)

    # Parse the user's arguments
    user_args = parser.parse_args()

    # Create the ImageMetadata
    image_metadata = ImageMetadata(user_args.image_header_file)
    
    # Print all of the metadata
    print image_metadata.get_image_file_data_type()
    print image_metadata.get_image_file_byte_order()
    print image_metadata.get_image_file_bytes_per_pixel()
    print image_metadata.get_image_min_volume()
    print image_metadata.get_image_max_volume()
    print image_metadata.get_image_x_resolution()
    print image_metadata.get_image_y_resolution()
    print image_metadata.get_number_image_slices()
    print image_metadata.get_pixel_units()
    print image_metadata.get_pixel_size_x()
    print image_metadata.get_pixel_size_y()
    print image_metadata.get_slice_offset()
    print image_metadata.get_start_pixel_x_index()
    print image_metadata.get_start_pixel_y_index()
    print image_metadata.get_start_slice_index()
    print image_metadata.get_patient_name()

    print "\nTests successful.\n"
