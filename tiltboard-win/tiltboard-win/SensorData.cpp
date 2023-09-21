#include <Python.h>
#include <iostream>
#include <string>
#include "SensorData.h"
using namespace std;

SensorData::SensorData()
{
	conductance = 0;
	Py_Initialize();
	pModule = PyImport_AddModule("__main__");
	PyRun_SimpleString(
		"import sys, os\n"
		"print(os.getcwd())\n"
		"sys.path.insert(1, (os.getcwd()+'/resources'))\n"
		"class CatchOutput :\n"
		"   def __init__(self) :\n"
		"       self.value = ''\n"
		"   def write(self, txt) :\n"
		"       self.value = txt\n"
		"outputCatch = CatchOutput()\n"
		"sys.stdout = outputCatch\n"
		"import read_sensor\n"
		"read_sensor.setup()\n");
	
}

void SensorData::getData() {
	while (true) {
		PyRun_SimpleString("read_sensor.read()");

		PyObject* catcher = PyObject_GetAttrString(pModule, "outputCatch");
		PyObject* output = PyObject_GetAttrString(catcher, "value");
		cout << _PyUnicode_AsString(output);
		
		if (stof(_PyUnicode_AsString(output))> 10 || stof(_PyUnicode_AsString(output))<0) {
			continue;
		}
		else {
			conductance = stof(_PyUnicode_AsString(output));
		}
	}
}

