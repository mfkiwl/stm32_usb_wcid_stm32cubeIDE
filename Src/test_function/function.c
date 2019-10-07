
#include "usbd_conf.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_ctlreq.h"
#include "wcid.h"

#define EPOUT_ADDR 			0x1
#define EPIN_ADDR 			0x81
#define MAX_PACKET 			64

static __ALIGN_BEGIN uint8_t receive_buffer[4*MAX_PACKET] __ALIGN_END;

static __ALIGN_BEGIN uint8_t transmit_buffer[(8*MAX_PACKET)-1] __ALIGN_END;

static uint8_t Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	USBD_LL_OpenEP(pdev, EPOUT_ADDR, USBD_EP_TYPE_BULK, MAX_PACKET);
	pdev->ep_out[EPOUT_ADDR & 0xFU].is_used = 1U;

	USBD_LL_OpenEP(pdev, EPIN_ADDR, USBD_EP_TYPE_BULK, MAX_PACKET);
	pdev->ep_in[EPIN_ADDR & 0xFU].is_used = 1U;

	USBD_LL_FlushEP(pdev, EPOUT_ADDR);
	USBD_LL_FlushEP(pdev, EPIN_ADDR);

	memset(receive_buffer, 0, sizeof(receive_buffer));
	USBD_LL_PrepareReceive (pdev, EPOUT_ADDR, receive_buffer, sizeof(receive_buffer));

	memset(transmit_buffer, 0, sizeof(transmit_buffer));
	USBD_LL_Transmit (pdev, EPIN_ADDR, transmit_buffer, sizeof(transmit_buffer));

	return USBD_OK;
}

static uint8_t DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	USBD_LL_CloseEP(pdev, EPOUT_ADDR);
	pdev->ep_out[EPOUT_ADDR & 0xFU].is_used = 0U;

	USBD_LL_CloseEP(pdev, EPIN_ADDR);
	pdev->ep_in[EPIN_ADDR & 0xFU].is_used = 0U;

	return USBD_OK;
}

static uint8_t DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	++transmit_buffer[0];
	USBD_LL_Transmit (pdev, EPIN_ADDR, transmit_buffer, sizeof(transmit_buffer));
	return USBD_OK; // the return-value ignored by the middleware ?!?
}


static uint8_t DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	USBD_LL_PrepareReceive (pdev, EPOUT_ADDR, receive_buffer, sizeof(receive_buffer));
	return USBD_OK; // the return-value ignored by the middleware ?!?
}


static USBD_CompatibleIDDescriptor USBD_CompatIDDesc;

static uint8_t Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	if (req->bRequest == USB_REQ_GET_OS_FEATURE_DESCRIPTOR) {
		if (req->wIndex == 0x0004) {
			USBD_CtlSendData(pdev, (uint8_t*) &USBD_CompatIDDesc,
					MIN(req->wLength, sizeof(USBD_CompatIDDesc)));
			return USBD_OK;
		}
	}

	return USBD_FAIL;
}

static uint8_t* GetString(struct _USBD_HandleTypeDef *pdev, uint8_t index, uint16_t *length)
{
	static __ALIGN_BEGIN uint8_t buffer[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

	memset(buffer, 0, sizeof(buffer));

	if (index == 0xee) {
		USBD_GetString((uint8_t *)"MSFT100", buffer, length);
		unsigned i = *length;
		buffer[i++] = USB_REQ_GET_OS_FEATURE_DESCRIPTOR;
		buffer[i++] = 0x00;		// padding
		*length = i;
		return buffer;
	}

	return NULL;
}

// ------------------------------------------------------------------------------------------------
// TODO: move to other descriptors...

static USBD_CompatibleIDDescriptor USBD_CompatIDDesc = {
	.dwLength = sizeof(USBD_CompatIDDesc),
	.bcdVersion = 0x0100,
	.wIndex = 0x0004,
	.bCount = 0x01,
	.bReserved1 = {0},
	.bFirstInterfaceNumber = 0x00,
	.bReserved2 = 0x01,
	.bCompatibleID = "WINUSB\0\0",
	.bSubCompatibleID = {0},
	.bReserved3 = {0}
};

static uint8_t configDescriptor[] __ALIGN_END = {

	0x09, /* bLength: Configuation Descriptor size */
	USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */

	0x00, /* length initialized on request */
	0x00,

	0x01, /* bNumInterfaces: 1 interface */
	0x01, /* bConfigurationValue: */
	0x00, /* iConfiguration: */
	0xC0, /* bmAttributes: */
	0x32, /* MaxPower 100 mA */

	/********************  Interface ********************/
	0x09, /* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
	0x00, /* bInterfaceNumber: Number of Interface */
	0x00, /* bAlternateSetting: Alternate setting */
	0x02, /* bNumEndpoints*/
	0xff, /* bInterfaceClass: Vendor specific */
	0xff, /* bInterfaceSubClass : vendor specific*/
	0xff, /* nInterfaceProtocol */
	0x00, /* iInterface: */

	/********************  Endpoints ********************/

	0x07, /*Endpoint descriptor length = 7*/
	USB_DESC_TYPE_ENDPOINT, /*Endpoint descriptor type */
	EPIN_ADDR, /*Endpoint address (IN, address 1) */
	0x02, /*Bulk endpoint type */
	LOBYTE(MAX_PACKET), HIBYTE(MAX_PACKET), 0x00, /*Polling interval in milliseconds */

	0x07, /*Endpoint descriptor length = 7 */
	USB_DESC_TYPE_ENDPOINT, /*Endpoint descriptor type */
	EPOUT_ADDR, /*Endpoint address (OUT, address 1) */
	0x02, /*Bulk endpoint type */
	LOBYTE(MAX_PACKET), HIBYTE(MAX_PACKET), 0x00 /*Polling interval in milliseconds*/

};

static uint8_t* GetFullSpeedConfigurationDesciptor(uint16_t *length)
{
	*length = sizeof(configDescriptor);

	configDescriptor[2] = sizeof(configDescriptor);

	return configDescriptor;
}

// ------------------------------------------------------------------------------------------------

USBD_ClassTypeDef functionHooks =
{
	.Init = Init,
	.DeInit = DeInit,
	.Setup = Setup,

	.EP0_TxSent = NULL,
	.EP0_RxReady = NULL,

	.DataIn = DataIn,
	.DataOut = DataOut,

	.SOF = NULL,
	.IsoINIncomplete = NULL,
	.IsoOUTIncomplete = NULL,

	.GetHSConfigDescriptor = NULL,
	.GetFSConfigDescriptor = GetFullSpeedConfigurationDesciptor,
	.GetOtherSpeedConfigDescriptor = NULL,
	.GetDeviceQualifierDescriptor = NULL,

	.GetUsrStrDescriptor = GetString
};
