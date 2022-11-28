#ifdef _FORTIFY_SOURCE
#undef _FORTIFY_SOURCE
#endif
/***************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * Description:
 */
/**\file  dr_7c.c
 * \brief AMLogic descriptor 7c parse
 *
 * \author Wentao.MA <wentao.ma@amlogic.com>
 * \date 2022-11-28
 ***************************************************************************/


#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#include <am_debug.h>
#include "../dvbpsi.h"
#include "../dvbpsi_private.h"
#include "../descriptor.h"

#include "dr_7c.h"

/*****************************************************************************
 * dvbpsi_DecodeAACDr
 *****************************************************************************/
dvbpsi_AAC_dr_t * dvbpsi_DecodeAACDr(dvbpsi_descriptor_t * p_descriptor)
{
  dvbpsi_AAC_dr_t * p_decoded;
  AM_DEBUG(1, "dr_7c dvbpsi_DecodeAACDr ");
  /* Check the tag */
  if (p_descriptor->i_tag != 0x7c)
  {
    DVBPSI_ERROR_ARG("dr_7c decoder", "bad tag (0x%x)", p_descriptor->i_tag);
    AM_DEBUG(1, "dr_7c decoder bad tag (0x%x)",p_descriptor->i_tag);
    return NULL;
  }

  /* Don't decode twice */
  if (p_descriptor->p_decoded)
    return p_descriptor->p_decoded;

  /* Allocate memory */
  p_decoded =
        (dvbpsi_AAC_dr_t *)malloc(sizeof(dvbpsi_AAC_dr_t));
  if (!p_decoded)
  {
    DVBPSI_ERROR("dr_7c decoder", "out of memory");
    AM_DEBUG(1, "dr_7c decoder out of memory");
    return NULL;
  }
  memset(p_decoded, 0, sizeof(dvbpsi_AAC_dr_t));
  /* Decode data and check the length */
  p_decoded->profile_and_level = p_descriptor->p_data[0];
  if (p_descriptor->i_length > 1)
  {
    p_decoded->AAC_type_flag = (p_descriptor->p_data[1] >> 7);
    p_decoded->SAOC_DE_flag = ((p_descriptor->p_data[1] & 0x40) >> 6);
    if (p_decoded->AAC_type_flag > 0)
    {
      p_decoded->AAC_type = p_descriptor->p_data[2];
    }
  }

  p_descriptor->p_decoded = (void *)p_decoded;

  AM_DEBUG(1, "AAC descriptor, tag:0x%x, length:%d, profile_and_level:%d,"
      " AAC_type_flag:%d, SAOC_DE_flag:%d, AAC_type:%d",p_descriptor->i_tag,
      p_descriptor->i_length, p_decoded->profile_and_level,
      p_decoded->AAC_type_flag, p_decoded->SAOC_DE_flag, p_decoded->AAC_type);

  return p_decoded;
}

