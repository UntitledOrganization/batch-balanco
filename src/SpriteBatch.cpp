#include "../include/SpriteBatch.hpp"
#include "../include/GLCall.hpp"
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

#define orthoX(value) (((value) / (mWidth / 2)) - 1)
#define orthoY(value) (1 - ((value) / (mHeight / 2)))

#define texOrthoX(value) ((value) / texture.GetWidth())
#define texOrthoY(value) (1 - ((value) / texture.GetHeight()))

namespace sbb
{

    SpriteBatch::SpriteBatch() : mInitiated(false) {}

    // Private functions

    unsigned int SpriteBatch::GetMaxTextures()
    {
        GLint maxTextures;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
        return (unsigned int)maxTextures;
    }

    void SpriteBatch::InitControlVariables()
    {
        mTextureCount = 0;
        mSpriteCount = 0;
        mBegun = false;
    }

    Status SpriteBatch::InitGlew()
    {
        // Init glew
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        const char *errorString = (const char *)glewGetErrorString(glewError);
        if (glewError != GLEW_OK)
        {

            return {ERROR_SPRITE_BATCH, std::string(errorString)};
        }

        return {RESULT_OK, ""};
    }

    bool SpriteBatch::InitBuffers(unsigned bufferSize)
    {
        // Set sprite buffer size
        mBufferSize = bufferSize;

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

    Status SpriteBatch::InitOpenglBuffers()
    {
        Status glStatus;

        // Generate vertex array
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);

        // Generate buffers
        GLuint buffers[2];
        glGenBuffers(2, buffers);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);         // Vertex buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]); // Element buffer

