/*
 *  Copyright (c) 2016 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be
 *  found  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP10_COMMON_WARPED_MOTION_H
#define VP10_COMMON_WARPED_MOTION_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <assert.h>

#include "./vpx_config.h"
#include "vpx_ports/mem.h"
#include "vpx_dsp/vpx_dsp_common.h"

// Bits of precision used for the model
#define WARPEDMODEL_PREC_BITS 8
#define WARPEDMODEL_ROW3HOMO_PREC_BITS 12

// Bits of subpel precision for warped interpolation
#define WARPEDPIXEL_PREC_BITS 6
#define WARPEDPIXEL_PREC_SHIFTS (1 << WARPEDPIXEL_PREC_BITS)

// Taps for ntap filter
#define WARPEDPIXEL_FILTER_TAPS 6

// Precision of filter taps
#define WARPEDPIXEL_FILTER_BITS 7

#define WARPEDDIFF_PREC_BITS (WARPEDMODEL_PREC_BITS - WARPEDPIXEL_PREC_BITS)

typedef void (*ProjectPointsType)(int *mat, int *points, int *proj, const int n,
                                  const int stride_points,
                                  const int stride_proj,
                                  const int subsampling_x,
                                  const int subsampling_y);
void projectPointsHomography(int *mat, int *points, int *proj, const int n,
                             const int stride_points, const int stride_proj,
                             const int subsampling_x, const int subsampling_y);
void projectPointsAffine(int *mat, int *points, int *proj, const int n,
                         const int stride_points, const int stride_proj,
                         const int subsampling_x, const int subsampling_y);
void projectPointsRotZoom(int *mat, int *points, int *proj, const int n,
                          const int stride_points, const int stride_proj,
                          const int subsampling_x, const int subsampling_y);
void projectPointsTranslation(int *mat, int *points, int *proj, const int n,
                              const int stride_points, const int stride_proj,
                              const int subsampling_x, const int subsampling_y);

typedef enum {
  UNKNOWN_TRANSFORM = -1,
  HOMOGRAPHY,   // homography, 8-parameter
  AFFINE,       // affine, 6-parameter
  ROTZOOM,      // simplified affine with rotation and zoom only, 4-parameter
  TRANSLATION,  // translational motion 2-parameter
  TRANS_TYPES
} TransformationType;

// number of parameters used by each transformation in TransformationTypes
static const int n_trans_model_params[TRANS_TYPES] = { 9, 6, 4, 2 };

typedef struct {
  TransformationType wmtype;
  int wmmat[8];  // For homography wmmat[9] is assumed to be 1
} WarpedMotionParams;

double vp10_warp_erroradv(WarpedMotionParams *wm,
#if CONFIG_VP9_HIGHBITDEPTH
                          int use_hbd, int bd,
#endif  // CONFIG_VP9_HIGHBITDEPTH
                          uint8_t *ref, int width, int height, int stride,
                          uint8_t *dst, int p_col, int p_row, int p_width,
                          int p_height, int p_stride, int subsampling_x,
                          int subsampling_y, int x_scale, int y_scale);

void vp10_warp_plane(WarpedMotionParams *wm,
#if CONFIG_VP9_HIGHBITDEPTH
                     int use_hbd, int bd,
#endif  // CONFIG_VP9_HIGHBITDEPTH
                     uint8_t *ref, int width, int height, int stride,
                     uint8_t *pred, int p_col, int p_row, int p_width,
                     int p_height, int p_stride, int subsampling_x,
                     int subsampling_y, int x_scale, int y_scale);

// Integerize model into the WarpedMotionParams structure
void vp10_integerize_model(const double *model, TransformationType wmtype,
                           WarpedMotionParams *wm);
#endif  // VP10_COMMON_WARPED_MOTION_H
