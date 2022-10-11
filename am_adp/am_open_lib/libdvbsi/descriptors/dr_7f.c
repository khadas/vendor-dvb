#ifdef _FORTIFY_SOURCE
#undef _FORTIFY_SOURCE
#endif
/***************************************************************************
 * Copyright (C) 2017 Amlogic, Inc. All rights reserved.
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
/**\file  dr_7f.c
 * \brief AMLogic descriptor 7f parse
 *
 * \author chen hua ling <hualing.chen@amlogic.com>
 * \date 2017-03-16: create the document
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

#include "dr_7f.h"


/*****************************************************************************
 * dvbpsi_Decode_Exten_Sup_Audio_Dr
 *****************************************************************************/
int dvbpsi_Decode_Exten_Sup_Audio_Dr(dvbpsi_EXTENTION_dr_t * p_decoded, uint8_t * p_data, uint8_t i_length)
{
    AM_DEBUG(1, "dr_7f dvbpsi_Decode_Exten_Sup_Audio_Dr ");
  /*1 5 1 1 8bit mix:1 edit:5 re:1 lang:1*/
  p_decoded->exten_t.sup_audio.mix_type = (p_data[1]&0x80)>>7; /*1000 0000*/
  p_decoded->exten_t.sup_audio.editorial_classification = (p_data[1]&0x7C)>>2; /*0111 1100*/
  p_decoded->exten_t.sup_audio.lang_code = p_data[1]&0x01; /*0000 0001*/
  if (p_decoded->exten_t.sup_audio.lang_code == 1 && i_length >= 5) {
    p_decoded->exten_t.sup_audio.iso_639_lang_code[0] = p_data[2];
    p_decoded->exten_t.sup_audio.iso_639_lang_code[1] = p_data[3];
    p_decoded->exten_t.sup_audio.iso_639_lang_code[2] = p_data[4];
  }
  /* Convert to lower case if possible */
  dvbpsi_ToLower(p_decoded->exten_t.sup_audio.iso_639_lang_code, 3);
  AM_DEBUG(1, "dr_7f dvbpsi_Decode_Exten_Sup_Audio_Dr: mix:%d edit:%d lang:%d",p_decoded->exten_t.sup_audio.mix_type,p_decoded->exten_t.sup_audio.editorial_classification,p_decoded->exten_t.sup_audio.lang_code );
  return 0;
}

/*****************************************************************************
 * dvbpsi_Decode_Exten_AC4_Audio_Dr
 *****************************************************************************/
int dvbpsi_Decode_Exten_AC4_Audio_Dr(dvbpsi_EXTENTION_dr_t * p_decoded, uint8_t * p_data, uint8_t i_length)
{
	AM_DEBUG(1, "dr_7f dvbpsi_Decode_Exten_AC4_Audio_Dr ");
	/*1 5 1 1 8bit mix:1 edit:5 re:1 lang:1*/
	p_decoded->exten_t.ac4_audio.ac4_config_flag = p_data[1]; /*1000 0000*/
	p_decoded->exten_t.ac4_audio.ac4_toc_flag = p_data[1]; /*0111 1100*/
	return 0;
}

/*****************************************************************************
 * dvbpsi_Decode_Exten_AC4_Audio_Dr
 *****************************************************************************/
int dvbpsi_Decode_Exten_Audio_Preselection_Dr(dvbpsi_EXTENTION_dr_t * p_decoded, uint8_t * p_data, uint8_t i_length)
{
	AM_DEBUG(1, "dr_7f dvbpsi_Decode_Exten_Audio_Preselection_Dr ");
	dvbpsi_EXTENTION_audio_preselection_t* ap = &p_decoded->exten_t.audio_preselection;
	ap->num_preselections = (p_data[1]&0xF8)>>3; /*1111 1000*/
	ap->reserved_zero_future_use = (p_data[1]&0x07); /*0000 0111*/
	if (ap->num_preselections <= 0)
	{
		return 0;
	}
	uint8_t* data = p_data+2;
	// TV-64939 mentions the use case of multi_stream_info_present.
	// It requires to skip the audio track which multi_stream_info_present is TRUE.
	// num_skipped_presel here is used to count number of preselections to be skipped
	// along this line.
	int num_skipped_presel = 0;
	for (int i=0;i<ap->num_preselections;i++)
	{
		// Considering the skipped preselection item, here is 'i-num_skipped_presel'
		dvbpsi_EXTENTION_preselection_t* ps_i = &ap->preselections[i-num_skipped_presel];
		int extra=0;
		ps_i->preselection_id = (data[0]&0xF8)>>3;
		ps_i->audio_rendering_indication = (data[0]&0x07);
		ps_i->audio_description = (data[1]&0x80)>>7;
		ps_i->spoken_subtitle = (data[1]&0x40)>>6;
		ps_i->dialogue_enhancement = (data[1]&0x20)>>5;
		ps_i->interactivity_enabled = (data[1]&0x10)>>4;
		ps_i->language_code_present = (data[1]&0x08)>>3;
		ps_i->text_label_present = (data[1]&0x04)>>2;
		ps_i->multi_stream_info_present = (data[1]&0x02)>>1;
		ps_i->future_extension = (data[1]&0x01);
		if (ps_i->language_code_present)
		{
			strncpy(ps_i->iso_639_language_code,data+2,3);
			extra+=3;
		}
		ps_i->message_id=0;
		if (ps_i->text_label_present)
		{
			ps_i->message_id = data[2+extra];
			extra+=1;
		}
		if (ps_i->multi_stream_info_present)
		{
			int num_aux_components = data[2+extra]>>5;
			extra+=num_aux_components+1;
			num_skipped_presel++;
		}
		if (ps_i->future_extension)
		{
			int future_extension_length = data[2+extra];
			extra+=future_extension_length+1;
		}
		AM_DEBUG(1, "dr_7f data:%p, preselection id:%d, lang:%c%c%c, msg_id:%d",
				data,(int)(ps_i->preselection_id),
				ps_i->iso_639_language_code[0], ps_i->iso_639_language_code[1],
				ps_i->iso_639_language_code[2], (int)(ps_i->message_id));
		data += (2+extra);
	}
	// num_preselections needs to be revised
	ap->num_preselections -= num_skipped_presel;
	return 0;
}

