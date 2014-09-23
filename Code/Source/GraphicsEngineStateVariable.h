// Name:
// GraphicsEngineStateVariable.h
// Description:
// Header file for GraphicsEngineStateVariable class.
// Notes:
// OS-Unaware
// 

#ifndef GRAPHICS_ENGINE_STATE_VARIABLE_H
#define GRAPHICS_ENGINE_STATE_VARIABLE_H

#include <memory>
using namespace std;

#include <glload/gl_3_3.h>

enum class VariableType
{
	Empty,
	Bool,
	Float,
	Double,
	Int,
	Int64
};

union stateValue
{
	GLboolean booleanValue;
	GLfloat floatValue;
	GLdouble doubleValue;
	GLint intValue;
	GLint64 int64Value;
};


class GraphicsEngineStateVariable
{
private:
	VariableType type;
	unsigned short count;
	stateValue value;

public:
	GraphicsEngineStateVariable() : type(VariableType::Empty), count(1){};
	GraphicsEngineStateVariable(GLboolean value) : type(VariableType::Bool), count(1) { this->value.booleanValue = value; };
	GraphicsEngineStateVariable(GLfloat value) : type(VariableType::Float), count(1) { this->value.floatValue = value; };
	GraphicsEngineStateVariable(GLdouble value) : type(VariableType::Double), count(1) { this->value.doubleValue = value; };
	GraphicsEngineStateVariable(GLint value) : type(VariableType::Int), count(1) { this->value.intValue = value; };
	GraphicsEngineStateVariable(GLint64 value) : type(VariableType::Int64), count(1) { this->value.int64Value = value; };
	GraphicsEngineStateVariable(GraphicsEngineStateVariable& source) = default;
	GraphicsEngineStateVariable& operator=(GraphicsEngineStateVariable&) = default;

	GLboolean getBool(){ if (type == VariableType::Bool) return value.booleanValue; throw exception("Attempt to get bool from non-bool GraphicsEngineStateVariable"); }
	GLfloat getFloat(){ if (type == VariableType::Float) return value.floatValue; throw exception("Attempt to get float from non-float GraphicsEngineStateVariable"); }
	GLdouble getDouble(){ if (type == VariableType::Double) return value.doubleValue; throw exception("Attempt to get double from non-double GraphicsEngineStateVariable"); }
	GLint getInt(){ if (type == VariableType::Int) return value.intValue; throw exception("Attempt to get int from non-int GraphicsEngineStateVariable"); }
	GLint64 getInt64(){ if (type == VariableType::Int64) return value.int64Value; throw exception("Attempt to get int64 from non-int64 GraphicsEngineStateVariable"); }

	VariableType getType(){ return type; }

	bool operator ==(GraphicsEngineStateVariable& other)
	{
		if (other.type != type)
			return false;

		if (other.type == VariableType::Bool)
		{
			if (other.value.booleanValue != this->value.booleanValue)
				return false;
		}

		if (other.type == VariableType::Float)
		{
			if (other.value.floatValue != this->value.floatValue)
				return false;
		}

		if (other.type == VariableType::Double)
		{
			if (other.value.doubleValue != this->value.doubleValue)
				return false;
		}

		if (other.type == VariableType::Int)
		{
			if (other.value.intValue != this->value.intValue)
				return false;
		}

		if (other.type == VariableType::Int64)
		{
			if (other.value.int64Value != this->value.int64Value)
				return false;
		}

		return true;
	}

	bool operator!=(GraphicsEngineStateVariable& other)
	{
		return !(*this == other);
	}
};

#endif
