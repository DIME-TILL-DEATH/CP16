/*
 * File:           F:\AMT\CP16\Adau_CP16\pa_IC_1_PARAM.h
 *
 * Created:        Tuesday, October 22, 2024 4:34:39 PM
 * Description:    pa:IC 1 parameter RAM definitions.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright ©2024 Analog Devices, Inc. All rights reserved.
 */
#ifndef __PA_IC_1_PARAM_H__
#define __PA_IC_1_PARAM_H__


/* Module DC1 - DC Input Entry*/
#define MOD_DC1_COUNT                                  1
#define MOD_DC1_DEVICE                                 "IC1"
#define MOD_DC1_DCINPALG1_ADDR                         0
#define MOD_DC1_DCINPALG1_FIXPT                        0x0043D70A
#define MOD_DC1_DCINPALG1_VALUE                        SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.53)
#define MOD_DC1_DCINPALG1_TYPE                         SIGMASTUDIOTYPE_FIXPOINT

#endif
