#ifndef _MATRIX3_
#define _MATRIX3_

#include <cmath>

class Vec3
{
public:
	float vec[3];

	Vec3() {
		for (int i = 0; i < 3; i++) {
			vec[i] = 0;
		}
	}

	Vec3(float setVec[3]) {
		for (int i = 0; i < 3; i++) {
			vec[i] = setVec[i];
		}
	}
};

class Matrix3 
{
public:
	float matrix[9];

	Matrix3() {
		for (int i = 0; i < 9; i++) {
			matrix[i] = 0;
		}
	}

	Matrix3(bool identity) {
		if (identity) {
			int index = 0;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					index = (i * 3) + j;
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

	Matrix3(float setMatrix[9]) {
		for (int i = 0; i < 9; i++) {
			matrix[i] = setMatrix[i];
		}
	}

	Matrix3(Vec3 fromVecs[3]) {
		int index = 0;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				index = (i * 3) + j;
				matrix[index] = fromVecs[j].vec[i];
			}
		}
	}
	
	Vec3 Multiply(Vec3 vec) {
		Vec3 newVec = Vec3();
		int index = 0;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				index = (i * 3) + j;
				newVec.vec[i] += matrix[index] * vec.vec[j];
			}
		}

		return newVec;
	}

	Vec3 GetVec(int j) {
		Vec3 newVec = Vec3();
		int index = 0;
		for (int i = 0; i < 3; i++) {
			index = (i * 3) + j;
			newVec.vec[i] = matrix[index];
		}
		return newVec;
	}

	void SetVec(int j, Vec3 vec) {
		int index = 0;
		for (int i = 0; i < 3; i++) {
			index = (i * 3) + j;
			matrix[index] = vec.vec[i];
		}
	}

	Matrix3 MatMul(Matrix3 mat) {
		Matrix3 newMat = Matrix3();
		for (int j = 0; j < 3; j++) {
			newMat.SetVec(j, Multiply(mat.GetVec(j)));
		}
		
		return newMat;
	}
};

Matrix3 Scale(float Sx, float Sy) {
	float arr[9]{
		Sx,  0.0, 0.0,
		0.0, Sy,  0.0,
		0.0, 0.0, 1.0 };

	return Matrix3(arr);
}

Matrix3 Rotate(float phi) {
	float arr[9]{
		cos(phi), -(sin(phi)), 0.0,
		sin(phi), cos(phi),    0.0,
		0.0,      0.0,         1.0 };

	return Matrix3(arr);
}

Matrix3 Translate(float Tx, float Ty) {
	float arr[9]{
		1.0, 0.0, Tx,
		0.0, 1.0, Ty,
		0.0, 0.0, 1.0 };

	return Matrix3(arr);
}

#endif
