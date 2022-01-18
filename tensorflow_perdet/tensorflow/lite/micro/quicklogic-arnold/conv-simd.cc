/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/


#include "tensorflow/lite/kernels/internal/reference/conv.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/integer_ops/conv.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/padding.h"

#include "accel.h"

#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/internal/common.h"

#include "arnold_apb_ctl.h"
extern "C" int oPrintf(const char* format, ...);

namespace tflite {

//namespace reference_ops {


void ConvSW_SIMD(const ConvParams& params, 
                  const RuntimeShape& input_shape, const uint8* input_data, 
                  const RuntimeShape& filter_shape, const int8_t* filter_data, 
                  const RuntimeShape& bias_shape, const int32* bias_data, 
                  const RuntimeShape& output_shape, uint8* output_data, 
                  const RuntimeShape& im2col_shape, uint8* im2col_data, 
                  void* cpu_backend_context,
                  bool fPrint) {
	int32_t iprintcol;
  if (fPrint)
    oPrintf("ConvSW_SIMD\n");
  

  if (fPrint) {
    iprintcol = 0;
    oPrintf("int32_t axOutput[] = {\n  ");
  }
 
  (void)cpu_backend_context;  // only used in optimized code.
  (void)im2col_data;   // only used in optimized code.
  (void)im2col_shape;  // only used in optimized code.
  const int stride_width = params.stride_width;
  const int stride_height = params.stride_height;
  const int dilation_width_factor = params.dilation_width_factor;
  const int dilation_height_factor = params.dilation_height_factor;
  const int pad_width = params.padding_values.width;
  const int pad_height = params.padding_values.height;
  const int32 input_offset = params.input_offset;
  const int32 filter_offset = params.weights_offset;
  const int32 output_offset = params.output_offset;
  const int32 output_multiplier = params.output_multiplier;
  const int output_shift = params.output_shift;
  const int32 output_activation_min = params.quantized_activation_min;
  const int32 output_activation_max = params.quantized_activation_max;
  TFLITE_DCHECK_LE(output_activation_min, output_activation_max);

  TFLITE_DCHECK_EQ(input_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(filter_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(output_shape.DimensionsCount(), 4);
  oPrintf("input_data = %x output_data = %x filter_data = %x\n", input_data, output_data, filter_data);


  oPrintf("input_shape\n");
  for( int cnt = 0; cnt < input_shape.DimensionsCount(); cnt++ )
  {
	  oPrintf("%d/%d %d %x\n",cnt, input_shape.DimensionsCount(), input_shape.Dims(cnt), input_shape.DimsData());
  }

  oPrintf("output_shape\n");
  for( int cnt = 0; cnt < output_shape.DimensionsCount(); cnt++ )
  {
	  oPrintf("%d/%d %d %x\n",cnt, output_shape.DimensionsCount(), output_shape.Dims(cnt), output_shape.DimsData());
  }

  oPrintf("filter_shape\n");
  for( int cnt = 0; cnt < filter_shape.DimensionsCount(); cnt++ )
  {
	  oPrintf("%d/%d %d %x\n",cnt, filter_shape.DimensionsCount(), filter_shape.Dims(cnt), filter_shape.DimsData());
  }

  oPrintf("const int batches");
  const int batches = MatchingDim(input_shape, 0, output_shape, 0);
  oPrintf("batches = %d\n", batches);

  oPrintf("const int input_depth");
  const int input_depth = MatchingDim(input_shape, 3, filter_shape, 3);
  oPrintf("input_depth = %d\n",input_depth);

  oPrintf("const int output_depth");
  const int output_depth = MatchingDim(filter_shape, 0, output_shape, 3);
  oPrintf("output_depth = %d\n",output_depth);

  if (bias_data) {
    TFLITE_DCHECK_EQ(bias_shape.FlatSize(), output_depth);
  }
  const int input_height = input_shape.Dims(1);
  const int input_width = input_shape.Dims(2);
  const int filter_height = filter_shape.Dims(1);
  const int filter_width = filter_shape.Dims(2);
  const int output_height = output_shape.Dims(1);
  const int output_width = output_shape.Dims(2);
  
  int first = 0;

  int32 acc_orig;
  int32 acc_new;
  
  const int simd = {8};
  if (fPrint) {
    oPrintf("batches=%d\n", batches);
    oPrintf("output_depth=%d\n", output_depth);
  }
  //gpio->set31_00 = (1 << 5); // gpio 5 set for timing
  
  for (int batch = 0; batch < batches; ++batch) {
    
      for (int out_y = 0; out_y < output_height; ++out_y) {
        for (int out_x = 0; out_x < output_width; ++out_x) {
          for (int out_channel = 0; out_channel < output_depth; ++out_channel) {
            for (int isimd = 0; isimd != simd; isimd++) {
              const int in_x_origin = (out_x * stride_width) - pad_width;
              const int in_y_origin = (out_y * stride_height) - pad_height;
              int32_t acc = 0;
              for (int filter_y = 0; filter_y < filter_height; ++filter_y) {
                for (int filter_x = 0; filter_x < filter_width; ++filter_x) {
                  for (int in_channel = 0; in_channel < input_depth; ++in_channel) {
                    const int in_x = in_x_origin + dilation_width_factor * filter_x;
                    const int in_y =
                        in_y_origin + dilation_height_factor * filter_y;
                    // If the location is outside the bounds of the input image,
                    // use zero as a default value.
                    if ((in_x >= 0) && (in_x < input_width) && (in_y >= 0) &&
                        (in_y < input_height)) {
                      int32_t ifilt = (((out_channel * input_depth) + in_channel) * simd) + isimd;
                      int32_t filter_val = filter_data[ifilt];
                      int32_t input_val = input_data[Offset(input_shape, batch, in_y,
                                                          in_x, in_channel)];
                      
                      // ***Expect this to be done in the accelerator*** //
					  // Convert uint8_t to int8_t //
                      input_val = input_val - 128;

                      acc += filter_val * input_val;
                    }
                  }
                }
              }
              if (bias_data) {
                acc += bias_data[out_channel * simd + isimd];  // Includes output_offset
                                                // Scaled so we can move it prior to quant
                                                // By bringing it earlier, quantization is now linear, not affine
                                                // NOTE: did add this into bias_data
              }
              
              int shift = -output_shift;
              
              int32_t uv2 = (int16_t)output_multiplier * (int16_t)(acc >> shift);
              int16_t uv3 = uv2 >> 16;
              int16_t uv4 = uv3 >> 4;
              acc_new = uv4;
              
              acc_new = std::max(acc_new, output_activation_min);
              acc_new = std::min(acc_new, output_activation_max);
              
                output_data[Offset(output_shape, batch, out_y, out_x, out_channel * simd + isimd)] = static_cast<uint8>(acc_new);
              if (fPrint) {
                oPrintf("%x, %d%s", acc, acc_new, (iprintcol == 7) ? ",\n  " : ", ");
                iprintcol++;
                if (iprintcol == 8) iprintcol = 0;
              }
            }
          }
        }
      }
   
  }
  //gpio->clear31_00 = ~(1<<5); // gpio 5 clear
  if (fPrint) {
    oPrintf("\n};\n");
  }
}


#include "apb_conv2d.h"

void ConvFPGA_SIMD(const ConvParams& params, 
                  const RuntimeShape& input_shape, const uint8* input_data, 
                  const RuntimeShape& filter_shape, const int8_t* filter_data, 
                  const RuntimeShape& bias_shape, const int32* bias_data, 
                  const RuntimeShape& output_shape, uint8* output_data, 
                  const RuntimeShape& im2col_shape, uint8* im2col_data, 
                  void* cpu_backend_context,
                  bool fPrint) {
	int32_t iprintcol;
  if (fPrint)
    oPrintf("ConvFPGA_SIMD\n");
  

  if (fPrint) {
    iprintcol = 0;
    oPrintf("int32_t axOutput[] = {\n  ");
  }
 
  (void)cpu_backend_context;  // only used in optimized code.
  (void)im2col_data;   // only used in optimized code.
  (void)im2col_shape;  // only used in optimized code.
  const int stride_width = params.stride_width;
  const int stride_height = params.stride_height;
  const int dilation_width_factor = params.dilation_width_factor;
  const int dilation_height_factor = params.dilation_height_factor;
  const int pad_width = params.padding_values.width;
  const int pad_height = params.padding_values.height;
  const int32 input_offset = params.input_offset;
  const int32 filter_offset = params.weights_offset;
  const int32 output_offset = params.output_offset;
  const int32 output_multiplier = params.output_multiplier;
  const int output_shift = params.output_shift;
  const int32 output_activation_min = params.quantized_activation_min;
  const int32 output_activation_max = params.quantized_activation_max;
  TFLITE_DCHECK_LE(output_activation_min, output_activation_max);

  TFLITE_DCHECK_EQ(input_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(filter_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(output_shape.DimensionsCount(), 4);
  const int batches = MatchingDim(input_shape, 0, output_shape, 0);
  const int input_depth = MatchingDim(input_shape, 3, filter_shape, 3);
  const int output_depth = MatchingDim(filter_shape, 0, output_shape, 3);
  if (bias_data) {
    TFLITE_DCHECK_EQ(bias_shape.FlatSize(), output_depth);
  }
  const int input_height = input_shape.Dims(1);
  const int input_width = input_shape.Dims(2);
  const int filter_height = filter_shape.Dims(1);
  const int filter_width = filter_shape.Dims(2);
  const int output_height = output_shape.Dims(1);
  const int output_width = output_shape.Dims(2);
  
  int first = 0;

  int32 acc_orig;
  int32 acc_new;
  const int  verify=false; 
  const int simd = {8};
  if (fPrint) {
    oPrintf("batches=%d\n", batches);
    oPrintf("output_depth=%d\n", output_depth);
    oPrintf("Calling FPGA with w = %d, h = %d, channels = %d, filters_h = %d, filter_w = %d\n",
	   input_width, input_height, output_depth, filter_height, filter_width );
    oPrintf("      input_depth = %d\n", input_depth);
    oPrintf("      &pixel = %x, &filter = %x, &bias = %x &result = %x\n",
	   input_data, filter_data, bias_data,output_data);
    oPrintf("      output_multiplier = %x, shift = %d\n",output_multiplier, output_shift);
  }

  efpga->width = input_width;
  efpga->height = input_height;
  efpga->channels = input_depth;
  efpga->filters = output_depth*8;
  efpga->total_pixels = efpga->width*efpga->height;
  efpga->pixel_base = (volatile unsigned int*) input_data;
  efpga->filter_base = (volatile unsigned int*) filter_data;
  efpga->bias_base = (volatile unsigned int*) bias_data;
  efpga->result_base = (volatile unsigned int*) output_data;
  efpga->quant = (-output_shift << 16) | output_multiplier;
  if (fPrint) {
    oPrintf("Width = %d\n", efpga->width);
    oPrintf("Height = %d\n", efpga->height);
    oPrintf("Channels = %d\n", efpga->channels);
    oPrintf("Filters = %d\n", efpga->filters);
    oPrintf("Quant = %x\n", efpga->quant);
  }

  int quant = efpga->quant;
  if (fPrint) {
    oPrintf("Total_pixels = %d\n", efpga->total_pixels);
    oPrintf("Pixel_base   = %x (%x)\n", efpga->pixel_base,input_data);
    oPrintf("Filter_base  = %x (%x)\n", efpga->filter_base,filter_data);
    oPrintf("Bias_base    = %x (%x)\n", efpga->bias_base, bias_data);
    oPrintf("Result_base  = %x (%x)\n", efpga->result_base, output_data);
  }
  efpga->control = 1;
  while (efpga->control & 1) {}
    oPrintf ("Elapsed Clocks = %d \n",efpga->clocks);//- elapsed_clocks);
  if (verify) {
  gpio->set31_00 = (1<<5);
  for (int batch = 0; batch < batches; ++batch) {
    
      for (int out_y = 0; out_y < output_height; ++out_y) {
        for (int out_x = 0; out_x < output_width; ++out_x) {
          for (int out_channel = 0; out_channel < output_depth; ++out_channel) {
            for (int isimd = 0; isimd != simd; isimd++) {
              const int in_x_origin = (out_x * stride_width) - pad_width;
              const int in_y_origin = (out_y * stride_height) - pad_height;
              int32_t acc = 0;
	      if (bias_data) {
                acc += bias_data[out_channel * simd + isimd];  // Includes output_offset
                                                // Scaled so we can move it prior to quant
                                                // By bringing it earlier, quantization is now linear, not affine
                                                // NOTE: did add this into bias_data
              }
              for (int filter_y = 0; filter_y < filter_height; ++filter_y) {
                for (int filter_x = 0; filter_x < filter_width; ++filter_x) {
                  for (int in_channel = 0; in_channel < input_depth; ++in_channel) {
                    const int in_x = in_x_origin + dilation_width_factor * filter_x;
                    const int in_y =
                        in_y_origin + dilation_height_factor * filter_y;
                    // If the location is outside the bounds of the input image,
                    // use zero as a default value.
                    if ((in_x >= 0) && (in_x < input_width) && (in_y >= 0) &&
                        (in_y < input_height)) {
                      int32_t ifilt = (((out_channel * input_depth) + in_channel) * simd) + isimd;
                      int32_t filter_val = filter_data[ifilt];
                      int32_t input_val = input_data[Offset(input_shape, batch, in_y,
                                                          in_x, in_channel)];
                      
                      // ***Expect this to be done in the accelerator*** //
					  // Convert uint8_t to int8_t //
                      input_val = input_val - 128;

                      acc += filter_val * input_val;
		      if (quant == 0x16253) {
			//			oPrintf("f=%02x * i=%02x == %06x\n",filter_val  & 0xff, input_val & 0xff, acc&0xffffff);
		      }
                    }
                  }
                }
              }
              
              int shift = -output_shift;
              
              int32_t uv2 = (int16_t)output_multiplier * (int16_t)(acc >> shift);
              int16_t uv3 = uv2 >> 16;
              int16_t uv4 = uv3 >> 4;
              acc_new = uv4;
              
              acc_new = std::max(acc_new, output_activation_min);
              acc_new = std::min(acc_new, output_activation_max);
	      if (output_data[Offset(output_shape, batch, out_y, out_x, out_channel * simd + isimd)] != static_cast<uint8>(acc_new) ) {
		  oPrintf("Hardware mismatch[%x] act=%x exp=%x\n",
			 Offset(output_shape, batch, out_y, out_x, out_channel * simd + isimd),
			 output_data[Offset(output_shape, batch, out_y, out_x, out_channel * simd + isimd)],
			 static_cast<uint8>(acc_new) );

              
		  //		  output_data[Offset(output_shape, batch, out_y, out_x, out_channel * simd + isimd)] = static_cast<uint8>(acc_new);
	      }
              if (fPrint) {
                oPrintf("%x, %d%s", acc, acc_new, (iprintcol == 7) ? ",\n  " : ", ");
                iprintcol++;
                if (iprintcol == 8) iprintcol = 0;
              }
            }
          }
        }
      }
   
  }
  //  gpio->clear31_00 = (1<<5);
  gpio->clear31_00 = ~(1<<5);
  } // end verify
  if (fPrint) {
    oPrintf("\n};\n");
  }
}
  

  //}  // namespace reference_ops
}  // namespace tflite

