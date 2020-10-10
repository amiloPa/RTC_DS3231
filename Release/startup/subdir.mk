################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32.s 

OBJS += \
./startup/startup_stm32.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -I"E:/STM32_ARM/MY_LIBRARIES/StdPeriferial_I2C_and_DS3231/StdPeriph_Driver/inc" -I"E:/STM32_ARM/MY_LIBRARIES/StdPeriferial_I2C_and_DS3231/inc" -I"E:/STM32_ARM/MY_LIBRARIES/StdPeriferial_I2C_and_DS3231/CMSIS/device" -I"E:/STM32_ARM/MY_LIBRARIES/StdPeriferial_I2C_and_DS3231/CMSIS/core" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


