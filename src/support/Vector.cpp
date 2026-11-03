#include "Vector.h"

#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace sim;

const double EPSILON = 0.000000000001;
const double HALFPI = 1.57079632679489661923;

Matrix4d identity_matrix (Matrix4d::Identity);
Matrix4d zero_matrix (Matrix4d::Zero);

// Transform a Vector3d
// "m" is a 3x3 transformation matrix augmented with offset information

Vector3d Matrix4d::operator * (const Vector3d & v)
const
{
    return
        v.
        Transform (*this, Augmented);
}


Vector3d
Vector3d::Transform (const Matrix4d & m,
AugmentationType t /*=Augmented*/ ) const
{
    if (t == Augmented)
    {
        return Vector3d (x * m.mat (0, 0) + y * m.mat (0, 1) +
            z * m.mat (0, 2) + m.mat (0, 3), x * m.mat (1,
            0) +
            y * m.mat (1, 1) + z * m.mat (1, 2) + m.mat (1,
            3),
            x * m.mat (2, 0) + y * m.mat (2,
            1) + z * m.mat (2,
            2) +
            m.mat (2, 3)
            );
    }
    else
    {
        return Vector3d (x * m.mat (0, 0) + y * m.mat (0, 1) +
            z * m.mat (0, 2), x * m.mat (1, 0) + y * m.mat (1,
            1)
            + z * m.mat (1, 2), x * m.mat (2,
            0) + y * m.mat (2,
            1)
            + z * m.mat (2, 2)
            );
    }
}


Matrix4d::Matrix4d (const double latitude, const double longitude)
{
    double s2, s3, c2, c3;

    s2 = sin (latitude);
    s3 = sin (longitude);
    c2 = cos (latitude);
    c3 = cos (longitude);

    m[0][0] = c2 * c3;
    m[0][1] = c2 * s3;
    m[0][2] = -s2;
    m[1][0] = -s3;
    m[1][1] = c3;
    m[1][2] = 0.0;
    m[2][0] = s2 * c3;
    m[2][1] = s2 * s3;
    m[2][2] = c2;
    m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0;
    m[3][3] = 1.0;
}


// multiplication of two matrices

Matrix4d Matrix4d::operator * (const Matrix4d & b)
const
{
    int
        i,
        j,
        k;
    double
        x;
    Matrix4d
        result;

    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            x = 0.0;
            for (k = 0; k < 4; ++k)
            {
                x += m[k][i] * b.m[j][k];
            }
            result.
                m[j][i] =
                x;
        }
    }
    return result;
}


void
Matrix4d::Translate (const Vector3d & x)
{
    mat (0, 3) =
        x.x * mat (0, 0) + x.y * mat (0, 1) + x.z * mat (0, 2) + mat (0, 3);
    mat (1, 3) =
        x.x * mat (1, 0) + x.y * mat (1, 1) + x.z * mat (1, 2) + mat (1, 3);
    mat (2, 3) =
        x.x * mat (2, 0) + x.y * mat (2, 1) + x.z * mat (2, 2) + mat (2, 3);
    mat (3, 3) =
        x.x * mat (3, 0) + x.y * mat (3, 1) + x.z * mat (3, 2) + mat (3, 3);
}


void
Matrix4d::Translate (double x, double y, double z)
{
    mat (0, 3) =
        x * mat (0, 0) + y * mat (0, 1) + z * mat (0, 2) + mat (0, 3);
    mat (1, 3) =
        x * mat (1, 0) + y * mat (1, 1) + z * mat (1, 2) + mat (1, 3);
    mat (2, 3) =
        x * mat (2, 0) + y * mat (2, 1) + z * mat (2, 2) + mat (2, 3);
    mat (3, 3) =
        x * mat (3, 0) + y * mat (3, 1) + z * mat (3, 2) + mat (3, 3);
}


void
Matrix4d::Scale (double x, double y, double z)
{
    mat (0, 0) *= x;
    mat (0, 1) *= y;
    mat (0, 2) *= z;
    mat (1, 0) *= x;
    mat (1, 1) *= y;
    mat (1, 2) *= z;
    mat (2, 0) *= x;
    mat (2, 1) *= y;
    mat (2, 2) *= z;
    mat (3, 0) *= x;
    mat (3, 1) *= y;
    mat (3, 2) *= z;
}


void
Matrix4d::Matrix4dToEuler (double *dPhi, double *dTheta, double *dPsi) const
{
    *dPhi = atan2 (mat1 (2, 3), mat1 (3, 3));
    *dTheta = -asin (mat1 (1, 3));
    *dPsi = atan2 (mat1 (1, 2), mat1 (1, 1));
    if (*dPsi < 0.0)
    {
        *dPsi += 2.0 * M_PI;
    }

}


// print Matrix4d

