/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef SF_GLES20RENDERENGINE_H_
#define SF_GLES20RENDERENGINE_H_

#include <stdint.h>
#include <sys/types.h>

#include <GLES2/gl2.h>
#include <Transform.h>

#include "RenderEngine.h"
#include "ProgramCache.h"
#include "Description.h"

// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------

class String8;
class Mesh;
class Texture;

class GLES20RenderEngine : public RenderEngine {
    GLuint mProtectedTexName;
    GLint mMaxViewportDims[2];
    GLint mMaxTextureSize;
    GLuint mVpWidth;
    GLuint mVpHeight;

    struct Group {
        GLuint texture;
        GLuint fbo;
        GLuint width;
        GLuint height;
        mat4 colorTransform;
    };

    Description mState;
    Vector<Group> mGroupStack;

    virtual void bindImageAsFramebuffer(EGLImageKHR image,
            uint32_t* texName, uint32_t* fbName, uint32_t* status,
            bool useReadPixels, int reqWidth, int reqHeight);
    virtual void unbindFramebuffer(uint32_t texName, uint32_t fbName, bool useReadPixels);

public:
    GLES20RenderEngine(uint32_t featureFlags); // See RenderEngine::FeatureFlag

protected:
    virtual ~GLES20RenderEngine();

    virtual void dump(String8& result);
    virtual void setViewportAndProjection(size_t vpw, size_t vph,
            Rect sourceCrop, size_t hwh, bool yswap,
            Transform::orientation_flags rotation);
#ifdef USE_HWC2
    virtual void setupLayerBlending(bool premultipliedAlpha, bool opaque,
            float alpha) override;
    virtual void setupDimLayerBlending(float alpha) override;

    // Color management related functions and state
    void setColorMode(android_color_mode mode);
    void setSourceDataSpace(android_dataspace source);
    void setWideColor(bool hasWideColor);
    bool usesWideColor();

    // Current color mode of display using the render engine
    android_color_mode mColorMode = HAL_COLOR_MODE_NATIVE;

    // Current dataspace of layer being rendered
    android_dataspace mDataSpace = HAL_DATASPACE_V0_SRGB;

    // Indicate if wide-color mode is needed or not
    bool mDisplayHasWideColor = false;
    bool mUseWideColor = false;
    uint64_t mWideColorFrameCount = 0;

    // Currently only supporting sRGB and DisplayP3 color spaces
    mat4 mSrgbToDisplayP3;
#else
    virtual void setupLayerBlending(bool premultipliedAlpha, bool opaque,
            int alpha);
    virtual void setupDimLayerBlending(int alpha);
#endif
    bool mPlatformHasWideColor = false;

    virtual void setupLayerTexturing(const Texture& texture);
    virtual void setupLayerBlackedOut();
    virtual void setupFillWithColor(float r, float g, float b, float a);
    virtual mat4 setupColorTransform(const mat4& colorTransform);
    virtual void disableTexturing();
    virtual void disableBlending();

    virtual void drawMesh(const Mesh& mesh);

    virtual size_t getMaxTextureSize() const;
    virtual size_t getMaxViewportDims() const;
};

// ---------------------------------------------------------------------------
}; // namespace android
// ---------------------------------------------------------------------------

#endif /* SF_GLES20RENDERENGINE_H_ */
