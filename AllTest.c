#include <stdio.h>

#include "./include/CuTest.h"
#include "./include/MemoryManagerADT.h"
#include "./include/MemoryManagerTest.h"

void RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, getMemoryManagerTestSuite());

	CuSuiteRun(suite);

	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);

	printf("%s\n", output->buffer);
}

int main(void)
{
	RunAllTests();
	return 0;
}
