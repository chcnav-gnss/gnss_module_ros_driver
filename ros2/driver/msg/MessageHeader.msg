# Message Header

uint16 message_id

# range: 0 ~ 1
uint8  antenna_index

uint8 sequence_num

# 0 indicates the processor is fully occupied. Other values indicate the availability of the processor to take on tasks.
# range: 0 ~ 100
uint8 cpu_idle

# GPS Reference Time Status:
# 20    UNKNOWN               Time validity is unknown
# 60    APPROXIMATE           Time is set approximately
# 80    COARSEADJUSTING       Time is approaching coarse precision
# 100   COARSE                This time is valid to coarse precision
# 120   COARSESTEERING        Time is coarse set and is being steered
# 130   FREEWHEELING          Position is lost and the range bias cannot be calculated
# 140   FINEADJUSTING         Time is adjusting to fine precision
# 160   FINE                  Time has fine precision
# 170   FINEBACKUPSTEERING    Time is fine set and is being steered by the backup system
# 180   FINESTEERING          Time is fine set and is being steered
# 200   SATTIME               Time from satellite. Only used in logs containing satellite data such as ephemeris and almanac
uint8  gps_time_status

uint16 gps_week_num
uint32 gps_milliseconds

# Firmware Version
uint32 firmware_version

#  bit    bitmask        defination      Description
#  0      0x00000001     ERR             接收机出现任何异常的汇总
#  1      0x00000002     TEMPERATURE     温度异常标识
#  2      0x00000004     Reserved        NA
#  3      0x00000008     Reserved        NA
#  4      0x00000010     Reserved        NA
#  5      0x00000020     ANT_OPEN        主天线开路标识
#  6      0x00000040     ANT_SHORT       主天线短路标识
#  7      0x00000080     CPU_OVERLOAD    CPU MIPS过载
#  8      0x00000100     COM_OVF         串口输出过载
#  9      0x00000200     SPOOFING        诱骗标志
#  10     0x00000400     HIGH_DYNAMIC    高动态标识
#  11     0x00000800     LINK_OVERRUN    除去串口之外的其他端口出现overrun的情况
#  12     0x00001000     INPUT_OVERRUN   任意端口出现输入overrun的情况
#  13     0x00002000     Reserved        NA
#  14     0x00004000     AGC_OVF         射频AGC增益溢出
#  15     0x00008000     JAM_DETECTED    干扰标志
#  16     0x00010000     INS_RESET       INS发散重置标志
#  17     0x00020000     IMU_COMM_FAIL   IMU通信错误
#  18     0x00040000     SIMULATOR       模拟器模式
#  19     0x00080000     POS_FLAG        定位异常标识
#  20     0x00100000     FIX             基站固定模式
#  21     0x00200000     CLK_STEERING    NA
#  22     0x00400000     Reserved        NA
#  23     0x00800000     EX_CLOCK        外钟模式
#  24     0x01000000     Reserved        NA
#  25     0x02000000     Reserved        Trick
#  26     0x04000000                 
#  27     0x08000000     TRACKING        跟踪模式
#  28     0x10000000     Reserved        NA
#  29     0x20000000     AUX3_EVENT      扩展事件3
#  30     0x40000000     AUX2_EVENT      扩展事件2
#  31     0x80000000     AUX1_EVENT      扩展事件1
uint32 receiver_status
