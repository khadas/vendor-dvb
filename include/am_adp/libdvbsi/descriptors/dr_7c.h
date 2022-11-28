/*
* Copyright (c) 2022 Amlogic, Inc. All rights reserved.
*
* This source code is subject to the terms and conditions defined in the
* file 'LICENSE' which is part of this source code package. *
* Description:
*/
/*****************************************************************************
 * dr_7c.h
 * (c)2022 amlogic
 *
 *****************************************************************************/

/*!
 * \file <dr_7c.h>
 * \author Wentao.MA <wentao.ma@amlogic.com>
 * \brief Application interface for the AAC descriptor decoder and generator.
 *
 * Application interface for the MPEG 2 TS AAC descriptor decoder and generator
 * This descriptor's definition can be found in ETSC EN 300 468
 */

#ifndef _DVBPSI_DR_7C_H_
#define _DVBPSI_DR_7C_H_

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * dvbpsi_AAC_dr_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_AAC_dr_t
 * \brief AAC descriptor structure.
 *
 * This structure is used to store a decoded AAC descriptor.
 * (ETSI EN 300 468).
 */
/*!
 * \typedef struct dvbpsi_AAC_dr_s dvbpsi_AAC_dr_t
 * \brief dvbpsi_AAC_dr_t type definition.
 */
typedef struct dvbpsi_AAC_dr_s
{
	uint8_t   profile_and_level;
	uint8_t   AAC_type_flag;
	uint8_t   SAOC_DE_flag;
	uint8_t   AAC_type;
} dvbpsi_AAC_dr_t;

/*****************************************************************************
 * dvbpsi_DecodeAACDr
 *****************************************************************************/
/*!
 * \fn dvbpsi_AAC_dr_t * dvbpsi_DecodeAACDr(
                                        dvbpsi_descriptor_t * p_descriptor)
 * \brief AAC descriptor decoder.
 * \param p_descriptor pointer to the descriptor structure
 * \return a pointer to a new AAC descriptor structure which
 * contains the decoded data.
 */
dvbpsi_AAC_dr_t* dvbpsi_DecodeAACDr(dvbpsi_descriptor_t * p_descriptor);


#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of dr_7c.h"
#endif

