#version 450

#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

uniform ivec2 windowSize;

uniform bool isSample;
uniform dvec2 samplePos;

uniform double zoom;
uniform dvec2 translation;

layout(origin_upper_left) in vec4 gl_FragCoord;
out vec4 color;

struct complex {
    double r, i;
};

complex c_mul(complex c1, complex c2) {
    return complex(c1.r * c2.r - c1.i * c2.i, c1.r * c2.i + c1.i * c2.r);
}

complex c_add(complex c1, complex c2) {
    return complex(c1.r + c2.r, c1.i + c2.i);
}

double c_sqLen(complex c) {
    return c.r * c.r + c.i * c.i;
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

double lerp(double a, double b, double f) {
    return a * (1 - f) + f * b;
}

dvec2 applyTransformations(dvec2 coord) {
    double whMin = min(windowSize.x, windowSize.y);
    coord -= windowSize / 2.0;
    coord *= zoom;
    //coord += windowSize / 2.0;
    coord /= whMin;
    coord -= translation;
    return coord;
}

void main(void) {
    double whMin = min(windowSize.x, windowSize.y);
    const dvec2 coord = applyTransformations(gl_FragCoord.xy);

    complex offset;
    complex z;
    if (isSample) {
        const dvec2 tm = samplePos;
        offset = complex(tm.x, tm.y);
        z = complex(coord.x, coord.y);
    }
    else {
        offset = complex(coord.x, coord.y);
        z = complex(0, 0);
    }

    const uint iterations = 200;
    const double sqR = 4;

    uint exceedIter = iterations;
    for (uint i = 0; i < iterations; i++) {
        z = c_add(c_mul(z, z), offset);

        if((z.r > 2 || z.i > 2 || c_sqLen(z) >= sqR) && exceedIter > i){
            exceedIter = i;
        }
    }

    if (exceedIter == iterations) {
        color = vec4(0, 0, 0, 1);
    }
    else {
        color = vec4(
            hsv2rgb(
              vec3(
                  mod(lerp(.7, -.3, exceedIter / double(iterations)), 1.0),
                  1,
                  lerp(.7, 1, exceedIter / double(iterations))
               )
            ), 
           1);
    }

    
    //color = vec4(gl_FragCoord.xy / windowSize.xy, 0.0, 1.0);
}