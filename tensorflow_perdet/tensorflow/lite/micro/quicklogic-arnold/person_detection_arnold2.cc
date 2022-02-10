  /*============================================================================

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

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/examples/person_detection/model_settings.h"
#include "tensorflow/lite/micro/examples/person_detection/no_person_image_data.h"
#include "tensorflow/lite/micro/examples/person_detection/person_detect_model_data.h"
#include "tensorflow/lite/micro/examples/person_detection/person_image_data.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_optional_debug_tools.h"
#include "tensorflow/lite/micro/testing/micro_test.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"    

#define SEND_PIC_ON_UART_1		0


extern "C" void person_detection_task( void *pParameter );
extern "C" int oPrintf(const char* format, ...);
extern "C" void CLI_printf(uint8_t aUartPortNum, const char *fmt, ... );
extern "C" uint8_t cam_grab_frame (int x, int y, void* pparam);
extern "C" void initPictureBuf(int x, int y, uint8_t* pictureBuf);
extern "C" void displayFrame(int x, int y, uint8_t* pictureBuf);

extern "C" {
#include "include/programFPGA.h"
#include "target/core-v-mcu/include/core-v-mcu-system.h"

}

bool fpga_programmed;
extern uint8_t camera_present;

//static uint8_t picture[(PICTURE_X_SIZE+4) * (PICTURE_Y_SIZE + 4) ];

// Create an area of memory to use for input, output, and intermediate arrays.
constexpr int tensor_arena_size = 93 * 1024;
uint8_t tensor_arena[tensor_arena_size] __attribute__ ((aligned (16)));

namespace micro_test {
	int tests_passed;
	int tests_failed;
	bool is_test_complete;
	bool did_test_fail;
	tflite::ErrorReporter* reporter;
}

void person_detection_task( void *pParameter )
{

    fpga_programmed = false;
    CLI_printf(0, "Person detection demo\n");
#if 1
    oPrintf("fpga_programmed = %x / %d \n",camera_present, fpga_programmed);
    //programFPGA();
    //fpga_programmed = true;

	// Set up logging.
	tflite::MicroErrorReporter micro_error_reporter;
	tflite::ErrorReporter* error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const tflite::Model* model = ::tflite::GetModel(g_person_detect_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.\n",
                         model->version(), TFLITE_SCHEMA_VERSION);
    }
    //PrintModelData(model, error_reporter);

    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.
    // An easier approach is to just use the AllOpsResolver, but this will
    // incur some penalty in code space for op implementations that are not
    // needed by this graph.
    //
    // tflite::AllOpsResolver resolver;
    tflite::MicroMutableOpResolver<3> micro_op_resolver;
    micro_op_resolver.AddAveragePool2D();
    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddDepthwiseConv2D();

    // Build an interpreter to run the model with.
    tflite::MicroInterpreter interpreter(model, micro_op_resolver, tensor_arena,
                                       tensor_arena_size, error_reporter);
    interpreter.AllocateTensors();

    // Get information about the memory area to use for the model's input.
    TfLiteTensor* input = interpreter.input(0);

    // Make sure the input has the properties we expect.
	TF_LITE_MICRO_EXPECT_NE(nullptr, input);
	TF_LITE_MICRO_EXPECT_EQ(4, input->dims->size);
	TF_LITE_MICRO_EXPECT_EQ(1, input->dims->data[0]);
	TF_LITE_MICRO_EXPECT_EQ(kNumRows, input->dims->data[1]);
	TF_LITE_MICRO_EXPECT_EQ(kNumCols, input->dims->data[2]);
	TF_LITE_MICRO_EXPECT_EQ(kNumChannels, input->dims->data[3]);
	TF_LITE_MICRO_EXPECT_EQ(kTfLiteUInt8, input->type);

	// Copy an image with a person into the memory area used for the input.
	const uint8_t* person_data = g_person_data;

// SHow us the picture
#if (SEND_PIC_ON_UART_1 == 1 )
	CLI_printf(0,"Displaying pic on UART1, Use 'python3 spi_load.py /dev/ttyUSBx' to view\n");
	CLI_printf(1,"ScReEn96 96\n");
	for (int j = 0; j<96; j++) {
		for (int k = 0; k < 96; k += 32) {
			int l = 0;
			CLI_printf(1,"ImAgE %d %d",j,k);
			while (l < 32)
				CLI_printf(1," %02x",person_data[j*96+k+(l++)]);
			CLI_printf(1,"\n");
			for (int m=0; m < 20000; m++)
				asm volatile("nop");
		}
	}
#endif

	for (int i = 0; i < input->bytes; ++i) {
	    input->data.uint8[i] = person_data[i];
	}


	// Run the model on this input and make sure it succeeds.
	//gpio->out31_00 = (1<<6);
	CLI_printf(0, "[i/p: person data] running. . .\n");
	TfLiteStatus invoke_status = interpreter.Invoke();
	//gpio->out31_00 = 0;
	if (invoke_status != kTfLiteOk) {
		TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
	}

	TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);


	// Get the output from the model, and make sure it's the expected size and
	// type.
	TfLiteTensor* output = interpreter.output(0);
	TF_LITE_MICRO_EXPECT_EQ(4, output->dims->size);
	TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[0]);
	TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[1]);
	TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[2]);
	TF_LITE_MICRO_EXPECT_EQ(kCategoryCount, output->dims->data[3]);
	TF_LITE_MICRO_EXPECT_EQ(kTfLiteUInt8, output->type);


	// Make sure that the expected "Person" score is higher than the other class.
	uint8_t person_score = output->data.uint8[kPersonIndex];
	uint8_t no_person_score = output->data.uint8[kNotAPersonIndex];
	TF_LITE_REPORT_ERROR(error_reporter,
					   "person data.  person score: %d, no person score: %d\n",
					   person_score, no_person_score);
	TF_LITE_MICRO_EXPECT_GT(person_score, no_person_score);

	// Now test with a different input, from an image without a person.
	const uint8_t* no_person_data = g_no_person_data;

#if (SEND_PIC_ON_UART_1 == 1 )
	for (int j = 0; j<96; j++) {
		for (int k = 0; k < 96; k += 32) {
			int l = 0;
			CLI_printf(1,"ImAgE %d %d",j,k);
			while (l < 32)
				CLI_printf(1," %02x",no_person_data[j*96+k+(l++)]);
			CLI_printf(1,"\n");
			for (int m=0; m < 20000; m++)
				asm volatile("nop");
		}
	}
#endif

	for (int i = 0; i < input->bytes; ++i) {
		input->data.uint8[i] = no_person_data[i];
	}

	// Run the model on this "No Person" input
	//gpio->out31_00 = (1<<6);
	CLI_printf(0, "[i/p: no person data] running. . .\n");
	invoke_status = interpreter.Invoke();
	//gpio->out31_00 = 0;
	if (invoke_status != kTfLiteOk) {
		TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
	}
	TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);

	// Get the output from the model, and make sure it's the expected size and
	// type.
	output = interpreter.output(0);
	TF_LITE_MICRO_EXPECT_EQ(4, output->dims->size);
	TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[0]);
	TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[1]);
	TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[2]);
	TF_LITE_MICRO_EXPECT_EQ(kCategoryCount, output->dims->data[3]);
	TF_LITE_MICRO_EXPECT_EQ(kTfLiteUInt8, output->type);

	// Make sure that the expected "No Person" score is higher.
	person_score = output->data.uint8[kPersonIndex];
	no_person_score = output->data.uint8[kNotAPersonIndex];
	TF_LITE_REPORT_ERROR(
	  error_reporter,
	  "no person data.  person score: %d, no person score: %d\n", person_score,
	  no_person_score);
	TF_LITE_MICRO_EXPECT_GT(no_person_score, person_score);

	TF_LITE_REPORT_ERROR(error_reporter, "Ran successfully\n");

#endif

    for(;;)
    {
    	if( camera_present )
    	{

    		initPictureBuf(PICTURE_X_SIZE, PICTURE_Y_SIZE, tensor_arena);
    		cam_grab_frame(PICTURE_X_SIZE,PICTURE_Y_SIZE, tensor_arena);
    		//displayFrame(PICTURE_X_SIZE,PICTURE_Y_SIZE,tensor_arena );
#if 1
			for (int i = 0; i < 96; ++i)
			{
				for (int j = 0; j<96; j++)
				{
				  input->data.uint8[i*96+j] = tensor_arena[74*324+i*324+144+j];
				}
			}

			CLI_printf(1,"ScReEn96 96\n");
			for (int j = 0; j<96; j++)
			{
				for (int k = 0; k < 96; k += 32)
				{
					int l = 0;
					CLI_printf(1, "ImAgE %d %d",j,k);
					while (l < 32)
						CLI_printf(1," %02x",input->data.uint8[j*96+k+(l++)] & 0xff);
					CLI_printf(1,"\n");
					//for (int m=0; m < 20000; m++)
					//	asm volatile("nop");
				}
			}
			// Run the model on this "No Person" input
			//gpio->out31_00 = (1<<6);
			CLI_printf(0, "[i/p: Himax camera data] running. . .\n");
			invoke_status = interpreter.Invoke();
			//gpio->out31_00 = 0;
			if (invoke_status != kTfLiteOk) {
				TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
			}
			TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);

			// Get the output from the model, and make sure it's the expected size and
			// type.
			output = interpreter.output(0);
			TF_LITE_MICRO_EXPECT_EQ(4, output->dims->size);
			TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[0]);
			TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[1]);
			TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[2]);
			TF_LITE_MICRO_EXPECT_EQ(kCategoryCount, output->dims->data[3]);
			TF_LITE_MICRO_EXPECT_EQ(kTfLiteUInt8, output->type);

			// Make sure that the expected "No Person" score is higher.
			person_score = output->data.uint8[kPersonIndex];
			no_person_score = output->data.uint8[kNotAPersonIndex];
			TF_LITE_REPORT_ERROR(
					error_reporter,
					"Himax Camera image.  person score: %d, no person score: %d\n", person_score,
					no_person_score);
#endif
    	}
    }
}


