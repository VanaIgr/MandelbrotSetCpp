#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include"Vector.h"
#include"ShaderLoader.h"
#include"Misc.h"

#include <iostream>
#include<algorithm>
#include<cmath>

//#define FULLSCREEN

#ifdef FULLSCREEN
const uint32_t windowWidth = 1920, windowHeight = 1080;
#else
const uint32_t windowWidth = 800, windowHeight = 800;
#endif // FULLSCREEN
const float whMin = fmin(windowWidth, windowHeight);

const dvec2 windowSizeD{ windowWidth, windowHeight };
const dvec2 windowCenterWorld{ windowSizeD / 2.0 / whMin };

static struct Viewport {
    dvec2 position{ 0, 0 };
    double worldScale{ 1 };
};

static const double scalingSpeed = 0.1f;
static Viewport viewport_current{};
static Viewport viewport_desired{};
static float chasingFactor = 0.5;
static bool pan;

static bool isSample = true, changeSamplePos = true;
static dvec2 samplePos{ 0, 0 };

dvec2 mousePos(0, 0), pmousePos(0, 0);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept {
    if (key == GLFW_KEY_SPACE) {
        if (action == GLFW_PRESS) {
            isSample = !isSample;
        }
    }
    else if(key == GLFW_KEY_TAB) {
        if (action == GLFW_PRESS) {
            changeSamplePos = !changeSamplePos;
        }
    }
}

void updateSamplePos() {
    if (changeSamplePos) {
        samplePos = -viewport_current.position + ((mousePos - windowSizeD / 2.0) * viewport_current.worldScale) / whMin;
    }
}

static void cursor_position_callback(GLFWwindow* window, double mousex, double mousey) noexcept {
	pmousePos = mousePos;
    mousePos = dvec2(mousex, mousey);

    if (pan) {
        viewport_desired.position += (mousePos - pmousePos) / whMin * viewport_current.worldScale;
    }
    updateSamplePos();
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) noexcept {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        pan = action == GLFW_PRESS;
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) noexcept {
    viewport_desired.worldScale *= (1 - yoffset * scalingSpeed);
    viewport_desired.position = 
        (viewport_current.position + (windowCenterWorld - mousePos / whMin) * viewport_current.worldScale) - (windowCenterWorld - mousePos / whMin) * viewport_desired.worldScale;
    
    updateSamplePos();
}

static void updateViewport() {
    viewport_current.position = misc::dvec2_lerp(viewport_current.position, viewport_desired.position, chasingFactor);
    viewport_current.worldScale = misc::lerp<double>(viewport_current.worldScale, viewport_desired.worldScale, chasingFactor);
}

int main(void) {
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    GLFWmonitor* monitor;
#ifdef FULLSCREEN
    monitor = glfwGetPrimaryMonitor();
#else
    monitor = NULL;
#endif // !FULLSCREEN

    window = glfwCreateWindow(windowWidth, windowHeight, "Template", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //glfwWindowHint(GLFW_SAMPLES, 2);
    //glEnable(GL_MULTISAMPLE);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        glfwTerminate();
        return -1;
    }
    fprintf(stdout, "Using GLEW %s\n", glewGetString(GLEW_VERSION));

    //load shaders

    GLuint programId = glCreateProgram();
    ShaderLoader sl{};
    sl.addScreenSizeTriangleStripVertexShader();
    sl.addShaderFromProjectFileName("shaders/main_frag.shader", GL_FRAGMENT_SHADER, "Main shader");

    sl.attachShaders(programId);

    glLinkProgram(programId);
    glValidateProgram(programId);

    sl.deleteShaders();

    glUseProgram(programId);

    //callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //initial data
    glUniform2i(glGetUniformLocation(programId, "windowSize"), GLint(windowWidth), GLint(windowHeight));

    /*GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(array), array, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);*/
    //glUniform1i(glGetUniformLocation(programId, "width"), GLint(windowWidth));

    while (!glfwWindowShouldClose(window))
    {
        glUniform1d(glGetUniformLocation(programId, "zoom"), viewport_current.worldScale);
        glUniform2d(glGetUniformLocation(programId, "translation"), GLdouble(viewport_current.position.x), GLdouble(viewport_current.position.y));
        glUniform2d(glGetUniformLocation(programId, "samplePos"), GLdouble(mousePos.x), GLdouble(mousePos.y));
        glUniform1i(glGetUniformLocation(programId, "isSample"), isSample);
        glUniform2d(glGetUniformLocation(programId, "samplePos"), samplePos.x, samplePos.y);


        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::cout << err << std::endl;
        }

        //glClear(GL_COLOR_BUFFER_BIT);

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

        glfwSwapBuffers(window);

        glfwPollEvents();

        updateViewport();
    }

    glfwTerminate();
    return 0;
}

