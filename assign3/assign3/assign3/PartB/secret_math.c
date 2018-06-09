#include <secret_message.h>
#include <stdio.h>
#include <stdlib.h>


float secret_add(float a, float b){
	float sum = 0.0;
	sum = a + b;
	return sum;
}

float secret_subtract(float a, float b){
	float c = 0.0;
	c = a - b;
	return c;
}

float secret_multiply(float a, float b){
	float result = 0.0;
	result = a * b;
	return result;
}

float secret_divide(float a, float b){
	float result = 0.0;
	result = a / b;
	return result;
}

