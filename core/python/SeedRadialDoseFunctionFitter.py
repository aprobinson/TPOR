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

# Tabulated radial dose function for Amersham EchoSeed 6733
amersham_6733_data = np.array([1.050, 1.076, 1.085, 1.069, 1.045, 1.000, \
                               0.912, 0.821, 0.656, 0.495, 0.379, 0.285, \
                               0.214, 0.155, 0.119, 0.0840])

# Tabulated radial dose function for Draximage BrachySeed LS-1
draximage_ls1_data = np.array([0.182, 0.323, 0.741, 0.964, 1.004, 1.000, \
                               0.937, 0.853, 0.680, 0.527, 0.400, 0.300, \
                               0.223, 0.166, 0.122, 0.0900])

# Tabulated radial dose function for Implant Sciences 3500
implant_sciences_3500_data = np.array([0.997, 1.011, 1.021, 1.030, 1.026, \
                                       1.000, 0.932, 0.854, 0.681, 0.532, \
                                       0.407, 0.308, 0.230, 0.171, 0.127, \
                                       0.0936])

# Tabulated radial dose function for IBt 1251L
ibt_1251l_data = np.array([0.757, 0.841, 0.963, 1.021, 1.024, 1.000, 0.937, \
                           0.859, 0.700, 0.554, 0.425, 0.323, 0.240, 0.180, \
                           0.138, 0.101])

# Tabulated radial dose function for IsoAid advantage IAI-125A
isoaid_iai_125a_data = np.array([1.040, 1.053, 1.066, 1.080, 1.035, 1.000, \
                                 0.902, 0.800, 0.611, 0.468, 0.368, 0.294, \
                                 0.227, 0.165, 0.141, 0.090])

# Tabulated radial dose function for MBI SL-125 SH-125
mbi_sl125_sh125_data = np.array([1.101, 1.101, 1.101, 1.084, 1.041, 1.000, \
                                 0.898, 0.795, 0.610, 0.456, 0.338, 0.250, \
                                 0.183, 0.134, 0.098, 0.072])

# Tabulated radial dose function for Source Tech STM1251
source_tech_stm1251_data = np.array([0.941, 0.972, 1.013, 1.033, 1.022, 1.000,\
                                     0.937, 0.856, 0.691, 0.540, 0.415, 0.314,\
                                     0.236, 0.176, 0.131, 0.0969])

# Tabulated radial dose function for Best Medical 2335
best_2335_data = np.array([0.826, 1.066, 1.236, 1.307, 1.128, 1.000, 0.742, \
                           0.533, 0.296, 0.158, 0.0920, 0.0529, 0.0309, \
                           0.0180, 0.0105, 0.0062])

# Tabulated radial dose function for Amersham 9011 seed
amersham_9011_radii = np.array([0.2, 0.3, 0.5, 0.7, 1.0, 1.5, 2.0, 2.5, 3.0, \
                                3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, \
                                8.0, 8.5, 9.0, 9.5, 10.0, 12.0])
amersham_9011_data = np.array([1.079, 1.084, 1.072, 1.047, 1.000, 0.908, \
                               0.811, 0.717, 0.629, 0.549, 0.477, 0.413, \
                               0.357, 0.308, 0.265, 0.228, 0.196, 0.168, \
                               0.144, 0.124, 0.106, 0.091, 0.078, 0.042]) 

# Tabulated radial dose function for Nucletron 130.002 seed
nucletron_130002_radii = np.array([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, \
                                   0.9, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 6, \
                                   7, 8, 9, 10]) 
nucletron_130002_data = np.array([1.042, 1.082, 1.087, 1.085, 1.078, 1.066, \
                                  1.052, 1.035, 1.019, 1.000, 0.907, 0.808, \
                                  0.713, 0.627, 0.548, 0.477, 0.414, 0.357, \
                                  0.265, 0.196, 0.144, 0.106, 0.078]) 

