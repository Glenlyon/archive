################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../Allocate.cc \
../Classifier.cc \
../Clone.cc \
../Deallocate.cc \
../Delay.cc \
../Fork.cc \
../Job.cc \
../JobList.cc \
../Job_m.cc \
../Join.cc \
../Merge.cc \
../PassiveQueue.cc \
../Queue.cc \
../ResourceBasedQueue.cc \
../ResourcePool.cc \
../Router.cc \
../SelectionStrategies.cc \
../Server.cc \
../Sink.cc \
../Source.cc 

OBJS += \
./Allocate.o \
./Classifier.o \
./Clone.o \
./Deallocate.o \
./Delay.o \
./Fork.o \
./Job.o \
./JobList.o \
./Job_m.o \
./Join.o \
./Merge.o \
./PassiveQueue.o \
./Queue.o \
./ResourceBasedQueue.o \
./ResourcePool.o \
./Router.o \
./SelectionStrategies.o \
./Server.o \
./Sink.o \
./Source.o 

CC_DEPS += \
./Allocate.d \
./Classifier.d \
./Clone.d \
./Deallocate.d \
./Delay.d \
./Fork.d \
./Job.d \
./JobList.d \
./Job_m.d \
./Join.d \
./Merge.d \
./PassiveQueue.d \
./Queue.d \
./ResourceBasedQueue.d \
./ResourcePool.d \
./Router.d \
./SelectionStrategies.d \
./Server.d \
./Sink.d \
./Source.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


