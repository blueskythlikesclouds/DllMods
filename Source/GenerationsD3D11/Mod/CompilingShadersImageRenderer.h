#pragma once

class VertexShader;
class PixelShader;
class Device;
class Texture;

class CompilingShadersImageRenderer
{
public:
    ComPtr<VertexShader> vertexShader;
    ComPtr<PixelShader> pixelShader;
    ComPtr<Texture> texture;
    float alpha;

    void render(Device* device);
};