//#define bytes 4
//#define point (32 * bytes - 4)
//#define mask 0xffff
//#define halfShift 16
//
//#define byte (point / 32)
//#define offset (point % 32)
//#define offset_wrap (32 - offset)
//struct fixedPoint {
//    uint32_t data[bytes];
//
//    void intPart(uint32_t (&dst)[bytes - byte]) {
//        dst[0] = 0;
//        for (uint32_t i = 0; i < bytes - byte; i++) {
//
//            uint32_t result = data[i + byte];
//
//            dst[i] += result >> offset;
//
//            if ((i+1) < (bytes - byte) && offset != 0) {
//                dst[i + 1] = result << offset_wrap;
//            }
//        }
//    }
//
//    fixedPoint operator+(fixedPoint o) {
//        fixedPoint fp{};
//
//        uint32_t carry = 0;
//        for (uint32_t i = 0; i < bytes; i++) {
//            uint32_t result_lower = (data[i] & mask) + (o.data[i] & mask) + carry;
//            carry = (result_lower >> halfShift) & mask;
//            uint32_t result_higher = (data[i] >> halfShift) + (o.data[i] >> halfShift) + carry;
//            carry = (result_higher >> halfShift) & mask;
//
//            uint32_t result = (result_lower & mask) + (result_higher << halfShift);
//            fp.data[i] = result;
//        }
//
//        return fp;
//    }
//    fixedPoint operator*(fixedPoint o) {
//        fixedPoint fp{};
//
//        for (uint32_t i = 0; i < bytes; i++) {
//            fp.data[i] = 0;
//        }
//
//        for (uint32_t i = 0; i < bytes; i++) {
//            uint32_t lower_half = data[i] & mask;
//            uint32_t higher_half = data[i] >> halfShift;
//
//
//            uint32_t carry_lower = 0;
//            uint32_t carry_middle = 0;
//            uint32_t carry_higher = 0;
//            for (uint32_t j = 0; j < bytes ; j++) {
//
//                uint32_t result_lower_lower = 0;
//                uint32_t result_lower_higher = 0;
//                uint32_t result_higher_lower = 0;
//                uint32_t result_higher_higher = 0;
//
//                for (uint32_t b = 0; b < halfShift; b++) {
//                    uint32_t firstLower = (lower_half >> b) & 1;
//                    uint32_t firstHigher = (higher_half >> b) & 1;
//                    uint32_t secondLower = (o.data[j] & mask) << b;
//                    uint32_t secondHigher = (o.data[j] >> halfShift) << b;
//
//                    result_lower_lower +=  firstLower * secondLower;
//                    result_lower_higher +=  firstLower * secondHigher;
//                    result_higher_lower +=  firstHigher * secondLower;
//                    result_higher_higher += firstHigher * secondHigher;
//                }
//
//                uint32_t result_bottom16 = (result_lower_lower & mask) + carry_lower;
//                uint32_t carry_bottom16 = result_bottom16 >> halfShift;
//                result_bottom16 &= mask;
//
//                uint32_t result_second16 = (result_lower_lower >> halfShift) + 
//                    (result_lower_higher & mask) + 
//                    (result_higher_lower & mask) + carry_bottom16;
//                uint32_t carry_second16 = result_second16 >> halfShift;
//                result_second16 &= mask;
//
//                uint32_t result_third16 = (result_higher_higher & mask) + 
//                    (result_lower_higher >> halfShift) + 
//                    (result_higher_lower >> halfShift) + carry_second16;
//                uint32_t carry_third16 = result_third16 >> halfShift;
//                result_third16 &= mask;
//
//                uint32_t result_top16 = (result_higher_higher >> halfShift) + carry_third16;
//                carry_higher = result_top16 >> halfShift;
//                result_top16 &= mask;
//
//
//                uint32_t result_bottom32 = result_bottom16 + (result_second16 << halfShift);
//                uint32_t result_higher32 = result_third16 + (result_top16 << halfShift);
//
//                int32_t index_lower32 = int32_t(i) - int32_t(byte);
//                if (index_lower32 >= 0 && (index_lower32 < bytes)) {
//                    fp.data[index_lower32] += result_bottom32 >> offset;
//                    if ((index_lower32 > 0) && (offset != 0)) {
//                        fp.data[index_lower32 - 1] += result_bottom32 << offset_wrap;
//                    }
//                }
//
//                int32_t index_higher32 = index_lower32 + 1;
//                if ((index_higher32 >= 0) && (index_higher32 < bytes)) {
//                    fp.data[index_higher32] += result_higher32 >> offset;
//                    if ((index_higher32 > 0) && (offset != 0)) {
//                        fp.data[index_higher32 - 1] += result_higher32 << offset_wrap;
//                    }
//                }
//
//                carry_lower = carry_middle;
//                carry_middle = carry_higher;
//            }
//
//            int32_t index_carry = int32_t(i) - int32_t(byte) + 2;
//            if ((index_carry >= 0) && (index_carry < bytes)) {
//                fp.data[index_carry] += carry_higher >> offset;
//                if ((index_carry > 0) && (offset != 0)) {
//                    fp.data[index_carry - 1] += carry_higher << offset_wrap;
//                }
//            }
//        }
//
//        return fp;
//    }
//};
//
//using namespace std;
//
//fixedPoint fromDouble(double number) {
//    number *= pow(2, point);
//    fixedPoint fp{};
//
//    for (uint32_t i = 0; i < bytes; i++) {
//        fp.data[i] = uint32_t(number);
//        number /= pow(2, 32);
//    }
//
//    return fp;
//}
//
//double toDouble(fixedPoint fp) {
//    double number = 0;
//
//    for (uint32_t ip1 = bytes; ip1 > 0; ip1--) {
//        number *= pow(2, 32);
//        number += fp.data[ip1 - 1];
//
//    }
//
//    number /= pow(2, point);
//    return number;
//}
//
//void main() {
//    fixedPoint a = fromDouble(2.78675);
//    fixedPoint b = fromDouble(0.36768);
//
//    uint32_t dst[bytes - byte]{};
//    a.intPart(dst);
//
//    printf("sum:%f, prod:%f, wholePart:%d" "\n", toDouble(a + b), toDouble(a * b), dst[0]);
//}