# Tabulated radial dose function for IsoAid IAPd-103A seed
isoaid_iapd103a_radii = np.array([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.75, 0.8, 0.9, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10.0])
isoaid_iapd103a_data = np.array([0.915, 1.234, 1.296, 1.290, 1.260, 1.213, 1.160, 1.134, 1.106, 1.053, 1.000, 0.768, 0.576, 0.429, 0.318, 0.233, 0.173, 0.127, 0.092, 0.069, 0.050, 0.037, 0.028, 0.020, 0.015, 0.011, 0.008, 0.006, 0.005])

# Tabulated radial dose function for Theragenics AgX100 Seed
theragenics_agx100_radii = np.array([0.1, 0.15, 0.2, 0.25, 0.3, 0.5, 0.75, 1, \
                                     1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 6, 7, 8, \
                                     9, 10])
theragenics_agx100_data = np.array([1.066, 1.086, 1.096, 1.098, 1.097, 1.076, \
                                    1.042, 1.000, 0.908, 0.813, 0.720, 0.633, \
                                    0.553, 0.482, 0.418, 0.361, 0.269, 0.199, \
                                    0.147, 0.108, 0.079]) 



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

    seed_name = "Seed name: \n\tAmersham_6702\n\tAmersham_6711"\
        "\n\tAmersham_6733\n\t\n\tAmersham_9011\n\tBest_2301\n\tBest_2335"\
        "\n\tNASI_MED3631\n\tNASI_MED3633\n\tBebig_I25_S06\n\tImagyn_IS12501"\
        "\n\tTheragenics_200\n\tTheragenics_AgX100\n\tDraximage_LS1"\
        "\n\tImplant_Sciences_3500\n\tIBt_1251L\n\tIsoAid_IAI125A"\
        "\n\tIsoAid_IAPd103A\n\tMBI_SL125_SH125\n\tSource_Tech_STM1251\n\t"
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
    elif user_args.seed_name == "Amersham_6733":
        independent_data = i125_radii
        dependent_data = amersham_6733_data
    elif user_args.seed_name == "Amersham_9011":
        independent_data = amersham_9011_radii
        dependent_data = amersham_9011_data
    elif user_args.seed_name == "Best_2301":
        independent_data = i125_radii
        dependent_data = best_2301_data
    elif user_args.seed_name == "Best_2335":
        independent_data = i125_radii
        dependent_data = best_2335_data
    elif user_args.seed_name == "NASI_MED3631":
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
    elif user_args.seed_name == "Theragenics_AgX100":
        independent_data = theragenics_agx100_radii
        dependent_data = theragenics_agx100_data
    elif user_args.seed_name == "Draximage_LS1":
        independent_data = i125_radii[3:]
        dependent_data = draximage_ls1_data[3:]
    elif user_args.seed_name == "Implant_Sciences_3500":
        independent_data = i125_radii
        dependent_data = implant_sciences_3500_data 
    elif user_args.seed_name == "IBt_1251L":
        independent_data = i125_radii[3:]
        dependent_data = ibt_1251l_data[3:]
    elif user_args.seed_name == "IsoAid_IAI125A":
        independent_data = i125_radii
        dependent_data = isoaid_iai_125a_data
    elif user_args.seed_name == "IsoAid_IAPd103A":
        independent_data = isoaid_iapd103a_radii[2:]
        dependent_data = isoaid_iapd103a_data[2:]
    elif user_args.seed_name == "MBI_SL125_SH125":
        independent_data = i125_radii
        dependent_data = mbi_sl125_sh125_data
    elif user_args.seed_name == "Source_Tech_STM1251":
        independent_data = i125_radii
        dependent_data = source_tech_stm1251_data
    elif user_args.seed_name == "Nucletron_130002":
        independent_data = nucletron_130002_radii[1:]
        dependent_data = nucletron_130002_data[1:]
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
        
