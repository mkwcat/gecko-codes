#pragma once

extern "C" {

/**
 * A 3x4 matrix.
 */
typedef float Mtx[3][4];

/**
 * A 4x4 matrix.
 */
typedef float Mtx44[4][4];

void PSMTXIdentity(Mtx* mtx);
void PSMTXConcat(const Mtx* a, const Mtx* b, Mtx* out);
void PSMTXCopy(const Mtx* src, Mtx* dst);
void PSMTXTrans(Mtx* mtx, float x, float y, float z);
void PSMTXScale(Mtx* mtx, float x, float y, float z);

void C_MTXOrtho(Mtx44*, float, float, float, float, float, float);

/**
 * Sets the given matrix to the identity matrix.
 */
#define MTXIdentity(mtx) PSMTXIdentity(mtx)

/**
 * Concatenates two matrices.
 */
#define MTXConcat(a, b, out) PSMTXConcat(a, b, out)

/**
 * Copies a matrix.
 */
#define MTXCopy(src, dst) PSMTXCopy(src, dst)

/**
 * Sets a translation matrix with the given components.
 */
#define MTXTrans(mtx, x, y, z) PSMTXTrans(mtx, x, y, z)

/**
 * Sets a scaling matrix with the given components.
 */
#define MTXScale(mtx, x, y, z) PSMTXScale(mtx, x, y, z)

} // extern "C"