        // Allocate and copy data
        GLCall(glBufferData(GL_ARRAY_BUFFER, mBufferSize * 4 * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW));
        if (glStatus = GLCheckError(), !glStatus)
            return {ERROR_OPENGL, "Couldn't create vertex buffer. " + glStatus.message};

        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mBufferSize * 6 * sizeof(unsigned int), &mElementBuffer[0], GL_DYNAMIC_DRAW));
        if (glStatus = GLCheckError(), !glStatus)
            return {ERROR_OPENGL, "Couldn't create element buffer. " + glStatus.message};

        // Set attributes
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, textureCoord));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, textureIndex));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        return {RESULT_OK, ""};
    }

    Status SpriteBatch::InitShader()
    {
        // Set shader source code
        std::string vertexCode = "#version 400 core\n"
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
                                 "    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0f);\n"
                                 "}\n";

        std::string fragmentCode = "#version 400 core\n"
                                   "in vec2 	vTexCoord;\n"
                                   "in float	vTexIndex;\n"
                                   "in vec4	    vColor;\n"
                                   "out vec4 	fColor;\n"
                                   "uniform sampler2D textures[" +
                                   std::to_string(mMaxTextures) +
                                   "];\n"
                                   "void main(){\n"
                                   "    int index = int(vTexIndex);\n"
                                   "    fColor = vColor * texture(textures[index], vTexCoord);\n"
                                   "}\n";

        ShaderSources source = {vertexCode, fragmentCode};
        mShader = new OpenGLShader(source);

        // Compile shader

        Status shaderInitStatus = mShader->Init();
        if (!shaderInitStatus)
            return shaderInitStatus;

        // Set texture uniform
        for (unsigned i = 0; i < mMaxTextures; i++)
        {
            std::string uniformName = "textures[" + std::to_string(i) + "]";
            mShader->Bind();

            Status uniformStatus = mShader->SetInt(uniformName.c_str(), (int)i);
            if (!uniformStatus)
                return uniformStatus;
        }

        return {RESULT_OK, ""};
    }

    Status SpriteBatch::Flush()
    {
        Status glStatus;

        mShader->Bind();

        glBindVertexArray(mVao);
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, mSpriteCount * 4 * sizeof(Vertex), &mVertexBuffer[0]));
        if (glStatus = GLCheckError(), !glStatus)
            return {ERROR_OPENGL, "Couldn't flush draw buffer. " + glStatus.message};
        glDrawElements(GL_TRIANGLES, mSpriteCount * 6, GL_UNSIGNED_INT, 0);

        ResetTextureBuffer();
        mSpriteCount = 0;

        return {RESULT_OK, ""};
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

    int SpriteBatch::FindInTextureBuffer(unsigned textureId)
    {
        for (unsigned i = 0; i < mTextureCount; i++)
        {
            if (textureId == mTextureBuffer[i])
                return i;
        }
        return -1;
    }

    bool SpriteBatch::IsFullTextureBuffer()
    {
        return (mTextureCount == mMaxTextures);
    }

    void SpriteBatch::ResetTextureBuffer()
    {
        // Init texture buffer
        for (unsigned i = 1; i < mTextureCount; i++)
        {
            mTextureBuffer[i] = 0;
        }
        mTextureCount = 1;

        // Reset white texture
        mWhiteTexture.ActivateAndBind(0);
    }

    unsigned SpriteBatch::PushInTextureBuffer(unsigned textureId)
    {
        mTextureBuffer[mTextureCount] = textureId;
        return (mTextureCount++);
    }

    // Public functions

    Status SpriteBatch::Init(unsigned width, unsigned height, unsigned nMaxSprites)
    {
        // Set screen size
        mWidth = width;
        mHeight = height;

        InitControlVariables();

        if (!InitGlew())
            return {ERROR_SPRITE_BATCH, "Couldn't init glew."};

        if (!InitBuffers(nMaxSprites))
            return {ERROR_SPRITE_BATCH, "Couldn't init buffers. Is there an active opengl context?"};

        Status glBufferStatus = InitOpenglBuffers();
        if (!glBufferStatus)
            return {glBufferStatus.type, "Couldn't init opengl buffers. " + glBufferStatus.message};

        Status shaderStatus = InitShader();
        if (!shaderStatus)
            return shaderStatus;

        // Set white texture
        unsigned char data[3] = {255, 255, 255};
        mWhiteTexture.Load(data, 1, 1, 3);
        PushInTextureBuffer(mWhiteTexture.GetId());
        mWhiteTexture.ActivateAndBind(0);

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
        Status flushStatus;

        if (!mInitiated)
            return {ERROR_SPRITE_BATCH, "SpriteBatch not initiated."};

        if (!mBegun)
            return {ERROR_SPRITE_BATCH, "Drawing not begun. You need to call Begin()"};

        if (mSpriteCount == mBufferSize)
        {
            if (flushStatus = Flush(), !flushStatus)
                return flushStatus;
        }

        // std::cout << "Go 1\n";

        unsigned id = texture.GetId();
        int index = FindInTextureBuffer(id);
        if (index < 0)
        {
            if (IsFullTextureBuffer())
            {
                // std::cout << "Textures is full \n";
                if (flushStatus = Flush(), !flushStatus)
                    return flushStatus;
            }

            index = PushInTextureBuffer(id);
            texture.ActivateAndBind(index);
        }

        // std::cout << "Index = " << index << "\n";
        // std::cout << "Id = " << id << "\n\n";

        Vec4 white = {1.0f, 1.0f, 1.0f, 1.0f};
        Vertex v0 = {{orthoX(dst.x), orthoY(dst.y)},
                     {texOrthoX(src.x), texOrthoY(src.y)},
                     white,
                     float(index)};
        Vertex v1 = {{orthoX(dst.x + dst.w), orthoY(dst.y)},
                     {texOrthoX(src.x + src.w), texOrthoY(src.y)},
                     white,
                     float(index)};
        Vertex v2 = {{orthoX(dst.x + dst.w), orthoY(dst.y + dst.h)},
                     {texOrthoX(src.x + src.w), texOrthoY(src.y + src.h)},
                     white,
                     float(index)};
        Vertex v3 = {{orthoX(dst.x), orthoY(dst.y + dst.h)},
                     {texOrthoX(src.x), texOrthoY(src.y + src.h)},
                     white,
                     float(index)};

        // std::cout << "V0 = (" << v0.textureCoord.x << ", " << v0.textureCoord.y << ")\n";
        // std::cout << "V1 = (" << v1.textureCoord.x << ", " << v1.textureCoord.y << ")\n";
        // std::cout << "V2 = (" << v2.textureCoord.x << ", " << v2.textureCoord.y << ")\n";
        // std::cout << "V3 = (" << v3.textureCoord.x << ", " << v3.textureCoord.y << ")\n";

        // std::cout << "Go 3\n";

        mVertexBuffer[mSpriteCount * 4 + 0] = v0;
        mVertexBuffer[mSpriteCount * 4 + 1] = v1;
        mVertexBuffer[mSpriteCount * 4 + 2] = v2;
        mVertexBuffer[mSpriteCount * 4 + 3] = v3;

        mSpriteCount++;

        // std::cout << "Go 4\n";

        return {RESULT_OK, ""};
    }

    Status SpriteBatch::DrawRect(const Rect &rect, const Vec4 &color)
    {
        if (!mInitiated)
            return {ERROR_SPRITE_BATCH, "SpriteBatch not initiated."};

        if (!mBegun)
            return {ERROR_SPRITE_BATCH, "Drawing not begun. You need to call Begin()"};

        Status flushStatus;
        if (mSpriteCount == mBufferSize)
        {
            if (flushStatus = Flush(), !flushStatus)
                return flushStatus;
        }

        Vec2 origin = {0.0f, 0.0f};

        Vertex v0 = {{orthoX(rect.x), orthoY(rect.y)},
                     origin,
                     color,
                     0.0f};
        Vertex v1 = {{orthoX(rect.x + rect.w), orthoY(rect.y)},
                     origin,
                     color,
                     0.0f};
        Vertex v2 = {{orthoX(rect.x + rect.w), orthoY(rect.y + rect.h)},
                     origin,
                     color,
                     0.0f};
        Vertex v3 = {{orthoX(rect.x), orthoY(rect.y + rect.h)},
                     origin,
                     color,
                     0.0f};

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

        Status flushStatus = Flush();

        mBegun = false;

        return flushStatus;
    }

    SpriteBatch::~SpriteBatch()
    {
    }

} // namespace sbb
