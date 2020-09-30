################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/DS3231.c \
../src/EXTI.c \
../src/F103_lib.c \
../src/I2C.c \
../src/UART.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f10x.c 

OBJS += \
./src/DS3231.o \
./src/EXTI.o \
./src/F103_lib.o \
./src/I2C.o \
./src/UART.o \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f10x.o 

C_DEPS += \
./src/DS3231.d \
./src/EXTI.d \
./src/F103_lib.d \
./src/I2C.d \
./src/UART.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f10x.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F1 -DSTM32F101C8Tx -DDEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -I"E:/STM32_ARM/MY_LIBRARIES/StdPeriferial_I2C_and_DS3231/StdPeriph_Driver/inc" -I"E:/STM32_ARM/MY_LIBRARIES/StdPeriferial_I2C_and_DS3231/inc" -I"E:/STM32_ARM/MY_LIBRARIES/StdPeriferial_I2C_and_DS3231/CMSIS/device" -I"E:/STM32_ARM/MY_LIBRARIES/StdPeriferial_I2C_and_DS3231/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


