################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/util/Task.cpp \
../src/util/pthread_wrapper.cpp \
../src/util/random_data.cpp 

CPP_DEPS += \
./src/util/Task.d \
./src/util/pthread_wrapper.d \
./src/util/random_data.d 

OBJS += \
./src/util/Task.o \
./src/util/pthread_wrapper.o \
./src/util/random_data.o 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../src/util/%.cpp src/util/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -I"../src" -O3 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-util

clean-src-2f-util:
	-$(RM) ./src/util/Task.d ./src/util/Task.o ./src/util/pthread_wrapper.d ./src/util/pthread_wrapper.o ./src/util/random_data.d ./src/util/random_data.o

.PHONY: clean-src-2f-util