std::ostream & operator << (std::ostream & o, const Matrix4d & m)
{
    int i1, i, j, k, n, max = 0;
    char str[16][128];
    double x;

    // First, determine the length of the longest printed Matrix4d element
    n = 0;
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            x = m.m[j][i];
            sprintf (&str[n][0], "%g", x);
            k = strlen (&str[n][0]);
            if (k > max)
            {
                max = k;
            }
            ++n;
        }
    }

    // Now print them ...

    n = 0;
    for (i = 0; i < 4; ++i)
    {
        o << "|  ";
        for (j = 0; j < 4; ++j)
        {
            n = i * 4 + j;
            k = max - strlen (&str[n][0]);
            for (i1 = 0; i1 < k; ++i1)
            {
                o << " ";
            }
            o << &str[n][0];
            o << "  ";
            ++n;
        }
        o << "|\n";
    }

    return o;
}


#ifdef notdef
// cast Matrix4d to quaternion

Matrix4d::operator Quaternion () const
{
    quaternion q;
    double tr, s, w, x, y, z, qi;
    int i, j, k;

    tr = mat (0, 0) + mat (1, 1) + mat (2, 2);
    if (tr > 0.0)
    {
        s = sqrt (tr + 1.0);
        w = 0.5 * s;
        s = 0.5 / s;

        x = (mat (1, 2) - mat (2, 1)) * s;
        y = (mat (2, 0) - mat (0, 2)) * s;
        z = (mat (0, 1) - mat (1, 0)) * s;
        q = Quaternion (w, x, y, z);
    }
    else
    {
        i = 0;
        if (mat (1, 1) > mat (0, 0))
            i = 1;
        if (mat (2, 2) > mat (i, i))
            i = 2;
        if ((j = i + 1) > 2)
            j = 0;
        if ((k = j + 1) > 2)
            k = 0;

        s = sqrt (mat (i, i) - (mat (j, j) + mat (k, k)) + 1.0);
        qi = s * 0.5;
        s = 0.5 / s;
        switch (i)
        {
            case 0:
                q = Quaternion ((mat (j, k) - mat (k, j)) * s,
                    qi,
                    (mat (i, j) - mat (j, i)) * s,
                    (mat (i, k) - mat (k, i)) * s);
                break;
            case 1:
                q = Quaternion ((mat (j, k) - mat (k, j)) * s,
                    (mat (i, k) - mat (k, i)) * s,
                    qi, (mat (i, j) - mat (j, i)) * s);
                break;
            case 2:
                q = Quaternion ((mat (j, k) - mat (k, j)) * s,
                    (mat (i, j) - mat (j, i)) * s,
                    (mat (i, k) - mat (k, i)) * s, qi);
                break;
        }
    }
    return q;
}


// cast quaternion to Matrix4d

Quaternion::operator Matrix4d () const
{
    Matrix4d mat (Matrix4d::Identity);
    Vector3d vs, w, x;
    double s0, yy, yz, zz;

    s0 = 2.0 / (s * s + v.x * v.x + v.y * v.y + v.z * v.z);

    vs = v * s0;
    w = s * vs;
    x = v.x * vs;
    yy = v.y * vs.y;
    yz = v.y * vs.z;
    zz = v.z * vs.z;

    mat.mat (0, 0) = 1.0 - (yy + zz);
    mat.mat (0, 1) = x.y + w.z;
    mat.mat (0, 2) = x.z - w.y;

    mat.mat (1, 0) = x.y - w.z;
    mat.mat (1, 1) = 1.0 - (x.x + zz);
    mat.mat (1, 2) = yz + w.x;

    mat.mat (2, 0) = x.z + w.y;
    mat.mat (2, 1) = yz - w.x;
    mat.mat (2, 2) = 1.0 - (x.x + yy);

    return mat;
}


Quaternion
Quaternion::Interpolate (const Quaternion & p, const Quaternion & q,
const double t)
{
    double sin_omega, cos_omega, omega, sp, sq;
    Quaternion r;

    cos_omega = p.s * q.s + p.v ^ q.v;

    if ((1.0 + cos_omega) > EPSILON)
    {
        if ((1.0 - cos_omega) > EPSILON)
        {
            omega = acos (cos_omega);
            sin_omega = sin (omega);
            sp = sin ((1.0 - t) * omega) / sin_omega;
            sq = sin (t * omega) / sin_omega;
        }
        else
        {
            sp = 1.0 - t;
            sq = t;
        }
        r.s = sp * p.s + sq * q.s;
        r.v.x = sp * p.v.x + sq * q.v.x;
        r.v.y = sp * p.v.y + sq * q.v.y;
        r.v.z = sp * p.v.z + sq * q.v.z;
    }
    else
    {
        r.v.x = -p.v.y;
        r.v.y = p.v.x;
        r.v.z = -p.s;
        r.s = p.v.z;
        sp = sin ((1.0 - t) * HALFPI);
        sq = sin (t * HALFPI);
        r.v.x = sp * p.v.x + sq * r.v.x;
        r.v.y = sp * p.v.y + sq * r.v.y;
        r.v.z = sp * p.v.z + sq * r.v.z;
    }
    return r;
}
#endif

