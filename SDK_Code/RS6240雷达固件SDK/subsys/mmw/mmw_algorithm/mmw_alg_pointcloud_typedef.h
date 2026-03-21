#ifndef MMW_POINT_CLOUD_COMMON_H
#define MMW_POINT_CLOUD_COMMON_H
#include "mmw_ctrl.h"

typedef enum {
    CART_TYPE,
    POLAR_TYPE
}PointCoordinateType_e;

typedef struct {
    uint16_t range_idx;         /* range index based on 0 */
    uint16_t doppler_idx;       /* doppler index based on 0, 0 is the max negetive range rate, doppler_len/2 refers to static */
    float azi_phase;            /* difference of phase for azimuth ants */
    float ele_phase;            /* difference of phase for elev ants */
    float sig_snr;              /* snr(dB) normalize to one ant. */
    float sig_power;            /* power of each target after non-coherent, sum of all mimo rxs */
    uint8_t cluster_id;         /* reserved for cluster & tracking application */
} MmwClusterDetectData_t;

typedef enum {
	/* 
	 * MMW_MOUNT_HORIZONTAL is defined:
	 *  ________________          ______________________		 ___________		 ____________
	 * |                |        |                      |		|Tx0	 	|		|	  	Rx3  |
	 * | Rx2          Tx|        |                Tx0   |		|	 Rx3	|		|	  		 |
	 * | Rx1 Rx0        |        |  Rx0                 |		|	 Rx2	|		|    	Rx2  |
	 * |________________|        |  Rx1 Rx2 Rx3         |		|	 Rx1	|		|	Rx1  	 |
	 *       RS6130              |                Tx1   |		|	 Rx0	|		|		Rx0	 |
     *                           |______________________|		|Tx1	 	|		|		 	 |
	 *                                    RS6240                |___________|		|Tx0		 |
	 * 															 RS7241_P2840		|Tx1		 |
	 *															 					|____________|
	 *																				 RS7241_P2828
	 * */
    MMW_MOUNT_HORIZONTAL,

	/* 
	 * VER_MOUNT_MODE is defined follows when at TOP view of the chip:
	 *  _________		 ____________		 ______________________		     _______________________________
	 * | Tx		 |		|Tx0	  Tx1|		|Tx1			   Tx0 |		|Tx1 Tx0						|
	 * |         |      |            |		|					   |		|				Rx1			  	|
	 * | 		 |		|			 |		|	Rx0 Rx1 Rx2 Rx3	   |		|			Rx0     Rx2 	Rx3	|
	 * | 		 | 		|			 |		|______________________|		|_______________________________|
	 * | 	 Rx0 |		|		Rx3	 |			  RS7241_P2840							RS7241_P2828
	 * | Rx2 Rx1 |		|		Rx2	 |										
	 * |_________|		|   Rx0	Rx1	 |									
	 *  RS6130          |____________|												       
	 * 						RS6240
	 * */
    MMW_MOUNT_VERTICAL,
	MMW_MOUNT_CEIL,
    MMW_MOUNT_MAX
}MmwMountType_e;

typedef struct {
    MmwMountType_e mount_mode;
    PointCoordinateType_e coord_type;
}MmwRadarConfig_t;

typedef struct {
    MmwClusterDetectData_t* ptr_motion_point_cloud_data;            /* storage of point cloud,length is point_cloud_num */
    uint16_t point_cloud_num;                                       /* num of motion point cloud */
}PointCloudBuffer_t;

/**
 * @brief Element structure of Angle result.
 */
typedef struct {
    int16_t azimuth;    /* Q7, degree */
    int16_t elevation;  /* Q7, degree */
    int16_t sinValue_X; /* Q15, X = R * sinValue_X */
    int16_t sinValue_Y; /* Q15, Y = R * sinValue_Y */
    int16_t sinValue_Z; /* Q15, Z = R * sinValue_Z */
    int16_t sideinfo;   /* reserve */
} MmwAngleInfo_t;

typedef int16_t S16Q8;

typedef struct {
    float real;
    float imag;
} Complexf32_RealImag;

typedef struct {
    int32_t real;
    int32_t imag;
} Complex32_RealImag;

typedef struct {
	uint8_t ant_tx_num;			/* enabled physical tx number */
	uint8_t ant_rx_num;			/* enabled physical ant number */
	uint8_t mimo_rx_num;		/* .ant_tx_num* * .ant_rx_num, virtual mimo number */
} MmwPsicMimoRxNum_t;

typedef struct {
	uint16_t range_fft_num;			/* range fft number */
	uint16_t dop_fft_num;			/* doppler fft number */
} MmwPsic2DFFTNum_t;

/* This struct is used to save geometry data for azi and elev */
typedef struct {
	uint8_t *ptr_geometry;
	uint8_t geometry_len;
} MmwPointAntGeometry_t;

typedef enum {
    MMW_DISABLE = 0,
	MMW_ENABLE,
}MmwPointCloudEnStatus_e;

/*
 * The order of this enum should NOT change!
*/
typedef enum {
    PEAKGROUPING_DISABLE,
    PEAKGROUPING_DOPPLER,
    PEAKGROUPING_DOPPLER_RANGE,
    PEAKGROUPING_RECTANGLE
} PeakGroupingMethod_e;


typedef enum {
    /* No clutter removal, keep power of stationary in datacube for all range */
    POINT_CLUTTER_REMOVAL_NONE,
    /* Clutter removal only for dc, keep power of stationary in datacube for far range */
    POINT_CLUTTER_REMOVAL_DC,  
    /* Clutter removal for all, DON'T keep power of stationary in datacube for far range */
    POINT_CLUTTER_REMOVAL_ALL,
} MmwPointClutterRemoval_e;

#endif/*MMW_POINT_CLOUD_COMMON_H*/