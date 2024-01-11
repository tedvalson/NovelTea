#ifndef NOVELTEA_ACTIVETEXTSEGMENT_HPP
#define NOVELTEA_ACTIVETEXTSEGMENT_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/TextTypes.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <vector>
#include <memory>

namespace NovelTea
{

struct StyledSegment;

class ActiveTextSegment : public ContextObject, public JsonSerializable, public sf::Drawable, public Hideable
{
public:
	static const int HIGHLIGHTS = 12;
	static const int FADEACROSS = 13;
	static const int ANIMALPHA = 14;
	static const int EFFECT_FACTOR = 15;
	static const int TREMBLE_FACTOR = 16;
	static const int TEST_FACTOR = 99;

	struct Segment {
		bool objectExists;
		std::string objectIdName;
		TweenText text;
		TextProperties style;
		sf::FloatRect bounds;
	};

	ActiveTextSegment(Context *context);
	ActiveTextSegment(Context *context, const std::vector<std::shared_ptr<StyledSegment>> &segments);
	void createRenderTexture();

	json toJson() const override;
	bool fromJson(const json &j) override;

	std::string toPlainText(bool stripBBCodes = false, const std::string &newlineChar = "\n") const;
	std::string objectFromPoint(const sf::Vector2f &point) const;

	void setStyledSegments(const std::vector<std::shared_ptr<StyledSegment>> &segments);
	void setText(const std::string &text);
	void setText(const std::string &text, const AnimationProperties &animProps);
	void setText(const std::string &text, const TextProperties &textProps);
	void setText(const std::string &text, const TextProperties &textProps, const AnimationProperties &animProps);
	std::string getText() const;

	void startAnim();
	void reset();

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setLastLineMaxHeight(unsigned int height);
	unsigned int getCurrentLineMaxHeight() const;

	void setLastCodePoint(sf::Uint32 codePoint);
	sf::Uint32 getCurrentCodePoint() const;

	void setHighlightId(const std::string &id);
	void refresh();

	float getTextWidth() const;

	size_t getDelayMs() const;
	size_t getDurationMs() const;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	void setLineSpacing(float lineSpacing);
	float getLineSpacing() const;

	void setCursorStart(const sf::Vector2f &cursorPos);
	const sf::Vector2f &getCursorEnd() const;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	void setHighlightFactor(float highlightFactor);
	float getHighlightFactor() const;

	void setFontSizeMultiplier(float fontSizeMultiplier);
	float getFontSizeMultiplier() const;

	void setFadeAcrossPosition(float position);
	float getFadeAcrossPosition() const;
	float getFadeAcrossLength() const;

	void setTrembleFactor(float trembleFactor);
	float getTrembleFactor() const;

	void setTestFactor(float testFactor);
	float getTestFactor() const;

	void setEffectFactor(float effectFactor);
	float getEffectFactor() const;

	std::vector<Segment> &getSegments();

	AnimationProperties &getAnimProps() const;

	bool update(float delta) override;

protected:
	virtual void setValues(int tweenType, float *newValues) override;
	virtual int getValues(int tweenType, float *returnValues) override;

	void setAnimAlpha(float alpha);
	void applyAlpha() const;
	void applyHighlightFactor() const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void ensureUpdate() const;

private:
	std::vector<std::shared_ptr<StyledSegment>> m_styledSegments;
	mutable std::vector<Segment> m_segments;
	mutable std::vector<sf::Vector2f> m_linePositions;
	mutable sf::Vector2f m_cursorPos;
	mutable sf::FloatRect m_bounds;
	mutable unsigned int m_lineMaxCharSize;
	sf::Uint32 m_lastCodePoint;
	sf::Vector2f m_cursorStart;
	sf::Vector2f m_size;
	std::string m_string;
	mutable bool m_needsUpdate;
	float m_lineSpacing;
	float m_alpha;
	float m_animAlpha;
	float m_trembleFactor;
	float m_effectFactor;
	float m_testFactor;
	float m_highlightFactor;
	float m_fontSizeMultiplier;

	// Effect-specific values
	float m_effectIntensity;

	float m_fadeAcrossPosition;
	int m_fadeLineIndex;
	mutable std::shared_ptr<sf::RenderTexture> m_renderTexture;
	mutable std::shared_ptr<sf::Shader> m_shader;
	sf::Sprite m_sprite;
	TweenRectangleShape m_shape;
	TweenRectangleShape m_shapeFade;

	mutable sf::RectangleShape m_debugBorder;
	mutable std::vector<sf::RectangleShape> m_debugSegmentShapes;

	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTIVETEXTSEGMENT_HPP
