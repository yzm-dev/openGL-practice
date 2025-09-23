#pragma once

#include "Renderer.h"

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, nrChannels; // BPP: Bytes Per Pixel
public:
	Texture(const std::string& path, const bool& gammaCorrection);
	~Texture();

	void BindTexture(unsigned int slot = 0) const;
	void UnBindTexture() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

};