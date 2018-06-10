#include <stdio.h>
#include "secret_math.h"
#include "secret_message.h"
#include "secret_unused.h"

int main() {

	printf("secret_add(13.4, 10.1) = %.2f\n", secret_add(13.4,10.1));
	printf("secret_subtract(5.8, 7.5) = %.2f\n", secret_subtract(5.8, 7.5));
	printf("secret_multiply(4.2, 11.0) = %.2f\n", secret_multiply(4.2, 11.0));
	printf("secret_divide(23.9, 3.3) = %.2f\n", secret_divide(23.9, 3.3));

	secret_message1();
	secret_message2();

	return 0;
}
