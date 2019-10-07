
#include "usbd_conf.h"
#include "usbd_core.h"
#include "usbd_desc.h"

static USBD_HandleTypeDef usbHandle;

extern USBD_DescriptorsTypeDef fullSpeedDescriptorsHooks;

extern USBD_ClassTypeDef functionHooks;

void test_function_init(void)
{
	if (USBD_Init(&usbHandle, &fullSpeedDescriptorsHooks, DEVICE_FS) != USBD_OK) {
		Error_Handler();
	}

	if (USBD_RegisterClass(&usbHandle, &functionHooks) != USBD_OK) {
		Error_Handler();
	}

	if (USBD_Start(&usbHandle) != USBD_OK) {
		Error_Handler();
	}
}
