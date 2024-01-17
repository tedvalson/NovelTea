#ifndef NOVELTEA_ACTIVETEXT_HPP
#define NOVELTEA_ACTIVETEXT_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/Utils.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Time.hpp>

namespace NovelTea
{

class ActiveTextSegment;
class CutsceneTextSegment;
struct AnimationProperties;
struct TextProperties;

using ActiveTextCallback = std::function<void()>;

class ActiveText : public ContextObject, public sf::Drawable, public Hideable
{
public:
	static const int HIGHLIGHTS = 12;

	ActiveText(Context *context);
	ActiveText(Context *context, const std::string &text);
	ActiveText(Context *context, const std::string &text, const AnimationProperties &animDefault);
	ActiveText(Context *context, const std::string &text, const TextProperties &textProps);
	ActiveText(Context *context, const std::string &text, const TextProperties &textProps, const AnimationProperties &animProps);

	static std::shared_ptr<ActiveText> fromCutsceneTextSegment(const CutsceneTextSegment *segment);

	void reset(bool preservePosition = false);
	void skipToNext(bool skipWaitForClick = false);

	void setText(const std::string &text);
	void setText(const std::string &text, const AnimationProperties &animProps);
	void setText(const std::string &text, const TextProperties &textProps);
	void setText(const std::string &text, const TextProperties &textProps, const AnimationProperties &animProps);
	const std::string& getText() const;

	void updateProps(const TextProperties &textProps, const AnimationProperties &animProps);
	void updateProps(const TextProperties &textProps);

	void show(float duration = 0.5f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;
	void hide(float duration = 0.5f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setFontSizeMultiplier(float fontSizeMultiplier);
	float getFontSizeMultiplier() const;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	void setCursorStart(const sf::Vector2f &cursorPos);
	const sf::Vector2f &getCursorPosition() const;
	const sf::Vector2f &getCursorEnd() const;

	void setHighlightFactor(float highlightFactor);
	float getHighlightFactor() const;

	void setLineSpacing(float lineSpacing);
	float getLineSpacing() const;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	bool update(float delta) override;

	std::string toPlainText(bool stripBBCodes = false, const std::string &newlineChar = "\n") const;
	std::string objectFromPoint(const sf::Vector2f &point) const;

	void setHighlightId(const std::string &id);

	bool isAnimating() const;
	bool isComplete() const;
	bool isWaitingForClick() const;
	void click();

	const sf::Time &getTimeToNext() const;
	const SharedVector<ActiveTextSegment>& getSegments() const;

	size_t getDurationMs() const;
	size_t getDurationMs(size_t indexEnd) const;

	size_t getDelayMs() const;
	size_t getDelayMs(size_t indexEnd) const;

	void onComplete(ActiveTextCallback callback);

	ADD_ACCESSOR(bool, SkipWaitingForClick, m_skipWaitingForClick)

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	virtual void setValues(int tweenType, float *newValues) override;
	virtual int getValues(int tweenType, float *returnValues) override;

	void addSegmentToQueue(size_t segmentIndex);
	void buildSegments(const TextProperties &textProps, const AnimationProperties &animProps);
	void updateSegments(float delta);

private:
	float m_alpha;
	float m_fontSizeMultiplier;
	float m_highlightFactor;
	float m_lineSpacing;
	int m_segmentIndex;
	bool m_isAnimating;
	bool m_isComplete;
	bool m_isWaitingForClick;
	bool m_skipWaitingForClick;
	TweenEngine::TweenManager m_tweenManager;
	std::string m_text;
	std::string m_highlightId;
	sf::Vector2f m_size;
	sf::Vector2f m_cursorPos;
	sf::Vector2f m_cursorEnd;
	sf::Vector2f m_cursorStart;
	SharedVector<ActiveTextSegment> m_segments;
	SharedVector<ActiveTextSegment> m_segmentsActive;
	std::shared_ptr<ActiveTextSegment> m_currentSegment;

	ActiveTextCallback m_callback;

	sf::Time m_timePassed;
	sf::Time m_timeToNext;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTIVETEXT_HPP
