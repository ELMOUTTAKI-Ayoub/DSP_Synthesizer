################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Vectors_intr.obj: ../Vectors_intr.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.2/bin/cl6x" -g --define=CHIP_6713 --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.2/include" --include_path="C:/Users/Matthi Laptop/workspace_v5_3/fooBar" --include_path="C:/Users/Matthi Laptop/workspace_v5_3/fooBar/inc" --include_path="C:/Users/Matthi Laptop/workspace/config/Support_Files/csl/include" --include_path="C:/Users/Matthi Laptop/workspace/config/Support_Files" --include_path="C:/Users/Matthi Laptop/workspace/config/Support_Files/dsk6713/include" --include_path="C:/Users/Matthi Laptop/workspace_v5_3/fooBar/Debug" --include_path="C:/ti/bios_5_42_00_07/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_00_07/packages/ti/rtdx/include/c6000" --diag_warning=225 --mem_model:data=far --abi=coffabi --preproc_with_compile --preproc_dependency="Vectors_intr.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.2/bin/cl6x" -g --define=CHIP_6713 --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.2/include" --include_path="C:/Users/Matthi Laptop/workspace_v5_3/fooBar" --include_path="C:/Users/Matthi Laptop/workspace_v5_3/fooBar/inc" --include_path="C:/Users/Matthi Laptop/workspace/config/Support_Files/csl/include" --include_path="C:/Users/Matthi Laptop/workspace/config/Support_Files" --include_path="C:/Users/Matthi Laptop/workspace/config/Support_Files/dsk6713/include" --include_path="C:/Users/Matthi Laptop/workspace_v5_3/fooBar/Debug" --include_path="C:/ti/bios_5_42_00_07/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_00_07/packages/ti/rtdx/include/c6000" --diag_warning=225 --mem_model:data=far --abi=coffabi --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


