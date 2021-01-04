#include "../include/SpriteBatch.hpp"
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

#define GLX(value) (((value) / (mWidth / 2)) - 1)
#define GLY(value) (1 - ((value) / (mHeight / 2)))

namespace sbb
{

    SpriteBatch::SpriteBatch() : mInitiated(false) {}

    // Private functions

    unsigned int SpriteBatch::GetMaxTextures()
    {
        GLint maxTextures;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextures);
        return (unsigned int)maxTextures;
    }

    void SpriteBatch::InitControlVariables()
    {
        mTextureCount = 0;
        mSpriteCount = 0;
        mBegun = false;
    }

    bool SpriteBatch::InitGlew()
    {
        // Init glew
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK)
        {
            return false;
        }

        return true;
    }

    bool SpriteBatch::InitBuffers(unsigned bufferSize)
    {
        // Set sprite buffer size
        mBufferSize = bufferSize;
        // Set max textures on GPU
        // mMaxTextures = GetMaxTextures();
        // if (mMaxTextures < 0)
        //     return false;

        if ((mMaxTextures = GetMaxTextures()) < 0)
            return false;

        // Init vertex buffer (each sprite has 4 vertices)
        mVertexBuffer = new Vertex[mBufferSize * 4];

        // Init element buffer with constant values:
        // For each 4 vertices (quad), define 6 indices drawing 2 triangles
        mElementBuffer = new unsigned[mBufferSize * 6];
        for (unsigned i = 0, k = 0; k < mBufferSize * 6; i += 4, k += 6)
        {
            mElementBuffer[k + 0] = i + 0;
            mElementBuffer[k + 1] = i + 1;
            mElementBuffer[k + 2] = i + 2;
            mElementBuffer[k + 3] = i + 2;
            mElementBuffer[k + 4] = i + 3;
            mElementBuffer[k + 5] = i + 0;
        }

        // Init texture buffer
        mTextureBuffer = new unsigned[mMaxTextures];
        for (unsigned i = 0; i < mMaxTextures; i++)
        {
            mTextureBuffer[i] = 0;
        }

        return true;
    }

    bool SpriteBatch::InitOpenglBuffers()
    {
        // Generate vertex array
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);

        // Generate buffers
        GLuint buffers[2];
        glGenBuffers(2, buffers);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);         // Vertex buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]); // Element buffer

        // Allocate and copy data
        glBufferData(GL_ARRAY_BUFFER, mBufferSize * 4 * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mBufferSize * 6 * sizeof(unsigned int), &mElementBuffer[0], GL_DYNAMIC_DRAW);

        // Set attributes
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, textureCoord));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, textureIndex));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        return true;
    }

    bool SpriteBatch::InitShader()
    {
        // Set shader source code
        std::string vertexCode = "#version 330 core\n"
                                 "layout (location = 0) in vec2 	aPos;\n"
                                 "layout (location = 1) in vec2 	aTexCoord;\n"
                                 "layout (location = 2) in vec4 	aColor;\n"
                                 "layout (location = 3) in float	aTexIndex;\n"
                                 "out vec2 	vTexCoord;\n"
                                 "out float 	vTexIndex;\n"
                                 "out vec4	vColor;\n"
                                 "void main(){\n"
                                 "    vTexCoord = aTexCoord;\n"
                                 "    vTexIndex = aTexIndex;\n"
                                 "    vColor = aColor;\n"
                                 "    gl_Position = vec4(aPos, 0.0, 1.0f);\n"
                                 "    //gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0f);\n"
                                 "}\n";

        std::string fragmentCode = "#version 330 core\n"
                                   "in vec2 	vTexCoord;\n"
                                   "in float	vTexIndex;\n"
                                   "in vec4	    vColor;\n"
                                   "out vec4 	fColor;\n"
                                   "uniform sampler2D textures[" +
                                   std::to_string(mMaxTextures) +
                                   "];\n"
                                   "void main(){\n"
                                   "    int index = int(vTexIndex);\n"
                                   "    if (index < 0) fColor = vColor;\n"
                                   "    else fColor = texture(textures[index], vTexCoord);\n"
                                   "    //fColor = vec4(vTexCoord, 0.0, 1.0);\n"
                                   "    //fColor = vec4((1.0/vTexIndex), 0.0, 0.0, 1.0);\n"
                                   "}\n";

        ShaderSources source = {vertexCode, fragmentCode};
        mShader = new OpenGLShader(source);

        // Compile shader (non-definitive syntax)
        mShader->Init();

        // Set texture uniform
        for (unsigned i = 0; i < mMaxTextures; i++)
        {
            std::string uniformName = "textures[" + std::to_string(i) + "]";
            mShader->Bind();
            mShader->SetInt(uniformName.c_str(), (int)i);
        }

        return true;
    }

    bool SpriteBatch::Flush()
    {
        mShader->Bind();

        glBindVertexArray(mVao);
        glBufferSubData(GL_ARRAY_BUFFER, 0, mSpriteCount * 4 * sizeof(Vertex), &mVertexBuffer[0]);
        glDrawElements(GL_TRIANGLES, mSpriteCount * 6, GL_UNSIGNED_INT, 0);

        ResetTextureBuffer();
        mSpriteCount = 0;

        return true;
    }

    bool SpriteBatch::IsInTextureBuffer(unsigned textureId)
    {
        for (unsigned i = 0; i < mTextureCount; i++)
        {
            if (textureId == mTextureBuffer[i])
                return true;
        }
        return false;
    }

    bool SpriteBatch::IsFullTextureBuffer()
    {
        return (mTextureCount == mMaxTextures);
    }

    void SpriteBatch::ResetTextureBuffer()
    {
        // Init texture buffer
        for (unsigned i = 0; i < mTextureCount; i++)
        {
            mTextureBuffer[i] = 0;
        }
        mTextureCount = 0;
    }

    unsigned SpriteBatch::PushInTextureBuffer(unsigned textureId)
    {
        mTextureBuffer[mTextureCount] = textureId;
        return (mTextureCount++);
    }

    // Public functions

    Status SpriteBatch::Init(unsigned bufferSize, unsigned width, unsigned height)
    {
        // Set screen size
        mWidth = width;
        mHeight = height;

        InitControlVariables();

        if (!InitGlew())
            return {ERROR_SPRITE_BATCH, "Couldn't init glew."};

        if (!InitBuffers(bufferSize))
            return {ERROR_SPRITE_BATCH, "Couldn't init buffers. Is there an active opengl context?"};

        if (!InitOpenglBuffers())
            return {ERROR_SPRITE_BATCH, "Couldn't init opengl buffers."};

        if (!InitShader())
            return {ERROR_SPRITE_BATCH, "Couldn't init shader."};

        mInitiated = true;
        return {RESULT_OK, ""};
    }

    Status SpriteBatch::SetWidth(unsigned value)
    {
        if (value < 0)
            return {ERROR_SPRITE_BATCH, "Value can't be less than zero."};

        mWidth = value;
        return {RESULT_OK, ""};
    }

    Status SpriteBatch::SetHeight(unsigned value)
    {
        if (value < 0)
            return {ERROR_SPRITE_BATCH, "Value can't be less than zero."};

        mHeight = value;
        return {RESULT_OK, ""};
    }

    Status SpriteBatch::SetClearColor(const Vec3 &color)
    {
        if (!mInitiated)
            return {ERROR_SPRITE_BATCH, "SpriteBatch not initiated."};

        glClearColor(color.x, color.y, color.z, 1.0f);

        return {RESULT_OK, ""};
    }

    Status SpriteBatch::Clear()
    {
        if (!mInitiated)
            return {ERROR_SPRITE_BATCH, "SpriteBatch not initiated."};

        glClear(GL_COLOR_BUFFER_BIT);

        return {RESULT_OK, ""};
    }

    Status SpriteBatch::Begin()
    {
        if (!mInitiated)
            return {ERROR_SPRITE_BATCH, "SpriteBatch not initiated."};

        mBegun = true;

        return {RESULT_OK, ""};
    }

    Status SpriteBatch::DrawSprite(Texture &texture, const Rect &src, const Rect &dst)
    {
        if (!mInitiated)
            return {ERROR_SPRITE_BATCH, "SpriteBatch not initiated."};

        if (!mBegun)
            return {ERROR_SPRITE_BATCH, "Drawing not begun. You need to call Begin()"};

        if (mSpriteCount == mBufferSize)
        {
            if (!Flush())
                return {ERROR_SPRITE_BATCH, "Couldn't draw sprites."};
        }

        unsigned id = texture.GetId();
        unsigned index;
        if (!IsInTextureBuffer(id))
        {
            if (IsFullTextureBuffer())
            {
                if (!Flush())
                    return {ERROR_SPRITE_BATCH, "Couldn't draw sprites."};
            }

            index = PushInTextureBuffer(id);
            texture.ActivateAndBind(index);
        }

        Vec4 black = {0.0f, 0.0f, 0.0f, 0.0f};
        Vertex v0 = {{GLX(dst.x), GLY(dst.y)}, {src.x, 1 - src.y}, black, float(index)};
        Vertex v1 = {{GLX(dst.x + dst.w), GLY(dst.y)}, {src.x + src.w, 1 - src.y}, black, float(index)};
        Vertex v2 = {{GLX(dst.x + dst.w), GLY(dst.y + dst.h)}, {src.x + src.w, 1 - (src.y + src.h)}, black, float(index)};
        Vertex v3 = {{GLX(dst.x), GLY(dst.y + dst.h)}, {src.x, 1 - (src.y + src.h)}, black, float(index)};

        mVertexBuffer[mSpriteCount * 4 + 0] = v0;
        mVertexBuffer[mSpriteCount * 4 + 1] = v1;
        mVertexBuffer[mSpriteCount * 4 + 2] = v2;
        mVertexBuffer[mSpriteCount * 4 + 3] = v3;

        mSpriteCount++;

        return {RESULT_OK, ""};
    }

    Status SpriteBatch::DrawRect(const Rect &rect, const Vec4 &color)
    {
        if (!mInitiated)
            return {ERROR_SPRITE_BATCH, "SpriteBatch not initiated."};

        if (!mBegun)
            return {ERROR_SPRITE_BATCH, "Drawing not begun. You need to call Begin()"};

        if (mSpriteCount == mBufferSize)
        {
            if (!Flush())
                return {ERROR_SPRITE_BATCH, "Couldn't draw sprites."};
        }

        Vec2 origin = {0.0f, 0.0f};

        Vertex v0 = {{GLX(rect.x), GLY(rect.y)}, origin, color, -1.0f};
        Vertex v1 = {{GLX(rect.x + rect.w), GLY(rect.y)}, origin, color, -1.0f};
        Vertex v2 = {{GLX(rect.x + rect.w), GLY(rect.y + rect.h)}, origin, color, -1.0f};
        Vertex v3 = {{GLX(rect.x), GLY(rect.y + rect.h)}, origin, color, -1.0f};

        mVertexBuffer[mSpriteCount * 4 + 0] = v0;
        mVertexBuffer[mSpriteCount * 4 + 1] = v1;
        mVertexBuffer[mSpriteCount * 4 + 2] = v2;
        mVertexBuffer[mSpriteCount * 4 + 3] = v3;

        mSpriteCount++;

        return {RESULT_OK, ""};
    }

    Status SpriteBatch::DrawRect(const Rect &rect, const Vec3 &color)
    {
        Vec4 opaqueColor = {color.x, color.y, color.z, 1.0f};
        return DrawRect(rect, opaqueColor);
    }

    Status SpriteBatch::End()
    {
        if (!mInitiated)
            return {ERROR_SPRITE_BATCH, "SpriteBatch not initiated."};

        if (!mBegun)
            return {ERROR_SPRITE_BATCH, "Drawing not begun. You need to call Begin()"};

        Flush();
        mBegun = false;

        return {RESULT_OK, ""};
    }

    SpriteBatch::~SpriteBatch()
    {
    }

} // namespace sbb
