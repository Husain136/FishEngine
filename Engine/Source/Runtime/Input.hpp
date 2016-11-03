#ifndef Input_hpp
#define Input_hpp

#include "GLEnvironment.hpp"
#include "Vector3.hpp"
#include "Screen.hpp"
#include <glfw/glfw3.h>

namespace FishEngine
{
    enum class KeyCode {
        Alpha0 = GLFW_KEY_0, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,
        A = GLFW_KEY_A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        LeftControl = GLFW_KEY_LEFT_CONTROL,    RightControl = GLFW_KEY_RIGHT_CONTROL,
        LeftShift = GLFW_KEY_LEFT_SHIFT,        RightShift = GLFW_KEY_RIGHT_SHIFT,
        LeftAlt = GLFW_KEY_LEFT_ALT,            RightAlt = GLFW_KEY_RIGHT_ALT,
        LeftSuper = GLFW_KEY_LEFT_SUPER,        RightSuper = GLFW_KEY_RIGHT_SUPER,
    };

    enum class KeyState {
        Up = GLFW_RELEASE, // 0
        Down = GLFW_PRESS,  // 1
        Held = GLFW_REPEAT, // 2
        None = 3,
    };

    enum class MouseButtonState {
        None = 0,
        Down = 1,
        Held = 2,
        Up = 3,
    };

    enum class MouseButtonCode {
        Left = 0,
        Right = 1,
        Middle = 2,
    };

    enum class Axis {
        Vertical = 0,   // w, a, s, d and arrow keys
        Horizontal,
        Fire1,          // Control
        Fire2,          // Option(Alt)
        Fire3,          // Command
        Jump,
        MouseX,         // delta of mouse movement
        MouseY,
        MouseScrollWheel,
        WindowShakeX,   // movement of the window
        WindwoShakeY,
        AxisCount,
    };

    constexpr int keyCount = 512;

    class Input {
    public:
        Input() = delete;

        // The current mouse position in pixel coordinates. (Read Only)
        // bottom-left of the screen <==> (0, 0)
        // top-right of the screen <==> (Screen::width, Screen::height)
        static Vector3 mousePosition()
        {
            return Vector3(m_mousePositionX*Screen::width(), m_mousePositionY*Screen::height(), 0);
        }

        static Vector3 normalizedMousePosition()
        {
            return Vector3(m_mousePositionX, m_mousePositionX, 0);
        }

        // Returns the value of the virtual axis identified by axisName.
        static float GetAxis(Axis axis)
        {
            return m_axis[(int)axis];
        }

        // Returns true while the user holds down the key identified by name. Think auto fire.
        static bool GetKey(KeyCode key);

        // Returns true during the frame the user starts pressing down the key identified by name.
        static bool GetKeyDown(KeyCode key);

        // Returns true during the frame the user releases the key identified by name.
        static bool GetKeyUp(KeyCode key);

        // Returns whether the given mouse button is held down.
        // button values are 0 for left button, 1 for right button, 2 for the middle button.
        static bool GetMouseButton(int button);

        // Returns true during the frame the user pressed the given mouse button.
        // button values are 0 for left button, 1 for right button, 2 for the middle button.
        static bool GetMouseButtonDown(int button);

        // Returns true during the frame the user releases the given mouse button.
        // button values are 0 for left button, 1 for right button, 2 for the middle button.
        static bool GetMouseButtonUp(int button);

    private:
        static void Init();
        static void Update();

        static void UpdateAxis(Axis axis, float value);
        static void UpdateMousePosition(float xpos, float ypos);
        static void UpdateKeyState(int key, KeyState state);
        static void UpdateMouseButtonState(int button, MouseButtonState state);

    private:
        friend class GameLoop;
        friend class FishEditor::FishEditorWindow;

        static KeyState m_keyStates[keyCount];

        // button values are 0 for left button, 1 for right button, 2 for the middle button.
        static MouseButtonState m_mouseButtonStates[3];
        static float m_mousePositionX;  // normalized, (0, 1)
        static float m_mousePositionY;  // normalized, (0, 1)
        static float m_axis[(int)Axis::AxisCount];
    };
}

#endif // Input_hpp
