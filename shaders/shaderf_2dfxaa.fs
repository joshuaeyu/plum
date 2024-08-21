#version 410 core

// Adapted from https://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

float rgb2luma(vec3 rgb) {
    return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

float QUALITY(int i) {
    float values[7] = float[](1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0);
    if (i <= 6) {
        return values[i-1];
    } else {
        return values[6];
    }
}

vec4 fxaa(float EDGE_THRESHOLD_MIN, float EDGE_THRESHOLD_MAX, float SUBPIXEL_QUALITY, int ITERATIONS) {
    vec2 texelStep = 1.0 / textureSize(screenTexture, 0);

    vec3 colorCenter = texture(screenTexture, TexCoords).rgb;
    float lumaCenter = rgb2luma(colorCenter);

    float lumaDown  = rgb2luma( textureOffset(screenTexture, TexCoords, ivec2(0,-1)).rgb );
    float lumaUp    = rgb2luma( textureOffset(screenTexture, TexCoords, ivec2(0, 1)).rgb );
    float lumaLeft  = rgb2luma( textureOffset(screenTexture, TexCoords, ivec2(-1,0)).rgb );
    float lumaRight = rgb2luma( textureOffset(screenTexture, TexCoords, ivec2(1, 0)).rgb );

    // (1) Determine whether to perform AA on this pixel
    float lumaMin = min( lumaCenter, min( min(lumaDown,lumaUp),min(lumaLeft,lumaRight) ) );
    float lumaMax = max( lumaCenter, max( max(lumaDown,lumaUp),max(lumaLeft,lumaRight) ) );

    float lumaRange = lumaMax - lumaMin;

    if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax*EDGE_THRESHOLD_MAX)) {
        return vec4(colorCenter, 1.0);
    }

    // (2) Determine edge orientation
    float lumaDownLeft  = rgb2luma( textureOffset(screenTexture, TexCoords, ivec2(-1,-1)).rgb );
    float lumaDownRight = rgb2luma( textureOffset(screenTexture, TexCoords, ivec2( 1,-1)).rgb );
    float lumaUpLeft    = rgb2luma( textureOffset(screenTexture, TexCoords, ivec2(-1, 1)).rgb );
    float lumaUpRight   = rgb2luma( textureOffset(screenTexture, TexCoords, ivec2( 1, 1)).rgb );

    float edgeHorizontal = abs((lumaUpLeft-lumaLeft)-(lumaLeft-lumaDownLeft)) + 2*abs((lumaUp-lumaCenter)-(lumaCenter-lumaDown)) + abs((lumaUpRight-lumaRight)-(lumaRight-lumaDownRight));  // Basically how different the rows of pixels are 
    float edgeVertical   = abs((lumaUpLeft-lumaUp)-(lumaUp-lumaUpRight)) + 2*abs((lumaLeft-lumaCenter)-(lumaCenter-lumaRight)) + abs((lumaDownLeft-lumaDown)-(lumaDown-lumaDownRight));  // Basically how different the columns of pixels are

    bool isHorizontal = (edgeHorizontal >= edgeVertical);   // Tie (perfect diagonal edge) is labeled as horizontal

    // (3) Determine edge location and shift UV half a pixel towards it
    float luma1 = isHorizontal ? lumaDown : lumaLeft;
    float luma2 = isHorizontal ? lumaUp : lumaRight;

    float gradient1 = luma1 - lumaCenter;
    float gradient2 = luma2 - lumaCenter;

    bool is1Steepest = (abs(gradient1) >= abs(gradient2));  // Tie (equal gradients) goes to lower or left edge

    float gradientScaled = 0.25 * max( abs(gradient1), abs(gradient2) );  // Adjust scaling factor to make edge traversal termination criteria more/less strict

    float stepLength = isHorizontal ? texelStep.y : texelStep.x;

    float lumaLocalAverage;
    if (is1Steepest) {
        stepLength = -stepLength;   // If offsetting up or left
        lumaLocalAverage = 0.5 * (luma1 + lumaCenter);
    } else {
        lumaLocalAverage = 0.5 * (luma2 + lumaCenter);
    }

    vec2 currentUv = TexCoords;
    if (isHorizontal) {
        currentUv.y += 0.5 * stepLength;
    } else {
        currentUv.x += 0.5 * stepLength;
    }

    // (4) Explore along (axis of) edge
    vec2 offset = isHorizontal ? vec2(texelStep.x, 0) : vec2(0, texelStep.y);

    vec2 uv1 = currentUv - offset;
    vec2 uv2 = currentUv + offset;

    float lumaEnd1 = rgb2luma( texture(screenTexture, uv1).rgb );
    float lumaEnd2 = rgb2luma( texture(screenTexture, uv2).rgb );
    lumaEnd1 -= lumaLocalAverage;   // Delta luma 1
    lumaEnd2 -= lumaLocalAverage;   // Delta luma 2

    bool reached1 = (abs(lumaEnd1) >= gradientScaled);  // Compare delta_luma(local_edge) with delta_luma(center, extremity1)
    bool reached2 = (abs(lumaEnd2) >= gradientScaled);  // Compare delta_luma(local_edge) with delta_luma(center, extremity2)
    bool reachedBoth = (reached1 && reached2);

    if (!reached1) {
        uv1 -= offset;
    }
    if (!reached2) {
        uv2 += offset;
    }

    // (5) Continue exploring until delta luma is greater than local gradient
    if (!reachedBoth) {
        for (int i = 1; i < ITERATIONS; i++) {  // why not i = 1 to start
            if (!reached1) {
                lumaEnd1 = rgb2luma( texture(screenTexture, uv1).rgb );
                lumaEnd1 -= lumaLocalAverage;   // Delta luma 1
            }
            if (!reached2) {
                lumaEnd2 = rgb2luma( texture(screenTexture, uv2).rgb );
                lumaEnd2 -= lumaLocalAverage;   // Delta luma 2
            }

            reached1 = (abs(lumaEnd1) >= gradientScaled);
            reached2 = (abs(lumaEnd2) >= gradientScaled);
            reachedBoth = (reached1 && reached2);

            if (!reached1) { uv1 -= offset * QUALITY(i); }
            if (!reached2) { uv2 += offset * QUALITY(i); }
            if (reachedBoth)
                break;
        }
    }

    // (6) Estimate final UV offset to use
    float distance1 = isHorizontal ? (TexCoords.x - uv1.x) : (TexCoords.y - uv1.y);
    float distance2 = isHorizontal ? (uv2.x - TexCoords.x) : (uv2.y - TexCoords.y);

    bool isDirection1 = (distance1 < distance2);
    float distanceFinal = min(distance1, distance2);

    float edgeLength = distance1 + distance2;

    float pixelOffset = -distanceFinal / edgeLength + 0.5;      // How is this derived

    bool isCenterSmaller = lumaCenter < lumaLocalAverage;
    bool correctVariation = ( ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isCenterSmaller );    // Why?

    float finalOffset = correctVariation ? pixelOffset : 0.0;

    // (7) Subpixel antialiasing
    float lumaAverage = 1.0/12.0 * (lumaUpLeft + 2*lumaUp + lumaUpRight + 2*lumaLeft + 2*lumaRight + lumaDownLeft + 2*lumaDown + lumaDownRight);
    float subPixelOffset1 = clamp( abs(lumaAverage-lumaCenter)/lumaRange, 0.0, 1.0 );
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;     // ??
    float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

    // (8) Final UV and color
    finalOffset = max(finalOffset, subPixelOffsetFinal);

    vec2 finalUv = TexCoords;
    if (isHorizontal) {
        finalUv.y += finalOffset * stepLength;
    } else {
        finalUv.x += finalOffset * stepLength;
    }
    // return vec4(1);
    return texture(screenTexture, finalUv);
}

void main() {
    // Constants
    float EDGE_THRESHOLD_MIN = 0.0312;
    float EDGE_THRESHOLD_MAX = 0.125;
    float SUBPIXEL_QUALITY = 0.75;
    int ITERATIONS = 12;
    // if (gl_FragCoord.x < 3024/4) {
    //     FragColor = fxaa(EDGE_THRESHOLD_MIN, EDGE_THRESHOLD_MAX, SUBPIXEL_QUALITY, ITERATIONS);
    // // } else if (gl_FragCoord.x > 3024/4 - 4 && gl_FragCoord.x < 3024/4 + 4) {
    // //     FragColor = vec4(1);
    // } else {
    //     FragColor = fxaa(EDGE_THRESHOLD_MIN, EDGE_THRESHOLD_MAX, 0.75, ITERATIONS);
    //     // FragColor = texture(screenTexture, TexCoords);
    // }
    FragColor = fxaa(EDGE_THRESHOLD_MIN, EDGE_THRESHOLD_MAX, SUBPIXEL_QUALITY, ITERATIONS);
}