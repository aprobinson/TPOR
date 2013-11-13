#! /usr/bin/env python
import numpy as np

def modified_cunningham_equation(x, a1, a2, a3, a4, a5):
    """
    Modified Cunningham equation for fitting radial dose functions.
    """
    return a5*(a3+np.exp(a1*(x-a4)))/(a3+np.exp(a1*(x-a4))+np.exp(a2*(x-a4)))

def fifth_order_polynomial(x, a0, a1, a2, a3, a4, a5):
    """
    Polynomial of order 5 for fitting radial dose functions.
    """
    return a0+a1*x+a2*x**2+a3*x**3+a4*x**4+a5*x**5

# Radial points (cm) where all radial dose funtions are tabulated
i125_radii = np.array([0.1, 0.15, 0.25, 0.50, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, \
                       5.0, 6.0, 7.0, 8.0, 9.0, 10.0])
nasi_i125_radii = i125_radii[2:13]
pd103_radii = np.array([0.1, 0.15, 0.25, 0.3, 0.4, 0.5, 0.75, 1.0, 1.5, 2.0, \
                        2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 7.0, 10.0])
nasi_pd103_radii = pd103_radii[2:17]

# Tabulated radial dose function for Amersham 6702 seed
amersham_6702_data = np.array([1.020, 1.022, 1.024, 1.030, 1.020, 1.000, \
                               0.935, 0.861, 0.697, 0.553, 0.425, 0.332, \
                               0.241, 0.179, 0.134, 0.0979])

# Tabulated radial dose function for Amersham 6711 seed
amersham_6711_data = np.array([1.055, 1.078, 1.082, 1.071, 1.042, 1.000, \
                               0.908, 0.814, 0.632, 0.496, 0.364, 0.270, \
                               0.199, 0.148, 0.109, 0.0803])

# Tabulated radial dose function for Best 2301 seed
best_2301_data = np.array([1.033, 1.029, 1.027, 1.028, 1.030, 1.000, 0.938, \
                           0.866, 0.707, 0.555, 0.427, 0.320, 0.248, 0.187, \
                           0.142, 0.103])

# Tabulated radial dose function for NASI MED3631-A/M seed
nasi_med_3631_data = np.array([0.998, 1.025, 1.019, 1.000, 0.954, 0.836, \
                               0.676, 0.523, 0.395, 0.293, 0.211])

# Tabulated radial dose function for Bebig I25.S06 seed
bebig_i25_s06_data = np.array([1.010, 1.018, 1.030, 1.030, 1.020, 1.000, \
                               0.937, 0.857, 0.689, 0.538, 0.409, 0.313, \
                               0.232, 0.176, 0.134, 0.0957])

# Tabulated radial dose function for Imagyn IS12501 seed
imagyn_is12501_data = np.array([1.022, 1.058, 1.093, 1.080, 1.048, 1.000, \
                                0.907, 0.808, 0.618, 0.463, 0.348, 0.253, \
                                0.193, 0.149, 0.100, 0.075])

# Tabulated radial dose function for Theragenics 200 seed
theragenics_200_data = np.array([0.911, 1.21, 1.37, 1.38, 1.36, 1.30, 1.15, \
                                 1.000, 0.749, 0.555, 0.410, 0.302, 0.223, \
                                 0.163, 0.0887, 0.0482, 0.0262, 0.00615])

# Tabulated radial dose function for NASI MED3633 seed
nasi_med_3633_data = np.array([1.331, 1.322, 1.286, 1.243, 1.125, 1.000, \
                               0.770, 0.583, 0.438, 0.325, 0.241, 0.177, \
                               0.098, 0.053, 0.028])

