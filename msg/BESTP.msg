std_msgs/Header header

MessageHeader raw_msg_header

# Position Solution Status
SolutionStatus pos_sol_status

# Position Type
PositionOrVelocityType pos_type

# Datum ID number
# 0 = WGS84
# 1 = USER
uint8 datum_id

# Latitude (degrees)
float64 lat

# Longitude (degrees)
float64 lon

# Height above mean sea level (metres)
float64 hgt

# Undulation - the relationship between the geoid and the ellipsoid (m) of the chosen datum
# When using a datum other than WGS84, the undulation value also includes the vertical shift
# due to differences between the datum in use and WGS84.
float32 undulation

# Latitude standard deviation (m)
float32 lat_std

# Longitude standard deviation (m)
float32 lon_std

# Height standard deviation (m)
float32 hgt_std

# Differential age in seconds
float32 diff_age

# Solution age in seconds
float32 sol_age

# Number of satellites tracked
uint8 track_svs

# Number of satellites used in solution
uint8 sol_svs

# Number of satellites with multi-frequency signals used in solution
uint8 sol_muti_svs

# Number of satellites with L1/E1/B1 signals used in solution
uint8 sol_l1_svs

ExtendedSolutionStatus ext_sol_status

# GPS and GLONASS Signal-Used Mask
GpsGloSigMask gps_glo_sig_mask

# BeiDou Signal-Used Mask
BeiDouSigMask bd_sig_mask

# Galileo Signal-Used Mask
GalSigMask gal_sig_mask

# Base station ID
uint16 station_id
