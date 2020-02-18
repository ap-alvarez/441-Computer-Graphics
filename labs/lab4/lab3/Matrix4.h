#ifndef _MATRIX4_
#define _MATRIX4_

#include <cmath>

class Vec4
{
public:
	float vec[4];

	Vec4() {
		for (int i = 0; i < 4; i++) {
			vec[i] = 0;
		}
	}

	Vec4(float setVec[4]) {
		for (int i = 0; i < 4; i++) {
			vec[i] = setVec[i];
		}
	}
};

class Matrix4 
{
public:
	float matrix[16];

	Matrix4() {
		for (int i = 0; i < 16; i++) {
			matrix[i] = 0;
		}
	}

	Matrix4(bool identity) {
		if (identity) {
			int index = 0;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					index = (i * 4) + j;
					if (j == i) {
						matrix[index] = 1;
					}
					else {
						matrix[index] = 0;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < 9; i++) {
				matrix[i] = 0;
			}
		}
	}

	Matrix4(float setMatrix[16]) {
		for (int i = 0; i < 16; i++) {
			matrix[i] = setMatrix[i];
		}
	}

	Matrix4(Vec4 fromVecs[4]) {
		int index = 0;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				index = (i * 4) + j;
				matrix[index] = fromVecs[j].vec[i];
			}
		}
	}
	
	Vec4 Multiply(Vec4 vec) {
		Vec4 newVec = Vec4();
		int index = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				index = (i * 4) + j;
				newVec.vec[i] += matrix[index] * vec.vec[j];
			}
		}

		return newVec;
	}

	Vec4 GetVec(int j) {
		Vec4 newVec = Vec4();
		int index = 0;
		for (int i = 0; i < 4; i++) {
			index = (i * 4) + j;
			newVec.vec[i] = matrix[index];
		}
		return newVec;
	}

	void SetVec(int j, Vec4 vec) {
		int index = 0;
		for (int i = 0; i < 4; i++) {
			index = (i * 4) + j;
			matrix[index] = vec.vec[i];
		}
	}

	Matrix4 MatMul(Matrix4 mat) {
		Matrix4 newMat = Matrix4();
		for (int j = 0; j < 4; j++) {
			newMat.SetVec(j, Multiply(mat.GetVec(j)));
		}
		
		return newMat;
	}
};

Matrix4 Scale(float Sx, float Sy, float Sz) {
	float arr[16]{
		Sx,  0.0, 0.0, 0.0,
		0.0, Sy,  0.0, 0.0,
		0.0, 0.0,  Sz, 0.0,
		0.0, 0.0, 0.0, 1.0 };

	return Matrix4(arr);
}

Matrix4 Rotate(float Rx, float Ry, float Rz) {
	float zRot[16]{
		cos(Rz), -(sin(Rz)), 0.0, 0.0,
		sin(Rz), cos(Rz),    0.0, 0.0,
		0.0,      0.0,         1.0, 0.0,
		0.0,	  0.0,		   0.0, 1.0 };

	float yRot[16]{
		cos(Ry), 0.0, sin(Ry), 0.0,
		0.0,	  1.0,	    0.0, 0.0,
		-(sin(Ry)), cos(Ry),0.0, 0.0,
		0.0,	  0.0,		   0.0, 1.0 };

	float xRot[16]{
		1.0,	  0.0,		   0.0,		0.0,
		0.0,	  cos(Rx), -(sin(Rx)), 0.0,
		0.0,	  sin(Rx),  cos(Rx),   0.0,
		0.0,	  0.0,		   0.0,     1.0 };

	return Matrix4(zRot).MatMul(Matrix4(yRot).MatMul(Matrix4(xRot)));
}

Matrix4 Translate(float Tx, float Ty, float Tz) {
	float arr[16]{
		1.0, 0.0, 0.0, Tx,
		0.0, 1.0, 0.0, Ty,
		0.0, 0.0, 1.0, Tz,
		0.0, 0.0, 0.0, 1.0 };

	return Matrix4(arr);
}

#endif
