# Flex-SVM

This repository contains a modified version of the open-source CFU-Playground framework. With this setup, you can program your FPGA with a SERV-based design enhanced by a custom co-processor, and evaluate it using any C program of your choice.

## Required setup

We are using the 2020.1 version of the Vivado suite. The tools directory should be located in your home directory. In order for the Dockerfile to correctly set-up the environment, you need to change the **home** directory to **workspace** in the following files:

- tools/Xilinx/Vivado/2020.1/settings64.sh

- tools/Xilinx/Vivado/2020.1/.settings64-Vivado.sh

- tools/Xilinx/DocNav/.settings64-DocNav.sh

#### Original Line - Example  

``` source /home/pvergos/tools/Xilinx/Vivado/2020.1/.settings64-Vivado.sh ```

#### Updated Line - Example

``` source /workspace/tools/Xilinx/Vivado/2020.1/.settings64-Vivado.sh ```

## Installation

Follow these steps to install the project:

### 1. Clone the repository
   ```bash
   git clone https://github.com/PolykarposV/Flex-SVM.git
   ```

### 2. Cd into it
   ```bash
   cd ./Flex-SVM
   ```

### 3. Build the Dockerfile inside the Container
   ```bash
   docker build -t cfu ./Container
   ```

### 4a. Locate your FPGA's port

Before running the Docker container, find where your FPGA is connected by running:
  ```bash
  lsusb
  ```

This will list all connected USB devices. Your FPGA device should look like:

``` Bus 00A Device 00B: ID xxxx:yyyy 'Your FPGA Device' ```

### 4b. Run the docker
In this example the FPGA is connected to BUS = 001 and Device = 004, so we use 001/004. If your numbers are different replace it with your values. Also replace
your home directory accordingly.
   ```bash
   docker run --rm --privileged --device=/dev/bus/usb/001/004 -ti -v /home/pvergos/tools:/workspace/tools:ro cfu bash
   ```

Great! You have succesfully entered the image you created and can now start using the emulator!


