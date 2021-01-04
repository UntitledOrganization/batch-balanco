#pragma once

#include "Types.hpp"
#include "OpenGLShader.hpp"
#include "Texture.hpp"

namespace sbb
{
    class SpriteBatch
    {
    private:
        struct Vertex
        {
            Vec2 position;
            Vec2 textureCoord;
            Vec4 color;
            float textureIndex;
        };

        // Control variables
        bool mInitiated;
        bool mBegun;
        unsigned mTextureCount;
        unsigned mSpriteCount;

        // Screen size
        unsigned mWidth;
        unsigned mHeight;

        // Buffer sizes
        unsigned mBufferSize; // max number os sprites in buffer
        unsigned mMaxTextures;

        // Buffers
        Vertex *mVertexBuffer;
        unsigned *mElementBuffer;
        unsigned *mTextureBuffer;

        // GL dependent variables
        unsigned mVao;
        OpenGLShader *mShader;

        // Private functiosn

        void ResetTextureBuffer();
        unsigned PushInTextureBuffer(unsigned textureId);
        bool IsInTextureBuffer(unsigned textureId);
        bool IsFullTextureBuffer();

        // Draw everything on sprite buffer
        bool Flush();

        // Get max textures based on GPU
        unsigned int GetMaxTextures();

        // Init functions
        bool InitGlew();
        void InitControlVariables();
        bool InitBuffers(unsigned bufferSize);
        bool InitOpenglBuffers();
        bool InitShader();

    public:
        SpriteBatch();
        ~SpriteBatch();

        Status Init(unsigned bufferSize, unsigned width = 0, unsigned height = 0);

        Status SetWidth(unsigned value);

        Status SetHeight(unsigned value);

        Status SetClearColor(const Vec3 &color);

        Status Clear();

        Status Begin();

        Status DrawSprite(Texture &, const Rect &source, const Rect &dest);

        Status DrawRect(const Rect &, const Vec4 &color);

        Status DrawRect(const Rect &, const Vec3 &color);

        Status End();
    };

} // namespace sbb
