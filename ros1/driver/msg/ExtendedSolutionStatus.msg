# Extended Solution Status
# Bit 0-2: Pseudorange Iono Correction
#     0 = Unknown or default Klobuchar model
#     1 = Klobuchar Broadcast
#     2 = SBAS Broadcast
#     3 = Multi-frequency Computed
#     4 = SPPDiff Correction
# Bit 3: AtmIntegrity Use Flat
#     0 = Not Used
#     1 = Used
# Bit 4-7: AtmIntegrity Version
#     0 = Version 3.0
#     1 = Version 4.0
# Bit 8-13: AtmIntegrity Timeout (0~61s)
# bit 14-15: Reserved
uint16 status