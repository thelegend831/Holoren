/*
 * Copyright (C) 2013 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of Holoren.
 *
 * Holoren is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Holoren is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Holoren.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * This file contains all the OpenCL kernels used by program
 */

/**
 * Algorithm 1
 *
 * A kernel to compute spherical wave equation of a single point in optical field.
 * This kernel is meant to be used with `renderAlgorithm_SinglePass`.
 */
__kernel void compObjWave_SinglePass(__global float *pc,    /// a point cloud with point sources
                                     uint pc_size,          /// the number of elements in point cloud
                                     __global float2 *of,   /// an output optical field (float2 because this is an array of complex numbers)
                                     float hologram_z,      /// the z depth of hologram
                                     float k,               /// wavenumber
                                     float pitch,           /// pitch between samples
                                     float corner_x,        /// x corner of optical field
                                     float corner_y)        /// y corner of optical field
{
  int row = get_global_id(0);
  int col = get_global_id(1);

  float2 sample = (float2) (
    0.0f,
    0.0f
  );

  float4 dist = (float4) (
    ((col - 1) * pitch) + corner_x,
    ((row - 1) * pitch) + corner_y,
    hologram_z,
    0.0f
  );
  
  for (uint i = 0; i < pc_size; ++i)
  {
    float4 ps = (float4) (pc[i * 3], pc[i * 3 + 1], pc[i * 3 + 2], 0.0f);    // TODO: optimize this
                                                                             // maybe by passing a float4 aligned array to kernel

    ps = dist - ps;
    ps.w = sqrt(ps.x * ps.x + ps.y * ps.y + ps.z * ps.z) * k;

    sample.s0 += cos(ps.w);
    sample.s1 += sin(ps.w);
  }

  of[row * get_global_size(1) + col] = sample;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Algorithm 3
 *
 * A kernel to compute spherical wave equation of a single point in optical field.
 * This method is meant to be used with multipass algorithm.
 * It does not use any special optimisations.
 */
__kernel void compObjWave_MultiPass(__global float *pc,    /// a point cloud with point sources
                                    uint pc_size,          /// the number of elements in point cloud
                                    __global float2 *of,   /// an output optical field (float2 because this is an array of complex numbers)
                                    int offset,            /// offset in the resulting optical field
                                    int num_cols,          /// number of pixels in a column of optical field
                                    float hologram_z,      /// the z depth of hologram
                                    float k,               /// wavenumber
                                    float pitch,           /// pitch between samples
                                    float corner_x,        /// x corner of optical field
                                    float corner_y)        /// y corner of optical field
{
  size_t gid = get_global_id(0);
  int row = (gid + offset) / num_cols;
  int col = (gid + offset) - (row * num_cols);

  /* cumulative sum of complex amplitudes from point sources */
  float2 sample = (float2) (
    0.0f,
    0.0f
  );

  /* target position on the optical field */
  float4 of_pos = (float4) (
    ((col - 1) * pitch) + corner_x,
    ((row - 1) * pitch) + corner_y,
    hologram_z,
    0.0f
  );
  
  /* loop through the point sources and sum complex amplitudes
     from each point source to the target location in optical field  */
  for (uint i = 0; i < pc_size; ++i)
  {
    float4 ps = (float4) (pc[i * 3], pc[i * 3 + 1], pc[i * 3 + 2], 0.0f);  // TODO: optimize this
                                                                           // maybe by passing a float4 aligned array to kernel
    ps = of_pos - ps;
    ps.w = sqrt(ps.x * ps.x + ps.y * ps.y + ps.z * ps.z) * k;

    sample.s0 += cos(ps.w);
    sample.s1 += sin(ps.w);
  }

  // put the result to a correct place in global memory
  of[gid] = sample;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Algorithm 4
 *
 * A kernel to compute spherical wave equation of a single point in optical field.
 * This method is meant to be used with multipass algorithm.
 * It exploits native_* instructions to boost its performance.
 */
__kernel void compObjWave_MultiPass_native(__global float *pc,    /// a point cloud with point sources
                                           uint pc_size,          /// the number of elements in point cloud
                                           __global float2 *of,   /// an output optical field (float2 because this is an array of complex numbers)
                                           int offset,            /// offset in the resulting optical field
                                           int num_cols,          /// number of pixels in a column of optical field
                                           float hologram_z,      /// the z depth of hologram
                                           float k,               /// wavenumber
                                           float pitch,           /// pitch between samples
                                           float corner_x,        /// x corner of optical field
                                           float corner_y)        /// y corner of optical field
{
  size_t gid = get_global_id(0);
  int row = (gid + offset) / num_cols;
  //int row = native_divide((gid + offset), num_cols);
  int col = (gid + offset) - (row * num_cols);

  /* cumulative sum of complex amplitudes from point sources */
  float2 sample = (float2) (
    0.0f,
    0.0f
  );

  /* target position on the optical field */
  float4 of_pos = (float4) (
    ((col - 1) * pitch) + corner_x,
    ((row - 1) * pitch) + corner_y,
    hologram_z,
    0.0f
  );
  
  /* loop through the point sources and sum complex amplitudes
     from each point source to the target location in optical field  */
  for (uint i = 0; i < pc_size; ++i)
  {
    float4 ps = (float4) (pc[i * 3], pc[i * 3 + 1], pc[i * 3 + 2], 0.0f);  // TODO: optimize this
                                                                           // maybe by passing a float4 aligned array to kernel
    ps = of_pos - ps;
    ps.w = native_sqrt(ps.x * ps.x + ps.y * ps.y + ps.z * ps.z) * k;

    sample.s0 += native_cos(ps.w);
    sample.s1 += native_sin(ps.w);
  }

  // put the result to a correct place in global memory
  of[gid] = sample;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Algorithm 5
 *
 * A kernel to compute spherical wave equation of a single point in optical field.
 * This function is meant to be used in a multipass algorithm.
 * It is meant to handle point clouds that are aligned to float4 instead of float3.
 */
__kernel void compObjWave_MultiPass_aligned(__global float4 *pc,   /// a point cloud with point sources
                                            uint pc_size,          /// the number of elements in point cloud
                                            __global float2 *of,   /// an output optical field (float2 because this is an array of complex numbers)
                                            int offset,            /// offset in the resulting optical field
                                            int num_cols,          /// number of pixels in a column of optical field
                                            float hologram_z,      /// the z depth of hologram
                                            float k,               /// wavenumber
                                            float pitch,           /// pitch between samples
                                            float corner_x,        /// x corner of optical field
                                            float corner_y)        /// y corner of optical field
{
  size_t gid = get_global_id(0);
  int row = (gid + offset) / num_cols;
  int col = (gid + offset) - (row * num_cols);

  float2 sample = (float2) (
    0.0f,
    0.0f
  );

  float4 dist = (float4) (
    ((col - 1) * pitch) + corner_x,
    ((row - 1) * pitch) + corner_y,
    hologram_z,
    0.0f
  );
  
  for (uint i = 0; i < pc_size; ++i)
  {
    float4 ps = (float4) (pc[i]);
    ps = dist - ps;
    ps.w = sqrt(ps.x * ps.x + ps.y * ps.y + ps.z * ps.z) * k;

    sample.s0 += cos(ps.w);
    sample.s1 += sin(ps.w);
  }

  // put the result to a correct place in global memory
  of[gid] = sample;
}
