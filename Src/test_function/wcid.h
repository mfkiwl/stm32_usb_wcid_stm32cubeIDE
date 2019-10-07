/*
 * wcid.h
 *
 *  Created on: Oct 7, 2019
 *      Author: pavol
 */

#ifndef TEST_FUNCTION_WCID_H_
#define TEST_FUNCTION_WCID_H_

#define USB_REQ_GET_OS_FEATURE_DESCRIPTOR 0x20

typedef struct __attribute__((packed)) {
// Header
	uint32_t dwLength;
	uint16_t bcdVersion;
	uint16_t wIndex;
	uint8_t bCount;
	uint8_t bReserved1[7];
// Function Section 1
	uint8_t bFirstInterfaceNumber;
	uint8_t bReserved2;
	uint8_t bCompatibleID[8];
	uint8_t bSubCompatibleID[8];
	uint8_t bReserved3[6];
}  USBD_CompatibleIDDescriptor ;

#endif /* TEST_FUNCTION_WCID_H_ */