/*****************************************************************************
 * dvbpsi_DecodeEXTENTIONDr
 *****************************************************************************/
dvbpsi_EXTENTION_dr_t * dvbpsi_DecodeEXTENTIONDr(dvbpsi_descriptor_t * p_descriptor)
{
    dvbpsi_EXTENTION_dr_t * p_decoded;
    AM_DEBUG(1, "dr_7f dvbpsi_DecodeEXTENTIONDr ");
  /* Check the tag */
  if (p_descriptor->i_tag != 0x7f)
  {
    DVBPSI_ERROR_ARG("dr_7f decoder", "bad tag (0x%x)", p_descriptor->i_tag);
    AM_DEBUG(1, "dr_7f decoder bad tag (0x%x)",p_descriptor->i_tag);
    return NULL;
  }

  /* Don't decode twice */
  if (p_descriptor->p_decoded)
    return p_descriptor->p_decoded;

  /* Allocate memory */
  p_decoded =
        (dvbpsi_EXTENTION_dr_t *)malloc(sizeof(dvbpsi_EXTENTION_dr_t));
  if (!p_decoded)
  {
    DVBPSI_ERROR("dr_7f decoder", "out of memory");
    AM_DEBUG(1, "dr_7f decoder out of memory");
    return NULL;
  }

  /* Decode data and check the length */
  p_decoded->i_extern_des_tag = (p_descriptor->p_data[0]);

  switch (p_decoded->i_extern_des_tag)
  {
    case AM_SI_EXTEN_DESCR_IMAGE_ICON:
      AM_DEBUG(1, "dr_7f exten tag DESCR_IMAGE_ICON ");
      break;
    case AM_SI_EXTEN_DESCR_CPCM_DELIVERY_SIGNAL:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_CPCM_DELIVERY_SIGNAL ");
      break;
    case AM_SI_EXTEN_DESCR_CP:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_CP ");
      break;
    case AM_SI_EXTEN_DESCR_CP_IDENTIFITER:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_CP_IDENTIFITER ");
      break;
    case AM_SI_EXTEN_DESCR_T2_DELIVERY_SYS:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_T2_DELIVERY_SYS ");
      break;
    case AM_SI_EXTEN_DESCR_SH_DELIVERY_SYS:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_SH_DELIVERY_SYS ");
      break;
    case AM_SI_EXTEN_DESCR_SUP_AUDIO:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_SUP_AUDIO ");
      dvbpsi_Decode_Exten_Sup_Audio_Dr(p_decoded, p_descriptor->p_data, p_descriptor->i_length);
      break;
    case AM_SI_EXTEN_DESCR_NETWORK_CHANGE_NOTIFY:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_NETWORK_CHANGE_NOTIFY ");
      break;
    case AM_SI_EXTEN_DESCR_MESSAGE:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_MESSAGE ");
      break;
    case AM_SI_EXTEN_DESCR_TARGET_REGION:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_TARGET_REGION ");
      break;
    case AM_SI_EXTEN_DESCR_TARGET_REGION_NAME:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_TARGET_REGION_NAME ");
      break;
    case AM_SI_EXTEN_DESCR_SERVICE_RELOCATED:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_SERVICE_RELOCATED ");
      break;
    case AM_SI_EXTEN_DESCR_XAIT_PID:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_XAIT_PID ");
      break;
    case AM_SI_EXTEN_DESCR_C2_DELIVERY:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_C2_DELIVERY ");
      break;
    case AM_SI_EXTEN_DESCR_DTSHD_AUDIO:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_DTSHD_AUDIO ");
      break;
    case AM_SI_EXTEN_DESCR_DTS_NEURAL:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_DTS_NEURAL ");
      break;
    case AM_SI_EXTEN_DESCR_VIDEO_DEPTH_RANGE:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_VIDEO_DEPTH_RANGE ");
      break;
    case AM_SI_EXTEN_DESCR_T2MI:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_T2MI ");
      break;
    case AM_SI_EXTEN_DESCR_URI_LINKAGE:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_URI_LINKAGE ");
      break;
    case AM_SI_EXTEN_DESCR_BCI_ANCILLARY:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_BCI_ANCILLARY ");
      break;
    case AM_SI_EXTEN_DESCR_AC4:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_AC4 ");
      dvbpsi_Decode_Exten_AC4_Audio_Dr(p_decoded, p_descriptor->p_data, p_descriptor->i_length);
      break;
    case AM_SI_EXTEN_DESCR_AUDIO_PRESELECTION:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_AUDIO_PRESELECTION ");
      dvbpsi_Decode_Exten_Audio_Preselection_Dr(
          p_decoded, p_descriptor->p_data, p_descriptor->i_length);
      break;
    case AM_SI_EXTEN_DESCR_OTHER:
      AM_DEBUG(1, "dr_7f exten tag AM_SI_EXTEN_DESCR_OTHER ");
      break;
    default:
      AM_DEBUG(1, "Scan: Unkown exten tag data, tag 0x%x", p_descriptor->p_data[0]);
      p_decoded->i_extern_des_tag = AM_SI_EXTEN_DESCR_OTHER;
      break;
  }

  p_descriptor->p_decoded = (void *)p_decoded;
  return p_decoded;
}
