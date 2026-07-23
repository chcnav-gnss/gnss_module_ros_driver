#  Code  Defination         Description
#   0    SOL_COMPUTED       Solution computed
#   1    INSUFFICIENT_OBS   Insufficient observations
#   2    NO_CONVERGENCE     No convergence
#   3    SINGULARITY        Singularity at parameters matrix
#   4    COV_TRACE          Covariance trace exceeds maximum (trace > 1000 m)
#   5    VARIANCE           Variance exceeds limits
#   6    PENDING            When a SETFIXPOS position command is entered, the receiver computes its own position and determines if the fixed position is valid
#   7    INVALID_FIX        The fixed position, entered using the SETFIXPOS position command, is not valid
#   8    RESIDUALS          Residuals are too large
#   9    INTEGRITY_WARNING  Large residuals make position unreliable
uint8 status