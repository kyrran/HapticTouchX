#pragma once
#include <Python.h>

class SensorData {
	public:
		SensorData();
		void getData();
		virtual ~SensorData() {};

	public:
		PyObject* pModule;
		float conductance;

};
