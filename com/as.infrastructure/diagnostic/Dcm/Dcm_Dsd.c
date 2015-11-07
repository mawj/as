/* -------------------------------- Arctic Core ------------------------------
 * Arctic Core - the open source AUTOSAR platform http://arccore.com
 *
 * Copyright (C) 2009  ArcCore AB <contact@arccore.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * -------------------------------- Arctic Core ------------------------------*/
/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2015  AS <parai@foxmail.com>
 */


/*
 *  General requirements
 */
/** @req DCM600 */ // Generated by BSW builder


#include <string.h>
#include "Dcm.h"
#include "Dcm_Internal.h"
#include "MemMap.h"

typedef struct {
	const PduInfoType 				*pduRxData;
	PduInfoType 					*pduTxData;
	const Dcm_DsdServiceTableType	*serviceTable;
	Dcm_ProtocolAddrTypeType		addrType;
	PduIdType 						txPduId;
	PduIdType 						rxPduId;
} MsgDataType;

// In "queue" to DSD
static boolean	dsdDslDataIndication = FALSE;
static MsgDataType msgData;

static uint8	currentSid;
static boolean	suppressPosRspMsg;

/*
 * Local functions
 */


static Std_ReturnType askApplicationForServicePermission(uint8 *requestData, uint16 dataSize)
{
	Std_ReturnType returnCode = E_OK;
	const Dcm_DslServiceRequestIndicationType *serviceRequestIndication = DCM_Config.Dsl->DslServiceRequestIndication;
	Std_ReturnType result;

	while ((!serviceRequestIndication->Arc_EOL) && (returnCode != E_REQUEST_NOT_ACCEPTED)) {
		if (serviceRequestIndication->Indication != NULL) {
			result = serviceRequestIndication->Indication(requestData, dataSize);
			if (result != E_OK){
				returnCode = result;
			}
		}
		serviceRequestIndication++;
	}

	return returnCode;
}


static void createAndSendNcr(Dcm_NegativeResponseCodeType responseCode)
{
	if (!((msgData.addrType == DCM_PROTOCOL_FUNCTIONAL_ADDR_TYPE)
		  && ((responseCode == DCM_E_SERVICENOTSUPPORTED) || (responseCode == DCM_E_SUBFUNCTIONNOTSUPPORTED) || (responseCode == DCM_E_REQUESTOUTOFRANGE)))) {   /** @req DCM001 */
		msgData.pduTxData->SduDataPtr[0] = SID_NEGATIVE_RESPONSE;
		msgData.pduTxData->SduDataPtr[1] = currentSid;
		msgData.pduTxData->SduDataPtr[2] = responseCode;
		msgData.pduTxData->SduLength = 3;
		DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_READY);	/** @req DCM114 */ /** @req DCM232.Ncr */
	}
	else {
		DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_SUPPRESSED);
	}
}


