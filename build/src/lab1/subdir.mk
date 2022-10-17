################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/lab1/Lab1.cpp \
../src/lab1/MatrixMultiplicator.cpp \
../src/lab1/Task1.cpp \
../src/lab1/Task2.cpp 

CPP_DEPS += \
./src/lab1/Lab1.d \
./src/lab1/MatrixMultiplicator.d \
./src/lab1/Task1.d \
./src/lab1/Task2.d 

OBJS += \
./src/lab1/Lab1.o \
./src/lab1/MatrixMultiplicator.o \
./src/lab1/Task1.o \
./src/lab1/Task2.o 


# Each subdirectory must supply rules for building sources it contributes
src/lab1/%.o: ../src/lab1/%.cpp src/lab1/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -I"../src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lab1

clean-src-2f-lab1:
	-$(RM) ./src/lab1/Lab1.d ./src/lab1/Lab1.o ./src/lab1/MatrixMultiplicator.d ./src/lab1/MatrixMultiplicator.o ./src/lab1/Task1.d ./src/lab1/Task1.o ./src/lab1/Task2.d ./src/lab1/Task2.o

.PHONY: clean-src-2f-lab1