# Run the script directly
if __name__ == '__main__':
    import argparse as ap
    import scipy.optimize
    import pylab
    import sys

    # Set up the argument parser
    description = "Fit the tabulated radial dose function of a brachytherapy "\
        "seed to a modified Cunningham equation."
    
    parser = ap.ArgumentParser(description=description)

    seed_name = "Seed name: \n\tAmersham_6702\n\tAmersham_6711\n\tBest_2301"\
        "\n\tNASI_MED3632\n\tNASI_MED3633\n\tBebig_I25_S06\n\tImagyn_IS12501"\
        "\n\tTheragenics_200\n\t"
    parser.add_argument('seed_name', help=seed_name)

    # Parse the user's arguments
    user_args = parser.parse_args()

    # Load the necessary data arrays
    independent_data = np.array([])
    dependent_data = np.array([])
    if user_args.seed_name == "Amersham_6702":
        independent_data = i125_radii
        dependent_data = amersham_6702_data
    elif user_args.seed_name == "Amersham_6711":
        independent_data = i125_radii
        dependent_data = amersham_6711_data
    elif user_args.seed_name == "Best_2301":
        independent_data = i125_radii
        dependent_data = best_2301_data
    elif user_args.seed_name == "NASI_MED3632":
        independent_data = nasi_i125_radii 
        dependent_data = nasi_med_3631_data
    elif user_args.seed_name == "NASI_MED3633":
        independent_data = nasi_pd103_radii
        dependent_data = nasi_med_3633_data
    elif user_args.seed_name == "Bebig_I25_S06":
        independent_data = i125_radii
        dependent_data = bebig_i25_s06_data
    elif user_args.seed_name == "Imagyn_IS12501":
        independent_data = i125_radii
        dependent_data = imagyn_is12501_data
    elif user_args.seed_name == "Theragenics_200":
        independent_data = pd103_radii
        dependent_data = theragenics_200_data
    else:
        sys.exit("Invalid seed name.")
        
    # Generate the fifth order polynomial fit coeffs. for the specified seed
    popt_poly, pcov_poly = scipy.optimize.curve_fit(fifth_order_polynomial,\
                                                    independent_data, \
                                                    dependent_data)
    
    # Generate the modified Cunningham fit coefficients for the specified seed
    popt_cun, pcov_cun =scipy.optimize.curve_fit(modified_cunningham_equation,\
                                                 independent_data, \
                                                 dependent_data)

    # Print the coefficients
    print "\nFifth Order Polynomial Fit Coefficients: "
    print "a0 = ", popt_poly[0]
    print "a1 = ", popt_poly[1]
    print "a2 = ", popt_poly[2]
    print "a3 = ", popt_poly[3]
    print "a4 = ", popt_poly[4]
    print "a5 = ", popt_poly[5]
    print "\nModified Cunningham Fit Coefficients: "
    print "a1 = ", popt_cun[0]
    print "a2 = ", popt_cun[1]
    print "a3 = ", popt_cun[2]
    print "a4 = ", popt_cun[3]
    print "a5 = ", popt_cun[4]

    # Print the fit data
    print "\nFifth Order Polynomial Fit Quality:\n", pcov_poly
    print "\nModified Cunningham Fit Quality:\n", pcov_cun

    # Plot the original data and the functional fit
    samples = np.arange(0,30,0.01)
    poly_function_samples = fifth_order_polynomial(samples, \
                                                   popt_poly[0], \
                                                   popt_poly[1], \
                                                   popt_poly[2], \
                                                   popt_poly[3], \
                                                   popt_poly[4], \
                                                   popt_poly[5])
    cun_function_samples = modified_cunningham_equation(samples, \
                                                        popt_cun[0], \
                                                        popt_cun[1], \
                                                        popt_cun[2], \
                                                        popt_cun[3], \
                                                        popt_cun[4])
    pylab.plot(samples, poly_function_samples, color="blue")
    pylab.plot(samples, cun_function_samples, color="red")
    pylab.scatter(independent_data, dependent_data, color="black")
    pylab.xlim(0.0, 30.0)
    pylab.ylim(0.0, dependent_data.max())
    pylab.show()
        
