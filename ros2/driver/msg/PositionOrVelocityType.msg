#  Code  Defination        Description
#   0    NONE              No solution
#   1    SINGLE            Solution calculated using only data supplied by the GNSS satellites
#   2    SPPDIFF           Solution calculated using pseudorange differential (DGPS, DGNSS) corrections
#                          Reserved 3
#   4    NARROW_INT        Multi-frequency RTK solution with carrier phase ambiguities resolved to narrow-lane integers
#   5    NARROW_FLOAT      Multi-frequency RTK solution with unresolved, float carrier phase ambiguities
#                          Reserved 6
#   7    FIXEDPOS          Position has been fixed by the SETFIXPOS position command or by position averaging.
#                          Reserved 8
#   9    SBAS              Solution calculated using corrections from an SBAS satellite
#   10   PPP               Converged B2b or HAS or MADOCA PPP solution
#   11   PPP_FIXED         Converged PointSky or CLAS PPP solution
#   12   PPP_CONVERGING    Converging PPP solution
#   13   WIDE_INT          Multi-frequency RTK solution with carrier phase ambiguities resolved to wide-lane integers
#   14   L1_INT            Single-frequency RTK solution with carrier phase ambiguities resolved to integers
#   15   L1_FLOAT          Single-frequency RTK solution with unresolved, float carrier phase ambiguities
#   16   DOPPLER_VELOCITY  Velocity computed using instantaneous Doppler
#                          Reserved 17-19
#   20   INS               INS positioning solution
#   21   INS_SBAS          INS position, where the last applied position update used a GNSS solution computed using corrections from an SBAS solution
#   22   INS_SPP           INS position, where the last applied position update used a single point GNSS(SINGLE) solution
#   23   INS_SPPDIFF       INS position, where the last applied position update used a pseudorange differential GNSS (SPPDIFF) solution
#   24   INS_RTKFLOAT      INS position, where the last applied position update used a floating ambiguity RTK (L1_FLOAT or NARROW_FLOAT) solution
#   25   INS_RTKFIXED      INS position, where the last applied position update used a fixed integer ambiguity RTK (L1_INT, WIDE_INT or NARROW_INT) solution
#   26   INS_PPP           INS position, where the last applied position update used B2b or HAS or MADOCA ppp converged solution
#   27   INS_PPPFIXED      INS position, where the last applied position update used PointSky or CLAS ppp converged solution
uint8 type