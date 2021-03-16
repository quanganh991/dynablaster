#pragma once
#include <map>
#include "GameManager/Singleton.h"
#include "GameConfig.h"
#include "GameObject/Shaders.h"
#include "GameObject/Texture.h"
#include "GameObject/Models.h"
#include "GameObject/Font.h"
#include "GameObject/Camera.h"
#include "GameObject/Sprite2D.h"
#include "GameObject/Sprite3D.h"
#include "GameObject/Text.h"
#include "soloud.h"
#include "soloud_wav.h"

class ResourceManagers : public CSingleton<ResourceManagers>
{
public:
	ResourceManagers();
	~ResourceManagers();
	void AddShader(const std::string& name);
	void AddModel(const std::string& name);
	void AddTexture(const std::string& name);
	void AddFont(const std::string& name);

	void RemoveShader(const std::string& name);
	void RemoveModel(const std::string& name);
	void RemoveTexture(const std::string& name);
	void RemoveFont(const std::string& name);
	
	//Sound
	void AddSound(const std::string& name);
	void PlaySound(const std::string& name, bool loop = false);
	void PauseSound(const std::string& name);

	std::shared_ptr<Shaders> GetShader(const std::string& name);
	std::shared_ptr<Texture> GetTexture(const std::string& name);
	std::shared_ptr<Models> GetModel(const std::string& name);
	std::shared_ptr<Font> GetFont(const std::string& name);
private:
	std::map<std::string, std::shared_ptr<Shaders>> m_MapShader;
	std::map<std::string, std::shared_ptr<Texture>> m_MapTexture;
	std::map<std::string, std::shared_ptr<Models>> m_MapModels;
	std::map<std::string, std::shared_ptr<Font>> m_MapFont;

	//Sound
	std::string m_SoundsPath;
	std::map<std::string, std::shared_ptr<SoLoud::Wav>> m_MapWave;

	//std::shared_ptr<SoLoud::Soloud> m_Soloud;	//Engine core
	SoLoud::Soloud m_Soloud;

	std::string m_ShaderPath;
	std::string m_TexturePath;
	std::string m_ModelsPath;
	std::string m_FontPath;

};
