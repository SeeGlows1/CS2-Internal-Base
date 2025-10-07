#pragma once

#include <Windows.h>
#include <cmath>
#include <complex>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Vector2 {
    constexpr Vector2(
        const float X = 0.f,
        const float Y = 0.f
    ) noexcept : X(X), Y(Y) {
    }

    float X, Y;
};

struct Vector3 {
    constexpr Vector3(
        const float X = 0.f,
        const float Y = 0.f,
        const float Z = 0.f
    ) noexcept : X(X), Y(Y), Z(Z) {
    }

    Vector3 operator+(const Vector3& Other) const {
        return Vector3(X + Other.X, Y + Other.Y, Z + Other.Z);
    }

    Vector3 operator-(const Vector3& Other) const {
        return Vector3(X - Other.X, Y - Other.Y, Z - Other.Z);
    }

    Vector3 operator*(float Scalar) const {
        return Vector3(X * Scalar, Y * Scalar, Z * Scalar);
    }

    float Length() const {
        return std::sqrt(X * X + Y * Y + Z * Z);
    }

    float Distance(const Vector3& Other) const {
        return (*this - Other).Length();
    }

    bool WTS(Vector2& Out, float(*ViewMatrix)[4][4], int WindowWidth, int WindowHeight) const {
        const float W = (*ViewMatrix)[3][0] * X + (*ViewMatrix)[3][1] * Y + (*ViewMatrix)[3][2] * Z + (*ViewMatrix)[3][3];

        if (W <= 0.01f)
            return false;

        const float InvW = 1.0f / W;

        Out.X = (WindowWidth / 2.0f) + (((*ViewMatrix)[0][0] * X + (*ViewMatrix)[0][1] * Y + (*ViewMatrix)[0][2] * Z + (*ViewMatrix)[0][3]) * InvW * (WindowWidth / 2.0f));
        Out.Y = (WindowHeight / 2.0f) - (((*ViewMatrix)[1][0] * X + (*ViewMatrix)[1][1] * Y + (*ViewMatrix)[1][2] * Z + (*ViewMatrix)[1][3]) * InvW * (WindowHeight / 2.0f));

        return true;
    }

    float X, Y, Z;
};

// View angles structure for aimbot
struct ViewAngles {
    float Pitch; // Up/Down
    float Yaw;   // Left/Right
    float Roll;  // Not used in CS2

    ViewAngles() : Pitch(0.0f), Yaw(0.0f), Roll(0.0f) {}
    ViewAngles(float pitch, float yaw) : Pitch(pitch), Yaw(yaw), Roll(0.0f) {}

    // Calculate angle to target
    static ViewAngles CalculateAngle(const Vector3& from, const Vector3& to) {
        Vector3 delta = to - from;
        float distance = sqrt(delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z);

        // Avoid division by zero
        if (distance < 0.001f) {
            return ViewAngles(0.0f, 0.0f);
        }

        ViewAngles angles;
        angles.Pitch = -atan2(delta.Z, distance) * 180.0f / M_PI;
        angles.Yaw = atan2(delta.Y, delta.X) * 180.0f / M_PI;

        // Clamp pitch to valid range
        if (angles.Pitch > 89.0f) angles.Pitch = 89.0f;
        if (angles.Pitch < -89.0f) angles.Pitch = -89.0f;

        return angles;
    }

    // Calculate FOV between two angles
    float GetFOV(const ViewAngles& other) const {
        float deltaPitch = Pitch - other.Pitch;
        float deltaYaw = Yaw - other.Yaw;

        // Normalize yaw difference
        while (deltaYaw > 180.0f) deltaYaw -= 360.0f;
        while (deltaYaw < -180.0f) deltaYaw += 360.0f;

        return sqrt(deltaPitch * deltaPitch + deltaYaw * deltaYaw);
    }

    // Smooth interpolation between angles
    ViewAngles SmoothTo(const ViewAngles& target, float smoothing) const {
        // Safety check to prevent division by zero
        if (smoothing <= 0.0f) {
            return target;
        }

        ViewAngles result;

        // Handle yaw wrapping
        float deltaYaw = target.Yaw - Yaw;
        while (deltaYaw > 180.0f) deltaYaw -= 360.0f;
        while (deltaYaw < -180.0f) deltaYaw += 360.0f;

        result.Pitch = Pitch + (target.Pitch - Pitch) / smoothing;
        result.Yaw = Yaw + deltaYaw / smoothing;

        return result;
    }
};