static void selectServiceFunction(uint8 sid)
{
	/** @req DCM442.Partially */
	switch (sid)	 /** @req DCM221 */
	{
#ifdef DCM_USE_SERVICE_DIAGNOSTICSESSIONCONTROL
	case SID_DIAGNOSTIC_SESSION_CONTROL:
		DspUdsDiagnosticSessionControl(msgData.pduRxData, msgData.txPduId, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_ECURESET
	case SID_ECU_RESET:
		DspUdsEcuReset(msgData.pduRxData, msgData.txPduId, msgData.pduTxData);
		break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_CLEARDIAGNOSTICINFORMATION)
	case SID_CLEAR_DIAGNOSTIC_INFORMATION:
		DspUdsClearDiagnosticInformation(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_READDTCINFORMATION)
	case SID_READ_DTC_INFORMATION:
		DspUdsReadDtcInformation(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_READDATABYIDENTIFIER
	case SID_READ_DATA_BY_IDENTIFIER:
		DspUdsReadDataByIdentifier(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
		
#ifdef DCM_USE_SERVICE_READMEMORYBYADDRESS
	case SID_READ_MEMORY_BY_ADDRESS:
		DspUdsReadMemoryByAddress(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
		
#ifdef DCM_USE_SERVICE_WRITEMEMORYBYADDRESS
	case SID_WRITE_MEMORY_BY_ADDRESS:
		DspUdsWriteMemoryByAddress(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_READSCALINGDATABYIDENTIFIER
	case SID_READ_SCALING_DATA_BY_IDENTIFIER:
		DspUdsReadScalingDataByIdentifier(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_SECURITYACCESS
	case SID_SECURITY_ACCESS:
		DspUdsSecurityAccess(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_WRITEDATABYIDENTIFIER
	case SID_WRITE_DATA_BY_IDENTIFIER:
		DspUdsWriteDataByIdentifier(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_ROUTINECONTROL
	case SID_ROUTINE_CONTROL:
		DspUdsRoutineControl(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_TESTERPRESENT
	case SID_TESTER_PRESENT:
		DspUdsTesterPresent(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_CONTROLDTCSETTING)
	case SID_CONTROL_DTC_SETTING:
		DspUdsControlDtcSetting(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_READDATABYPERIODICIDENTIFIER
	case SID_READ_DATA_BY_PERIODIC_IDENTIFIER:
		DspReadDataByPeriodicIdentifier(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
		
#ifdef DCM_USE_SERVICE_DYNAMICALLYDEFINEDATAIDENTIFIER
	case SID_DYNAMICALLY_DEFINE_DATA_IDENTIFIER:
		DspDynamicallyDefineDataIdentifier(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
		
#ifdef DCM_USE_SERVICE_INPUTOUTPUTCONTROLBYIDENTIFIER
	case SID_INPUT_OUTPUT_CONTROL_BY_IDENTIFIER:
		DspIOControlByDataIdentifier(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_COMMUNICATIONCONTROL
	case SID_COMMUNICATION_CONTROL:
		DspCommunicationControl(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
	/* OBD */
#ifdef DCM_USE_SERVICE_REQUESTCURRENTPOWERTRAINDIAGDATA
	case SID_REQUEST_CURRENT_POWERTRAIN_DIAGNOSTIC_DATA:
		DspObdRequestCurrentPowertrainDiagnosticData(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
		
#if defined(USE_DEM) && defined(DCM_USE_SERVICE_REQUESTPOWERTRAINFREEZEFRAMEDATA)
	case SID_REQUEST_POWERTRAIN_FREEZE_FRAME_DATA:
		DspObdRequsetPowertrainFreezeFrameData(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
		
#if defined(USE_DEM) && defined(DCM_USE_SERVICE_CLEAREMISSIONRELATEDDIAGNOSTICDATA)
	case SID_CLEAR_EMISSION_RELATED_DIAGNOSTIC_INFORMATION:
		DspObdClearEmissionRelatedDiagnosticData(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
		
#if defined(USE_DEM) && defined(DCM_USE_SERVICE_REQUESTEMISSIONRELATEDDTCS)
	case SID_REQUEST_EMISSION_RELATED_DIAGNOSTIC_TROUBLE_CODES:
		DspObdRequestEmissionRelatedDiagnosticTroubleCodes(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_REQUESTEMISSIONRELATEDDTCSDETECTEDDURINGCURRENTORLASCOMPLETEDDRIVINGCYCLE)
	case SID_REQUEST_EMISSION_RELATED_DIAGNOSTIC_TROUBLE_CODES_DETECTED_DURING_CURRENT_OR_LAST_COMPLETED_DRIVING_CYCLE:
		DspObdRequestEmissionRelatedDiagnosticTroubleCodesService07(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_REQUESTVEHICLEINFORMATION
	case SID_REQUEST_VEHICLE_INFORMATION:
		DspObdRequestvehicleinformation(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
	/* OBD */
#ifdef DCM_USE_SERVICE_UPLOAD_DOWNLOAD
	case SID_REQUEST_DOWNLOAD:
		DspRequestDownload(msgData.pduRxData, msgData.pduTxData);
		break;
	case SID_REQUEST_UPLOAD:
		DspRequestUpload(msgData.pduRxData, msgData.pduTxData);
		break;
	case SID_TRANSFER_DATA:
		DspTransferData(msgData.pduRxData, msgData.pduTxData);
		break;
	case SID_REQUEST_TRANSFER_EXIT:
		DspRequestTransferExit(msgData.pduRxData, msgData.pduTxData);
		break;
#endif
	default:
		/* Non implemented service */
		createAndSendNcr(DCM_E_SERVICENOTSUPPORTED);
		break;
	}
}


static boolean lookupSid(uint8 sid, const Dcm_DsdServiceType **sidPtr)
{
	boolean returnStatus = TRUE;
	const Dcm_DsdServiceType *service = msgData.serviceTable->DsdService;

	while ((service->DsdSidTabServiceId != sid) && (!service->Arc_EOL)) {
		service++;
	}

	if (!service->Arc_EOL) {
		*sidPtr = service;
	}
	else {
		returnStatus = FALSE;
		*sidPtr = NULL;
	}

	return returnStatus;
}


/*
 * Exported functions
 */

void DsdInit(void)
{

}


void DsdMain(void)
{
	if (dsdDslDataIndication) {
		dsdDslDataIndication = FALSE;
		DsdHandleRequest();
	}
}


void DsdHandleRequest(void)
{
	Std_ReturnType result;
	const Dcm_DsdServiceType *sidConfPtr = NULL;

	currentSid = msgData.pduRxData->SduDataPtr[0];	/** @req DCM198 */

	/** @req DCM178 */
	//lint --e(506, 774)	PC-Lint exception Misra 13.7, 14.1 Allow configuration variables in boolean expression
	if ((DCM_RESPOND_ALL_REQUEST == STD_ON) || ((currentSid & 0x7Fu) < 0x40)) {		/** @req DCM084 */
		if (lookupSid(currentSid, &sidConfPtr)) {		/** @req DCM192 */ /** @req DCM193 */ /** @req DCM196 */
			// SID found!
			if (DspCheckSessionLevel(sidConfPtr->DsdSidTabSessionLevelRef)) {		 /** @req DCM211 */
				if (DspCheckSecurityLevel(sidConfPtr->DsdSidTabSecurityLevelRef)) {	 /** @req DCM217 */
					//lint --e(506, 774)	PC-Lint exception Misra 13.7, 14.1 Allow configuration variables in boolean expression
					if (DCM_REQUEST_INDICATION_ENABLED == STD_ON) {	 /** @req DCM218 */
						 result = askApplicationForServicePermission(msgData.pduRxData->SduDataPtr, msgData.pduRxData->SduLength);
					} else {
						result = E_OK;
					}
					//lint --e(506, 774)	PC-Lint exception Misra 13.7, 14.1 Allow configuration variables in boolean expression
					if (result == E_OK) {
						// Yes! All conditions met!
						// Check if response shall be suppressed
						if ( (sidConfPtr->DsdSidTabSubfuncAvail) && (msgData.pduRxData->SduDataPtr[1] & SUPPRESS_POS_RESP_BIT) ) {	/** @req DCM204 */
							suppressPosRspMsg = TRUE;	/** @req DCM202 */
							msgData.pduRxData->SduDataPtr[1] &= ~SUPPRESS_POS_RESP_BIT;	/** @req DCM201 */
						}
						else
						{
							suppressPosRspMsg = FALSE;	/** @req DCM202 */
						}
						selectServiceFunction(currentSid);
					}
					else {
						if (result == E_REQUEST_ENV_NOK) {
							createAndSendNcr(DCM_E_CONDITIONSNOTCORRECT);	/** @req DCM463 */
						}
						else {
							// Do not send any response		/** @req DCM462 */
							DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_SUPPRESSED);
						}
					}
				}
				else {
					createAndSendNcr(DCM_E_SECUTITYACCESSDENIED);	/** @req DCM217 */
				}
			}
			else {
				createAndSendNcr(DCM_E_SERVICENOTSUPPORTEDINACTIVESESSION);	/** @req DCM211 */
			}
		}
		else {
			createAndSendNcr(DCM_E_SERVICENOTSUPPORTED);	/** @req DCM197 */
		}
	}
	else {
		// Inform DSL that message has been discard
		DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_SUPPRESSED);
	}
}



void DsdDspProcessingDone(Dcm_NegativeResponseCodeType responseCode)
{
	if (responseCode == DCM_E_POSITIVERESPONSE) {
		if (!suppressPosRspMsg) {	/** @req DCM200 */ /** @req DCM231 */
			/** @req DCM222 */
			msgData.pduTxData->SduDataPtr[0] = currentSid | SID_RESPONSE_BIT;	/** @req DCM223 */ /** @req DCM224 */
			DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_READY);	/** @req DCM114 */ /** @req DCM225 */ /** @req DCM232.Ok */
		}
		else {
			DspDcmConfirmation(msgData.txPduId);	/** @req DCM236 */ /** @req DCM240 */
			DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_SUPPRESSED);
		}
	}
	else {
		createAndSendNcr(responseCode);	/** @req DCM228 */
	}

}


void DsdDataConfirmation(PduIdType confirmPduId, NotifResultType result)
{
	(void)result;	/* Currently not used */
	DspDcmConfirmation(confirmPduId);	/** @req DCM236 */
}


void DsdDslDataIndication(const PduInfoType *pduRxData, const Dcm_DsdServiceTableType *protocolSIDTable, Dcm_ProtocolAddrTypeType addrType, PduIdType txPduId, PduInfoType *pduTxData, PduIdType rxContextPduId)
{
	msgData.rxPduId = rxContextPduId;
	msgData.txPduId = txPduId;
	msgData.pduRxData = pduRxData;
	msgData.pduTxData = pduTxData;
	msgData.addrType = addrType;
	msgData.serviceTable = protocolSIDTable;

	dsdDslDataIndication = TRUE;
}

//OBD: called by DSL to get the current Tx PduId
PduIdType DsdDslGetCurrentTxPduId(void)
{
    return msgData.txPduId;
}