Matrix4d
Quaternion::QuaternionToMatrix4d () const
{
    Matrix4d m;

    m.m[0][0] = m_e[0] * m_e[0] + m_e[1] * m_e[1] -
        m_e[2] * m_e[2] - m_e[3] * m_e[3];
    m.m[1][0] = 2.0 * (m_e[1] * m_e[2] + m_e[0] * m_e[3]);
    m.m[2][0] = 2.0 * (m_e[1] * m_e[3] - m_e[0] * m_e[2]);

    m.m[0][1] = 2.0 * (m_e[1] * m_e[2] - m_e[0] * m_e[3]);
    m.m[1][1] = m_e[0] * m_e[0] + m_e[2] * m_e[2] -
        m_e[1] * m_e[1] - m_e[3] * m_e[3];
    m.m[2][1] = 2.0 * (m_e[2] * m_e[3] + m_e[0] * m_e[1]);

    m.m[0][2] = 2.0 * (m_e[1] * m_e[3] + m_e[0] * m_e[2]);
    m.m[1][2] = 2.0 * (m_e[2] * m_e[3] - m_e[0] * m_e[1]);
    m.m[2][2] = m_e[0] * m_e[0] + m_e[3] * m_e[3] -
        m_e[1] * m_e[1] - m_e[2] * m_e[2];

    m.m[0][3] = m.m[3][0] = 0.0;
    m.m[1][3] = m.m[3][1] = 0.0;
    m.m[2][3] = m.m[3][2] = 0.0;
    m.m[3][3] = 1.0;

    return m;
}


// TODO: review rotations (seem to be backwards, sign wise)

void
Matrix4d::Rotate (const Axis rotation_axis, const double theta)
{

    Matrix4d m (Matrix4d::Identity), tmp = *this;

    switch (rotation_axis)
    {

        case XAxis:
            m.mat (1, 1) = m.mat (2, 2) = cos (theta);
            m.mat (2, 1) = sin (theta);
            m.mat (1, 2) = -m.mat (2, 1);
            break;

        case YAxis:
            m.mat (0, 0) = m.mat (2, 2) = cos (theta);
            m.mat (0, 2) = sin (theta);
            m.mat (2, 0) = -m.mat (0, 2);
            break;

        case ZAxis:
            m.mat (0, 0) = m.mat (1, 1) = cos (theta);
            m.mat (1, 0) = sin (theta);
            m.mat (0, 1) = -m.mat (1, 0);
            break;
    }

    *this = tmp * m;
}


// Create a geographic to body transformation matrix based on
// Euler angles.

void
Matrix4d::EulerToMatrix4d (double dPhi, double dTheta, double dPsi)
// phi == roll, theta == pitch, psi == heading
{
    double sinPhi, cosPhi, sinTheta, cosTheta, sinPsi, cosPsi;

    sinPhi = sin (dPhi);
    cosPhi = cos (dPhi);
    sinTheta = sin (dTheta);
    cosTheta = cos (dTheta);
    sinPsi = sin (dPsi);
    cosPsi = cos (dPsi);

    mat (0, 0) = cosTheta * cosPsi;
    mat (1, 0) = sinPhi * sinTheta * cosPsi - cosPhi * sinPsi;
    mat (2, 0) = cosPhi * sinTheta * cosPsi + sinPhi * sinPsi;
    mat (0, 1) = cosTheta * sinPsi;
    mat (1, 1) = sinPhi * sinTheta * sinPsi + cosPhi * cosPsi;
    mat (2, 1) = cosPhi * sinTheta * sinPsi - sinPhi * cosPsi;
    mat (0, 2) = -sinTheta;
    mat (1, 2) = sinPhi * cosTheta;
    mat (2, 2) = cosPhi * cosTheta;
    mat (3, 0) = mat (3, 1) = mat (3, 2) = 0.0;
    mat (0, 3) = mat (1, 3) = mat (2, 3) = 0.0;
    mat (3, 3) = 1.0;
}


Matrix4d
Matrix4d::Transpose () const
{
    int i, j;
    Matrix4d r;

    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            r.m[i][j] = m[j][i];
        }
    }
    return r;
}


Matrix4d::operator Quaternion ()
{
    double dPhi, dTheta, dPsi;
    Matrix4dToEuler (&dPhi, &dTheta, &dPsi);
    return Quaternion (dPhi, dTheta, dPsi);
};

// Apply the given transformation matrix m to this plane equation
// Note: for this to work, m_ABC must be a unit vector

PlaneEquation
PlaneEquation::Transform (const Matrix4d & m) const
{
    // Generate a normal vector for the plane in the new coordinate system
    Vector3d vNewNormal = Normal ().Transform (m, Unaugmented);

    // Generate a point on the plane and transform it to the new coordinate system
    Vector3d vPointOnPlane = m * Vector3d (0, 0, -m_D / m_ABC.z);

    double d = -(vNewNormal.x * vPointOnPlane.x +
        vNewNormal.y * vPointOnPlane.y +
        vNewNormal.z * vPointOnPlane.z);

    return PlaneEquation (vNewNormal, d);
}
