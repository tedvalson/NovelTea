#include <NovelTea/ProjectData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/StringUtils.hpp>
#include <NovelTea/Zip/Zip.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <fstream>
#include <iostream>

namespace NovelTea
{

static const std::string defaultFragShader = " \
	precision mediump float; \
	uniform sampler2D texture; \
	varying vec4 v_color; \
	varying vec2 v_texCoord; \
	void main() { \
		gl_FragColor = v_color * texture2D(texture, v_texCoord.st); \
	}";

static const std::string defaultVertexShader = " \
	precision mediump float; \
	uniform mat4 projMatrix; \
	uniform mat4 textMatrix; \
	uniform mat4 viewMatrix; \
	attribute vec4 color; \
	attribute vec2 position; \
	attribute vec2 texCoord; \
	varying vec4 v_color; \
	varying vec2 v_texCoord; \
	void main() { \
		gl_Position = projMatrix * viewMatrix * vec4(position, 0.0, 1.0); \
		v_texCoord = (textMatrix * vec4(texCoord, 0.0, 1.0)).xy; \
		v_color = color; \
	}";

ProjectData::ProjectData()
: m_loaded(false)
{
	newProject();
}

ProjectData::ProjectData(const std::string &fileName)
{
	loadFromFile(fileName);
}

ProjectData::~ProjectData()
{

}

void ProjectData::newProject()
{
	auto j = json({
		ID::engineVersion, NOVELTEA_VERSION,
		ID::projectName, "Project Name",
		ID::projectVersion, "1.0",
		ID::projectAuthor, "Author Name",
		ID::projectWebsite, "",
		ID::projectFontDefault, "sys",
		ID::projectFonts, sj::Array(),
		ID::startingInventory, sj::Array(),
		ID::scriptBeforeSave, "",
		ID::scriptAfterLoad, "",
		ID::scriptAfterAction, "",
		ID::scriptBeforeAction, "return true;",
		ID::scriptUndefinedAction, "return false;",
		ID::scriptAfterLeave, "",
		ID::scriptBeforeLeave, "return true;",
		ID::scriptAfterEnter, "",
		ID::scriptBeforeEnter, "return true;",
		ID::openTabs, sj::Array(),
		ID::openTabIndex, -1,
		ID::textures, sj::Array(),
	});

	j[ID::shaders] = json({
		"defaultFrag", defaultFragShader,
		"defaultVert", defaultVertexShader,
	});
	j[ID::systemShaders] = sj::Array(
		"defaultFrag", // Background
		"defaultFrag"  // Post Process
	);
	j[ID::engineFonts] = json({
		"sys", "LiberationSans.ttf",
		"sysIcon", "fontawesome.ttf",
	});
	fromJson(j);
	m_loaded = false;
}

void ProjectData::closeProject()
{
	m_json = sj::Array();
	m_loaded = false;
	m_fileName.clear();
}

void ProjectData::clearFilename()
{
	m_fileName.clear();
}

bool ProjectData::isLoaded() const
{
	return m_loaded;
}

bool ProjectData::isValid(std::string &errorMessage) const
{
	auto entryPoint = sj::Array(-1, "");
	if (m_json.hasKey(ID::entrypointEntity))
		entryPoint = m_json[ID::entrypointEntity];
//	auto entryIdName = entryPoint.value(NT_ENTITY_ID, "");
	if (entryPoint[1].IsEmpty())
	{
		errorMessage = "No valid entry point defined in project settings.";
		return false;
	}

	return true;
}

void renameJsonEntity(json &jentity, EntityType entityType, const std::string &oldName, const std::string &newName)
{
	if (jentity[0].ToInt() == static_cast<int>(entityType) && jentity[1].ToString() == oldName)
		jentity[1] = newName;
}

void ProjectData::renameEntity(Context *context, EntityType entityType, const std::string &oldName, const std::string &newName)
{
	if (oldName == newName || entityType == EntityType::Invalid)
		return;
	auto entityId = entityTypeToId(entityType);

	// Perform the actual renaming
	auto &j = m_json[entityId];
	j[newName] = j[oldName];
	j[newName][0] = newName;
	j.erase(oldName);

	// Rename parent refs
	for (auto &entityPair : j.ObjectRange())
	{
		auto &jparentId = entityPair.second[1];
		if (jparentId.ToString() == oldName)
			jparentId = newName;
	}

	// Project settings
	renameJsonEntity(m_json[ID::entrypointEntity], entityType, oldName, newName);

	// Actions
	if (entityType == EntityType::Object || entityType == EntityType::Verb)
	{
		auto action = std::make_shared<Action>(context);
		for (auto &actionPair : m_json[Action::id].ObjectRange())
		{
			action->fromJson(actionPair.second);
			auto jcombo = action->getVerbObjectCombo();
			if (entityType == EntityType::Verb && jcombo[0].ToString() == oldName)
				jcombo[0] = newName;
			else if (entityType == EntityType::Object)
				for (auto &jobject : jcombo[1].ArrayRange())
					if (jobject.ToString() == oldName)
						jobject = newName;
			action->setVerbObjectCombo(jcombo);
			m_json[Action::id][actionPair.first] = action->toJson();
		}
	}

	// Cutscenes
	auto cutscene = std::make_shared<Cutscene>(context);
	for (auto &pair : m_json[Cutscene::id].ObjectRange())
	{
		cutscene->fromJson(pair.second);
		auto jentity = cutscene->getNextEntityJson();
		renameJsonEntity(jentity, entityType, oldName, newName);
		cutscene->setNextEntityJson(jentity);
		m_json[Cutscene::id][pair.first] = cutscene->toJson();
	}

	// Dialogues
	auto dialogue = std::make_shared<Dialogue>(context);
	for (auto &pair : m_json[Dialogue::id].ObjectRange())
	{
		dialogue->fromJson(pair.second);
		auto jentity = dialogue->getNextEntityJson();
		renameJsonEntity(jentity, entityType, oldName, newName);
		dialogue->setNextEntityJson(jentity);
		m_json[Dialogue::id][pair.first] = dialogue->toJson();
	}

	// Rooms
	auto room = std::make_shared<Room>(context);
	for (auto &roomPair : m_json[Room::id].ObjectRange())
	{
		room->fromJson(roomPair.second);

		if (entityType == EntityType::Object)
		{
			auto objects = room->getObjects();
			for (auto &object : objects)
				if (object.idName == oldName)
					object.idName = newName;
			room->setObjects(objects);

			// Rename object refs in room description
			auto d = room->getDescriptionRaw();
			room->setDescriptionRaw(replace(d, "|"+oldName+"]]", "|"+newName+"]]"));
		}
		else
		{
			auto paths = room->getPaths();
			for (auto &jpath : paths.ArrayRange())
				renameJsonEntity(jpath[1], entityType, oldName, newName);
			room->setPaths(paths);
		}

		m_json[Room::id][roomPair.first] = room->toJson();
	}

	// Tests
	for (auto &pair : m_json[ID::tests].ObjectRange())
	{
		auto &j = pair.second;
		renameJsonEntity(j[ID::entrypointEntity], entityType, oldName, newName);
		if (entityType == EntityType::Object)
			for (auto &jobjectId : j[ID::startingInventory].ArrayRange())
				if (jobjectId.ToString() == oldName)
					jobjectId = newName;
		if (entityType == EntityType::Object || entityType == EntityType::Verb)
			for (auto &jstep : j[ID::testSteps].ArrayRange())
				if (jstep["type"].ToString() == "action")
				{
					if (entityType == EntityType::Object) {
						for (auto &jobjectId : jstep["objects"].ArrayRange())
							if (jobjectId.ToString() == oldName)
								jobjectId = newName;
					} else {
						auto &verbId = jstep["verb"];
						if (verbId.ToString() == oldName)
							verbId = newName;
					}
				}
	}
}

void ProjectData::setFontData(const std::string &alias, const std::string &data)
{
	m_fontsData[alias] = data;
}

const std::string &ProjectData::getFontData(const std::string &alias) const
{
	return m_fontsData.at(alias);
}

std::shared_ptr<sf::Font> ProjectData::getFont(const std::string &fontName) const
{
	if (m_fonts.find(fontName) == m_fonts.end())
		return m_fonts.at("sys");
	return m_fonts.at(fontName);
}

void ProjectData::setTextureData(const std::string &name, const std::string &data)
{
	m_texturesData[name] = data;
}

const std::string &ProjectData::getTextureData(const std::string &name) const
{
	return m_texturesData.at(name);
}

std::shared_ptr<sf::Texture> ProjectData::getTexture(const std::string &textureName) const
{
	if (m_textures.find(textureName) == m_textures.end())
		return nullptr;
	return m_textures.at(textureName);
}

std::shared_ptr<sf::Shader> ProjectData::getShader(const std::string &fragShaderId, const std::string &vertShaderId)
{
	auto& shaders = data()[ID::shaders];
	auto shader = std::make_shared<sf::Shader>();
	auto fragShader = shaders[shaders.hasKey(fragShaderId) ? fragShaderId : "defaultFrag"];
	auto vertShader = shaders[shaders.hasKey(vertShaderId) ? vertShaderId : "defaultVert"];
	if (shader->loadFromMemory(vertShader[0].ToString(), fragShader[0].ToString()))
	{
		auto loadUniforms = [this, &shader](json &jshader) {
			for (auto& j : jshader.ObjectRange()) {
				// Uniforms is either a float or a string (with texture id)
				bool ok;
				float value = j.second.ToFloat(ok);
				if (ok)
					shader->setUniform(j.first, value);
				else {
					auto texture = getTexture(j.second.ToString());
					if (texture) {
						shader->setUniform(j.first, *texture);
						texture->setRepeated(true);
						shader->setUniform(j.first + "Matrix", texture->getMatrix(sf::Texture::Normalized));
					}
				}
			}
		};

		shader->setUniform("texture", sf::Shader::CurrentTexture);
		loadUniforms(fragShader[1]);
		loadUniforms(vertShader[1]);
	}
	return shader;
}

std::shared_ptr<sf::Shader> ProjectData::getShader(int systemShaderIndex)
{
	return getShader(data()[ID::systemShaders][systemShaderIndex].ToString());
}

void ProjectData::saveToFile(const std::string &fileName)
{
	if (fileName.empty() && m_fileName.empty())
		return;
	if (!fileName.empty())
		m_fileName = fileName;

	auto j = toJson();
//	json::to_msgpack(j, file);
	ZipWriter zip(m_fileName);
	zip.write("game", j.dump());
	zip.write("image", m_imageData);

	for (auto& jfont : j[ID::projectFonts].ObjectRange()) {
		zip.write("fonts/" + jfont.second.ToString(), m_fontsData[jfont.first]);
	}
	for (auto& jtexture : j[ID::textures].ObjectRange()) {
		zip.write("textures/" + jtexture.first, m_texturesData[jtexture.first]);
	}
}

bool ProjectData::loadFromFile(const std::string &fileName)
{
	try
	{
		std::string s;

		sf::FileInputStream file;
		if (!file.open(fileName))
			return false;
		s.resize(file.getSize());
		file.read(&s[0], s.size());

		ZipReader zip(s.data(), s.size());

//		auto j = json::from_msgpack(file);
//		auto j = json::parse(file);

		auto j = json::Load(zip.read("game"));
		auto success = fromJson(j);
		if (success) {
			// Load fonts
			for (auto& jfont : j[ID::projectFonts].ObjectRange()) {
				auto& alias = jfont.first;
				auto font = std::make_shared<sf::Font>();
				m_fontsData[alias] = zip.read("fonts/" + jfont.second.ToString());
				auto &data = m_fontsData[alias];
				if (font->loadFromMemory(data.data(), data.size()))
					m_fonts[alias] = font;
				else {
					std::cerr << "Failed to load project font: " << jfont.second.ToString() << std::endl;
					auto &defaultFont = m_json[ID::projectFontDefault];
					if (alias == defaultFont.ToString())
						defaultFont = "sys";
				}
			}
			// Load textures
			for (auto& jtexture : j[ID::textures].ObjectRange()) {
				auto& name = jtexture.first;
				auto texture = std::make_shared<sf::Texture>();
				m_texturesData[name] = zip.read("textures/" + jtexture.first);
				auto &data = m_texturesData[name];
				texture->flip(true);
				texture->setRepeated(true);
				if (texture->loadFromMemory(data.data(), data.size())) {
					m_textures[name] = texture;
				} else {
					std::cerr << "Failed to load project texture: " << jtexture.second.ToString() << std::endl;
				}
			}
			m_imageData = zip.read("image");
			m_fileName = fileName;
		}
		return success;
	}
	catch (std::exception &e)
	{
		std::cout << "Failed to load project: " << fileName << std::endl;
		std::cout << e.what() << std::endl;
		return false;
	}
}

const std::string &ProjectData::fileName() const
{
	return m_fileName;
}

void ProjectData::setImageData(const std::string &data)
{
	m_imageData = data;
}

const std::string &ProjectData::getImageData() const
{
	return m_imageData;
}

json ProjectData::toJson() const
{
	return m_json;
}

bool ProjectData::fromJson(const json &j)
{
	m_loaded = false;
	m_fileName.clear();
	m_fonts.clear();
	m_imageData.clear();

	for (auto &jfont : j[ID::engineFonts].ObjectRange())
	{
		auto font = AssetManager<sf::Font>::get("fonts/" + jfont.second.ToString());
		if (font)
			m_fonts[jfont.first] = font;
	}

	m_json = j;
	m_loaded = true;

	return true;
}

const json &ProjectData::data() const
{
	return m_json;
}

json &ProjectData::data()
{
	return m_json;
}

void ProjectData::set(std::shared_ptr<Entity> obj, const std::string &idName)
{
	if (!idName.empty())
		obj->setId(idName);
	else if (obj->getId().empty())
		return;
	data()[obj->entityId()][obj->getId()] = obj->toJson();
}

} // namespace NovelTea
