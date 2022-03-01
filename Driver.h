#pragma once
#define TEST_DRIVER_PATH L"Z:\\Racoon Driver\\Release\\Racoon Driver\\RacoonDriver.sys"

#define IOCTL_EXAMPLE_SAMPLE_DIRECT_IN_IO    \
        CTL_CODE(FILE_DEVICE_UNKNOWN,        \
            0x800,                           \
            METHOD_IN_DIRECT,                \
            FILE_WRITE_DATA)

#define IOCTL_EXAMPLE_SAMPLE_DIRECT_OUT_IO   \
        CTL_CODE(FILE_DEVICE_UNKNOWN,        \
            0x801,                           \
            METHOD_OUT_DIRECT,               \
            FILE_READ_DATA)

#define IOCTL_EXAMPLE_SAMPLE_NARUTO_LOL      \
        CTL_CODE(FILE_DEVICE_UNKNOWN,        \
            0x802,                           \
            METHOD_OUT_DIRECT,               \
            FILE_READ_DATA)