> [!NOTE]
> Every time you need to enter the docker image you need to re-synthesize your design. For persistency reasons we recommend creating a directory and mounting it when running the image, similarly to the tools directory.
> The command to run the docker that mounts another directory 'build' as discussed above, should look like something like this.
> ```bash
> docker run --rm --privileged --device=/dev/bus/usb/001/004 -ti -v /home/pvergos/tools:/workspace/tools:ro -v $(pwd)/build:/workspace/CFU-Playground/soc/build cfu bash  

## How to generate and load the bitstream for SERV + ML Accelerator

After you have finished the installation process and are inside the docker image, follow these steps:

### 1. Cd into the demo directory:
   ```bash
   cd CFU-Playground/proj/demo
   ```

### 2. Synthesize your system with Vivado and load the bitstream to the FPGA:
   
   ```bash
   make TARGET=digilent_arty_s7 USE_VIVADO=1 EXTRA_LITEX_ARGS="--cpu-type=serv --cpu-variant=cfu" SERV=1 TTY=/dev/ttyUSB1 IGNORE_TIMING=1 prog
   ```

   - **TARGET** specifies the board we are using, in this case the Digilent Arty S7.
  
   - **USE_VIVADO** is set so that we can use the Vivado suite.
  
   - **EXTRA_LITEX_ARGS** contains the flags for the cpu and the variant, in this case SERV and CFU - Your accelerator.
  
   - **SERV** is set so that the UART speed is set accordingly for SERV.
  
   - **TTY** specifies the port for the serial connection between host and FPGA.
     
Running this command forces Synthesis, Placing and Routing and bistream generation to start. Once the SoC is finalized and the bitstream is uploaded to your board, you will see a chasing LED pattern in your FPGA, indicating that it has been programmed. 

## How to execute C programs 

After you have programmed your FPGA follow these steps to execute your C program using the SERV + ML Accelerator system:

### 1. Compile your C programs and load the binary files
   ```bash
   make TARGET=digilent_arty_s7 USE_VIVADO=1 EXTRA_LITEX_ARGS="--cpu-type=serv --cpu-variant=cfu" SERV=1 TTY=/dev/ttyUSB1 IGNORE_TIMING=1 load
   ```

### 2. Start the LiteX terminal
During execution, at some point, the process may appear to freeze. When this happens, simply press **Enter** to proceed.

### 3. Reboot the FPGA
Once the LiteX terminal appears, type **reboot** and press **Enter**. This will force a reset on your FPGA.

### 4. Run the Demo
After uploading finishes, you should see a 'Liftoff' message, followed by a menu. Press **d** to execute the demo.

### 5. Exiting the execution
If you wish to exit and return back, press repeatedly **Ctrl+C**.  


> [!NOTE]
> Every time you need to compile and load your source code you need to first run the ```make prog``` command as before. Since your project is already built it will only take a few seconds.

## How to create and test your own C programs

With the ```make load``` command and the proper arguments as discussed, everything that is inside the src directory of your project is being compiled.

Inside the src directory there is a donut.c file that contains the function _void donut()_.

This is the "main" function that contains your program under testing.

Within the _demo/models_ directory, you will find all the models we used to evaluate and validate our own SVM co-processor across five different datasets. For each dataset, there is one model per configuration—using 4-, 8-, or 16-bit weight parameters—and one model per classification strategy: one-vs-rest (OvR) or one-vs-one (OvO). This results in a total of 30 distinct test cases, each serving as a comprehensive example for testing and benchmarking the system.

In any case, keep the defined macros as they are, since these are the new instructions implemented for the custom co-processor, along with all the header files that you plan to use.

### Custom instructions

In total, seven new instructions have been defined, which are listed below.

- ``` SV_create_env(a,b) ```. Initializes the environment and internal registers. Acts as a software-accessible reset for the co-processor.
  
- ``` SV_calc(a,b) ```. Computes the weighted sum of a and b and updates the internal accumulator (cur_sum register) within the co-processor. 

- ``` SV_res(a,b) ```. Finalizes the computation of the classifier. It conditionally updates the max_id and max_sum internal registers and returns a 32-bit packed result that includes both the max_id and the sign of the cur_sum. The format of the value returned is:
  
  * ``` Output = {sign_bit_of_cur_sum, 23'0, max_id} ```

For different weight precisions (4-bit, 8-bit, 16-bit), the last two instructions are suffixed accordingly:
| Weight Precision | Instructions           |
|------------------|-----------------------|
| 4-bit            | `SV_calc4`, `SV_res4` |
| 8-bit            | `SV_calc8`, `SV_res8` |
| 16-bit           | `SV_calc16`, `SV_res16` |

If you plan to use the ML accelerator, it is important to adhere to its architectural constraints—particularly in how arguments are passed to the custom instructions.



| Instruction     | Weight Precision | Operand A Format                                      | Operand B Format               |
|----------------|------------------|-------------------------------------------------------|--------------------------------|
| SV_calc4      | 4-bit            | {input0, input1, ..., input7}                      | {weight0, weight1, ..., weight7} |
| SV_calc8      | 8-bit            | {input0 ×2, input1 ×2, input2 ×2, input3 ×2} | {weight0, weight1, weight2, weight3} |
| SV_calc16     | 16-bit           | {input0 ×4, input1 ×4}                              | {weight0, weight1}           |
| SV_res*       | All              | {0}                                                 | {0}                          |
| SV_create_env | All              | {0}                                                 | {0}                          | 



> For SV_res, the a and b arguments can be set to 0 only if no additional computation is needed, i.e., when all inputs and weights have already been processed using SV_calc.
> 
> Alternatively, SV_res can be used just like SV_calc to process the final batch and return the result.


## Files for synthesis

All the files required for synthesis are located within the _Synthesis_ directory. More specifically, it contains two subdirectories:

- SERV
  * It holds the .v, .tcl, and .sh files necessary for SERV synthesis.

- CFU
  * It contains the .v, .tcl, and .sh files necessary for ML accelerator synthesis.   

If you're planning to change the system's architecture you can find the .v files for SERV in the **Container/CFU-Playground/third_party/python/pythondata_cpu_serv/pythondata_cpu_serv/verilog/rtl/** directory and the cfu.v file in **Container/CFU-Playground/proj/demo/cfu.v**.


## Altering the ML accelerator's architecture

So far we have demonstrated how to program an FPGA board with the existing SERV + ML accelerator architecture and how to develop and run programs in C using the new instructions. However, if someone needs a co-processor tailored to their needs, they could quite easily re-program the existing one.

You could modify the cfu.v file inside the demo directory according to your application, but you need to maintain the same interface between SERV and co-processor. Also you need to respect the handshake signals that are implemented and the cycle-constraints that exist. 

In our case, the ML accelerator needs 1 cycle to store the inputs to the corresponding registers and 1 more cycle to compute and set the output flag to logic high. These 2 cycles must be preserved. It is possible for your design to take more than 1 clock cycle to compute the output, but these two cycles are mandatory for the correct communication between processor and co-processor.

If you want to change the existing interface you need to modify the SERV .v files accordingly and both the cfu.v file and the instantiation of the ML accelerator module.
